#include "precomp.h"
// @(#)$Id$
//
// Evita Common - CommonInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Decimal128Impl.h"

#include <float.h>

#include "../Box.h"
#include "../DebugHelper.h"
#include "../StringBuilder.h"

#include "./Float64Impl.h"

namespace Common {
namespace Numerics {

#include "./DecimalTables.h"

namespace {

// We can classify special values by MSB 7bit of.
static uint32 const kClassMask = 0xFEu << 24;
static uint32 const kSpecialValueMask = 0x78000000u;

static uint32 const kNegativeInfinityPattern = 0xF8u << 24;
static uint32 const kPositiveInfinityPattern = 0x78u << 24;

static uint32 const kNegativeQuietNaNPattern = 0xFCu << 24;
static uint32 const kPositiveQuietNaNPattern = 0x7Cu << 24;

static uint32 const kNegativeSignalingNaNPattern = 0xFEu << 24;
static uint32 const kPositiveSignalingNaNPattern = 0x7Eu << 24;

static uint32 const kNegativeZeroPattern = 0xA2080000u;
static uint32 const kPositiveZeroPattern = 0x22080000u;

// Bit mask for combination field
static uint const kCombinationFieldMask =
    (1 << Decimal128Impl::kCombinationFieldBits) - 1;

// Number of bits from 32bit word boundary of combination field
static int const kCombinationFieldShift = 26;

// Bit mask for exponent continuation field
static uint const kExponentContMask =
    (1 << Decimal128Impl::kExponentContBits) - 1;

// Number of bits from 32bit word boundary for exponent continuation
static int const kExponentContShift = 14;

static int const kDpdInData = 11;

// Number of bits in Densely Packed Decimal (DPD).
static int const kDpdBits = 10;
static int const kDpdMask = (1 << kDpdBits) - 1; // = 0x3FF

// Represents a string of decimal digits. Each decimal digit is stored in
// lower 4bit of byte, higher 4bit is used for pseudo carry/borrow during
// addition/substract.
//
// Example of BCD addition: 17+9=26
// 01 07 + 00 09 = 01 10  addition in binary
// 01 12 + 76 F6 = 78 06  propagate carry in high nibble to upper digit and
//                        add 6 for (a+b >= 16)
// 78 08 & 0F 0F = 08 06  clear carries
// 78 06 & 60 60 = 60 00  prepare for subtract 6.
// 08 06 - 06 00 = 02 06  subtract 6 if no carry
class BcdNum {
  private: static int const kDigits = 36;
  private: static int const kDigitsInWord = 4;
  private: static int const kWords = 9; // RoundUp(36, 4)
  private: uint32 words_[kWords];

  private: static uint const kTenThousdand = 10 * 1000;

  public: BcdNum();
  public: explicit BcdNum(uint32 u32);
  public: explicit BcdNum(uint64 u64);
  public: explicit BcdNum(const Decimal128Impl::EncodedData&);

  // operators
  private: BcdNum& operator =(const BcdNum&);
  public: BcdNum operator +(const BcdNum&) const;

  // properties
  public: int digit_at(int const digit_index) const;
  public: char16 digit_char_at(int const digit_index) const;
  public: void set_digit_at(int const digit_index, int const digit);

  // [A]
  public: Decimal128Impl::EncodedData Add(const BcdNum&, int, int) const;
  public: void AddOne();

  // [C]
  public: int CountDigits() const;

  // [E]
  public: Decimal128Impl::EncodedData Encode(int exponent, int sign) const;

  // [F]
  private: void FillZero();

  // [I]
  private: bool IsOdd() const { return digit_at(0) & 1; }

  // [M]
  private: static uint Mod10(uint n) { return n % 10; }

  // Returns true if we need to bump result following IEEE754 half-even
  // rounding rule.
  public: bool NeedBump(int const digit) const {
    return digit > 5 || (digit == 5 && IsOdd());
  }

  // [P]
  private: static uint32 PackFourDigits(uint32 const u32) {
    return Mod10(u32)
        | (Mod10(u32 / 10) << 8)
        | (Mod10(u32 / 100) << 16)
        | (Mod10(u32 / 1000) << 24);
  }

  // [R]
  public: int RemoveTrailingZeros();
  public: void RoundAt(int const num_result_digits);

  // [S]
  public: void ShiftLeft(int const shift_amount);
  public: void ShiftRight(int const shift_amount);
  public: void ShiftRightWithRound(int const shift_amount);
  public: Decimal128Impl::EncodedData Subtract(const BcdNum&, int, int) const;
};

// Align operands for addition and subtraction.
struct AlignedOperands {
    int exponent;
    BcdNum lhs_bcd;
    BcdNum rhs_bcd;

    AlignedOperands(
        const Decimal128Impl& lhs,
        const Decimal128Impl& rhs)
        : lhs_bcd(lhs.value()),
          rhs_bcd(rhs.value()) {
      DCHECK(lhs.is_finite());
      DCHECK(rhs.is_finite());
      auto const kDigits = Decimal128Impl::kDigits;
      auto const lhs_exp = lhs.exponent();
      auto const rhs_exp = rhs.exponent();
      exponent = min(lhs_exp, rhs_exp);
      if (lhs_exp > rhs_exp) {
        auto const lhs_num_digits = lhs_bcd.CountDigits();
        if (lhs_num_digits) {
          auto const lhs_shift = lhs_exp - rhs_exp;
          auto const overflow = lhs_num_digits + lhs_shift - (kDigits + 1);
          if (overflow <= 0) {
            lhs_bcd.ShiftLeft(lhs_shift);
          } else {
            lhs_bcd.ShiftLeft(lhs_shift - overflow);
            rhs_bcd.ShiftRightWithRound(overflow);
            exponent += overflow;
          }
        }

      } else if (lhs_exp < rhs_exp) {
        auto const rhs_num_digits = rhs_bcd.CountDigits();
        if (rhs_num_digits) {
          auto const rhs_shift = rhs_exp - lhs_exp;
          auto const overflow = rhs_num_digits + rhs_shift - (kDigits + 1);
          if (overflow <= 0) {
            rhs_bcd.ShiftLeft(rhs_shift);
          } else {
            rhs_bcd.ShiftLeft(rhs_shift - overflow);
            lhs_bcd.ShiftRightWithRound(overflow);
            exponent += overflow;
          }
       }
     }
   }

   DISALLOW_COPY_AND_ASSIGN(AlignedOperands);
};

// BinNum represent positive integer in multiple precision number using
// one billion base to keep at least 34 digits in decimal128.
//
// We use base one billion number for faster conversion from decimal128 to
// BinNum by converting three digits into each multiple precision number
// element to avoid converting decimal128 to binary number.
//
// This class is used of calculating division and multiply.
//
//  "lsu" stands for least significant unit.
//  "msu" stands for most significant unit.
class BinNum {
  private: static uint const kBillion = 1000 * 1000 * 1000;
  private: static uint const kFourBillion = uint(kBillion) * 4;
  private: static uint const kMillion = 1000 * 1000;

  private: static int const kDigitsInUnit = 9;
  private: static int const kUnits = 4; // RoundUp(kDigits, 9)
  private: static int const kDigits = kDigitsInUnit * kUnits;

  private: class DivisionAccumulator {
    private: uint32* lsu_ptr_;
    private: uint32* lsu_min_ptr_;
    private: uint32* msu_ptr_;
    private: uint32 units_[kUnits * 3];

    public: DivisionAccumulator(const BinNum&, const BinNum&);

    public: uint32* lsu_ptr() const { return lsu_ptr_; }
    public: uint32* lsu_min_ptr() const { return lsu_min_ptr_; }
    public: uint32* msu_ptr() const { return msu_ptr_; }
    public: uint32 msu(int k) const { return msu_ptr_[-k]; }
    public: int num_units() const { return msu_ptr_ - lsu_ptr_ + 1; }
    public: void set_msu(int k, uint32 unit) { msu_ptr_[k] = unit; }

    public: int CompareTo(const BinNum& rhs) const;
    public: void Extend();
    public: bool HasMoreUnit() const;
    public: bool is_zero() const;
    public: void MultiplyAndSubtract(const BinNum&, uint32);
    public: void RemoveLeadingZeros();
    public: void Zero();
  };

  // Divide quotient needs kUnits + 1.
  // Mulitply product needs kUnits * 2.
  // units_[0] contains least significant digits.
  private: int num_units_;
  private: uint32 units_[kUnits * 2];

  // ctor
  private: explicit BinNum(int num_units);
  public: explicit BinNum(const Decimal128Impl::EncodedData&);

  // properties
  public: int digit_at(int) const;
  public: uint32 msu(int k) const;
  public: int num_units() const { return num_units_; }

  // [C]
  private: static uint32 CalculateEstimatedDivisor(const BinNum&);
  public: int ConvertTo(BcdNum&) const;

  // [D]
  public: Decimal128Impl Divide(
      const BinNum& rhs,
      int exponent,
      int quotient_sign,
      DivideResult const result) const;

  private: static uint32 Divide(uint32 high, uint32 low, uint64 divisor);

  // [F]
  private: void FillZero();

  // [M]
  public: Decimal128Impl Multiply(
      const BinNum& rhs,
      int const exponent,
      int const result_sign) const;

  // [P]
  private: static uint32 PackDpdInBillion(uint, uint, uint);

  // [S]
  private: void ShiftRigh(int shift_amount);

  DISALLOW_COPY_AND_ASSIGN(BinNum);
};

class SpecialValueHandler {
  public: enum HandleResult {
    BothFinite,
    BothInfinity,
    EitherNaN,
    LhsIsInfinity,
    RhsIsInfinity,
  };

  private: enum Result {
    Lhs,
    Lhs_QuietNaN,
    Rhs,
    Rhs_QuietNaN,
  };

  private: const Decimal128Impl& lhs_;
  private: Result result_;
  private: const Decimal128Impl& rhs_;

  public: SpecialValueHandler(
    const Decimal128Impl& lhs,
    const Decimal128Impl& rhs)
        : lhs_(lhs), result_(Lhs), rhs_(rhs) {}

  public: Decimal128Impl value() const {
    switch (result_) {
      case Lhs:
        return lhs_;
      case Lhs_QuietNaN:
        return Decimal128Impl::QuietNaN(lhs_.sign());
      case Rhs:
        return rhs_;
      case Rhs_QuietNaN:
        return Decimal128Impl::QuietNaN(rhs_.sign());
      default:
        return Decimal128Impl::SignalingNaN(lhs_.sign());
    }
  }

  public: HandleResult Handle() {
    if (lhs_.is_finite() && rhs_.is_finite()) {
      return BothFinite;
    }

    auto const lhs_class = lhs_.ieee_class();
    auto const rhs_class = rhs_.ieee_class();
    if (lhs_class == Class_SignalingNaN) {
      result_ = Lhs_QuietNaN;
      return EitherNaN;
    }

    if (rhs_class == Class_SignalingNaN) {
      result_ = Rhs_QuietNaN;
      return EitherNaN;
    }

    if (lhs_class == Class_QuietNaN) {
      result_ = Lhs;
      return EitherNaN;
     }

    if (rhs_class == Class_QuietNaN) {
      result_ = Rhs;
      return EitherNaN;
     }

     if (lhs_class == Class_Infinity) {
       return rhs_class == Class_Infinity ? BothInfinity : LhsIsInfinity;
     }

     if (rhs_class == Class_Infinity) {
       return RhsIsInfinity;
     }

     return BothFinite;
  }

  DISALLOW_COPY_AND_ASSIGN(SpecialValueHandler);
};

BcdNum::BcdNum() {
  FillZero();
}

// Maximum value of uint32 is 4,294,967,295 (10 digits).
BcdNum::BcdNum(uint32 u32) {
  FillZero();
  auto i = 0u;
  while (u32 > 0) {
    DCHECK_LE(i, ARRAYSIZE(words_));
    words_[i] = PackFourDigits(u32);
    u32 /= kTenThousdand;
    ++i;
  }
}

// Maximum value of uint64 is 18,446,744,073,709,551,615 (20 digit).
BcdNum::BcdNum(uint64 u64) {
  FillZero();
  auto i = 0u;
  while (u64 > 0) {
    DCHECK_LE(i, ARRAYSIZE(words_));
    words_[i] = PackFourDigits(uint32(u64 % kTenThousdand));
    u64 /= kTenThousdand;
    ++i;
  }
}

// Convert decimal128 encoded data into BcdNum.
// decimal128 has 11 DPD and 1 digit in combination field.
BcdNum::BcdNum(const Decimal128Impl::EncodedData& data) {
  FillZero();

  auto num_digits = 0;
  for (auto i = 0; i < kDpdInData; ++i) {
    auto const dpd = data.dpd_at(i);
    DCHECK_LT(uint(dpd), ARRAYSIZE(kDpdToPackedBcd3));
    auto packed_bcd3 = kDpdToPackedBcd3[dpd];
    for (auto k = 0; k < 3; ++k) {
      auto const word_index = num_digits / kDigitsInWord;
      auto const shift = num_digits % kDigitsInWord * 8;
      words_[word_index] |= (packed_bcd3 & 0xF) << shift;
      packed_bcd3 >>= 4;
      ++num_digits;
    }
  }

  auto const word_index = num_digits / kDigitsInWord;
  auto const shift = num_digits % kDigitsInWord * 8;
  words_[word_index] |= data.coefficent_msd() << shift;
}

int BcdNum::digit_at(int const digit_index) const {
  auto const word_index = digit_index / kDigitsInWord;
  DCHECK_LT(uint(word_index), ARRAYSIZE(words_));
  auto const shift = (digit_index % kDigitsInWord) * 8;
  return (words_[word_index] >> shift) & 0xF;
}

char16 BcdNum::digit_char_at(int const digit_index) const {
  return char16(digit_at(digit_index) + '0');
}

void BcdNum::set_digit_at(int const digit_index, int const digit) {
  DCHECK_LE(uint(digit), 9u);
  auto const word_index = digit_index / kDigitsInWord;
  DCHECK_LT(uint(word_index), ARRAYSIZE(words_));
  auto const shift = (digit_index % kDigitsInWord) * 8;
  words_[word_index] &= ~(0x0F << shift);
  words_[word_index] |= digit << shift;
}

Decimal128Impl::EncodedData BcdNum::Add(
    const BcdNum& rhs,
    int const exponent,
    int const sign) const {
  BcdNum result;
  uint carry = 0;
  for (auto i = 0 ; i < ARRAYSIZE(result.words_); i++) {
    carry += words_[i] + rhs.words_[i];
    carry += 0x76F6F6F6; // BCD adjustment
    auto digits = carry & 0x0F0F0F0F; // clear carries
    digits -= (carry & 0x60606060) >> 4; // BCD adjustment
    result.words_[i] = digits;
    carry >>= 31;
  }
  // We don't care carry here it comes at kDigits - 1.
  return result.Encode(exponent,  sign);
}

// Destructive addition by one. This method is used for rounding.
void BcdNum::AddOne() {
  uint carry = 1;
  for (auto i = 0 ; i < ARRAYSIZE(words_); i++) {
    carry += words_[i];
    carry += 0x76F6F6F6; // BCD adjustment
    auto digits = carry & 0x0F0F0F0F; // clear carries
    digits -= (carry & 0x60606060) >> 4; // BCD adjustment
    words_[i] = digits;
    carry >>= 31;
  }
}

// Returns number of digits represented by this BcdNum instance. If this
// object representes 0, this method returns 0.
int BcdNum::CountDigits() const {
  for (auto num_digits = kDigits; num_digits; --num_digits) {
    if (digit_at(num_digits - 1)) {
      return num_digits;
    }
  }
  return 0;
}

Decimal128Impl::EncodedData BcdNum::Encode(
    int const exponent,
    int const result_sign) const {
  BcdNum result(*this);
  auto num_digits = result.CountDigits();
  auto result_exponent = exponent;

  auto const num_drop_digits = max(
      num_digits - Decimal128Impl::kPrecision,
      Decimal128Impl::kEtiny - exponent);

  if (num_drop_digits > 0) {
    result_exponent += num_drop_digits;
    auto need_bump = false;
    if (num_drop_digits < num_digits) {
      need_bump = result.NeedBump(result.digit_at(num_drop_digits - 1));
      result.ShiftRight(num_drop_digits);

    } else {
      // Discard all digits.
      if (num_drop_digits == num_digits) {
        need_bump = result.NeedBump(result.digit_at(num_digits - 1));
      }
      result.ShiftRight(num_digits);
    }

    if (need_bump) {
      result.AddOne();
      if (result.digit_at(Decimal128Impl::kPrecision)) {
        ++result_exponent;
        result.ShiftRight(1);
      }
    }
  }

  if (result_exponent > Decimal128Impl::kEbig) {
    num_digits = result.CountDigits();
    if (!num_digits) {
      return result.Encode(Decimal128Impl::kEbig, result_sign);
    }

    if (result_exponent + num_digits - 1 > Decimal128Impl::kEmax) {
      return Decimal128Impl::Infinity(result_sign).value();
    }

    auto const shift = result_exponent - Decimal128Impl::kEbig;
    if (shift > 0) {
      result.ShiftLeft(shift);
      result_exponent -= shift;
    }
  }

  DCHECK_LE(result.CountDigits(), Decimal128Impl::kPrecision);
  DCHECK_LE(uint(result_exponent + Decimal128Impl::kEbias),
            uint(Decimal128Impl::kElimit));

  Decimal128Impl::EncodedData data;
  for (auto i = 0; i < kDpdInData * 3; i += 3) {
    auto const packed_bcd3 = result.digit_at(i)
          | (result.digit_at(i + 1) << 4 )
          | (result.digit_at(i + 2) << 8);
    DCHECK_LE(uint(packed_bcd3), 0x999u);
    auto const dpd = kPackedBcd3ToDpd[packed_bcd3];
    DCHECK_LE(uint(dpd), 0x3FFu);
    DCHECK(packed_bcd3 == 0 || dpd > 0);
    data.set_dpd_at(i / 3, dpd);
  }

  auto const msd = result.digit_at(Decimal128Impl::kPrecision - 1);
  data.set_combination_field(kMsdToCombinationField[msd]);
  data.set_exponent(result_exponent);
  data.set_sign(result_sign);

  return data;
}

void BcdNum::FillZero() {
  ::ZeroMemory(words_, sizeof(words_));
}

// [R]
int BcdNum::RemoveTrailingZeros() {
  auto num_zeros = 0;
  while (num_zeros < kDigits && !digit_at(num_zeros)) {
    ++num_zeros;
  }

  if (num_zeros) {
    ShiftRight(num_zeros);
  }

  return num_zeros;
}

void BcdNum::RoundAt(int const num_result_digits) {
  DCHECK_GE(num_result_digits, 1);
  auto const num_digits = CountDigits();
  auto const num_extra_digits = num_digits - num_result_digits;
  if (num_extra_digits > 0) {
    ShiftRightWithRound(num_extra_digits);
  }
}

// [S]
void BcdNum::ShiftLeft(int const shift_amount) {
  DCHECK_GE(shift_amount, 0);
  DCHECK_LT(shift_amount, kDigits);

  auto const word_count = shift_amount / kDigitsInWord;
  auto const byte_count = shift_amount % kDigitsInWord;

  if (word_count) {
    int j = kWords - 1;
    while (j >= word_count) {
      words_[j] = words_[j - word_count];
      --j;
    }
    while (j >= 0) {
      words_[j] = 0;
      --j;
    }
  }

  if (byte_count) {
    auto const left_shift = byte_count * 8;
    auto const right_shift = 32 - left_shift;
    auto carry = 0;
    for (auto j = 0; j < kWords; j++) {
      auto const new_carry = words_[j] >> right_shift;
      words_[j] <<= left_shift;
      words_[j] |= carry;
      carry = new_carry;
    }
  }
}

void BcdNum::ShiftRight(int const shift_amount) {
  DCHECK_GE(shift_amount, 0);
  DCHECK_LT(shift_amount, kDigits);

  if (!shift_amount) {
    return;
  }

  auto const word_count = shift_amount / kDigitsInWord;
  auto const byte_count = shift_amount % kDigitsInWord;

  if (word_count) {
    int j = 0;
    while (j + word_count < ARRAYSIZE(words_)) {
      words_[j] = words_[j + word_count];
      ++j;
    }
    while (j < ARRAYSIZE(words_)) {
      words_[j] = 0;
      ++j;
    }
  }

  if (byte_count) {
    auto const right_shift = byte_count * 8;
    auto const left_shift = 32 - right_shift;
    auto const mask = (1 << right_shift) - 1;
    auto carry = 0;
    for (auto j = kWords - 1; j >= 0; --j) {
      auto const new_carry = words_[j] & mask;
      words_[j] >>= right_shift;
      words_[j] |= carry << left_shift;
      carry = new_carry;
    }
  }
}

// Shift digits to right by specified shift amount and rounding at
// discarded digit.
void BcdNum::ShiftRightWithRound(int const shift_amount) {
  DCHECK_GE(shift_amount, 0);
  if (!shift_amount) {
    return;
  }

  auto const num_digits = CountDigits();
  auto const num_new_digits = num_digits - shift_amount;
  if (num_new_digits > 0) {
    auto const need_bump = NeedBump(digit_at(shift_amount - 1));
    ShiftRight(shift_amount);
    if (need_bump) {
      AddOne();
    }
    return;
  }

  if (!num_new_digits) {
    auto const need_bump = NeedBump(digit_at(num_digits - 1));
    FillZero();
    if (need_bump) {
      set_digit_at(0, 1);
    }
    return;
  }

  // num_new_digits < 0
  FillZero();
}

Decimal128Impl::EncodedData BcdNum::Subtract(
    const BcdNum& rhs,
    int const exponent,
    int const sign) const {
  uint carry = 1;
  BcdNum result;
  for (auto i = 0 ; i < ARRAYSIZE(result.words_); i++) {
    carry += words_[i] + 0x09090909 - rhs.words_[i];
    carry += 0x76F6F6F6; // BCD adjustment
    auto digits = carry & 0x0F0F0F0F; // clear carries
    digits -= (carry & 0x60606060) >> 4; // BCD adjustment
    result.words_[i] = digits;
    carry >>= 31;
  }

  if (!carry) {
    // lhs < rhs: negate result
    uint carry = 1;
    for (auto i = 0 ; i < ARRAYSIZE(result.words_); i++) {
      carry += 0x09090909 - result.words_[i];
      carry += 0x76F6F6F6; // BCD adjustment
      auto digits = carry & 0x0F0F0F0F; // clear carries
      digits -= (carry & 0x60606060) >> 4; // BCD adjustment
      result.words_[i] = digits;
      carry >>= 31;
    }
    return result.Encode(exponent, sign ^ 1);
  }

  return result.Encode(exponent,  sign);
}

BinNum::DivisionAccumulator::DivisionAccumulator(
    const BinNum& lhs,
    const BinNum& rhs) {
  ::ZeroMemory(units_, sizeof(units_));
  msu_ptr_ = units_ + ARRAYSIZE(units_) - 1;
  lsu_min_ptr_ = msu_ptr_ - lhs.num_units() + 1;
  lsu_ptr_ = units_ + ARRAYSIZE(units_) - rhs.num_units();
  ::CopyMemory(
      lsu_min_ptr_,
      lhs.units_,
      sizeof(*units_) * lhs.num_units());
}

int BinNum::DivisionAccumulator::CompareTo(const BinNum& rhs) const {
  if (auto const diff_units = num_units() - rhs.num_units()) {
    return diff_units;
  }

  auto acc_ptr = msu_ptr_;
  auto rhs_ptr = rhs.units_ + rhs.num_units() - 1;
  while (rhs_ptr >= rhs.units_) {
    if (auto const diff = *acc_ptr - *rhs_ptr) {
      return diff;
    }
    --acc_ptr;
    --rhs_ptr;
  }
  return 0;
}

void BinNum::DivisionAccumulator::Extend() {
  DCHECK_GT(lsu_ptr_, units_);
  --lsu_ptr_;
}

bool BinNum::DivisionAccumulator::HasMoreUnit() const {
  return lsu_ptr_ > lsu_min_ptr_;
}

bool BinNum::DivisionAccumulator::is_zero() const {
  return msu_ptr_ == lsu_ptr_ && !*msu_ptr_;
}

// Fused multiply and subtract for calculating
//   accumulator -= divisor * quotient
void BinNum::DivisionAccumulator::MultiplyAndSubtract(
    const BinNum& divisor,
    uint32 const quotient) {
  DCHECK_NE(quotient, 0u);
  auto acc_ptr = lsu_ptr_;
  auto borrow = uint32(0);
  for (auto i = 0; i < divisor.num_units(); ++i) {
    auto const sub64 = uint64(divisor.units_[i]) * quotient + borrow;
    borrow = uint32(sub64 / kBillion);
    auto const sub32 = uint32(sub64 % kBillion);
    DCHECK_LE(acc_ptr, msu_ptr_);
    if (sub32 > *acc_ptr) {
      *acc_ptr += kBillion - sub32;
      ++borrow;
    } else {
      *acc_ptr -= sub32;
    }
    DCHECK_LT(*acc_ptr, uint(kBillion));
    acc_ptr++;
  }

  if (borrow) {
    DCHECK_LE(acc_ptr, msu_ptr_);
    DCHECK_GE(*acc_ptr, borrow);
    *acc_ptr -= borrow;
  }
}

void BinNum::DivisionAccumulator::RemoveLeadingZeros() {
  while (msu_ptr_ > lsu_ptr_ && !*msu_ptr_) {
    --msu_ptr_;
  }
}

void BinNum::DivisionAccumulator::Zero() {
  msu_ptr_ = lsu_ptr_;
  *msu_ptr_ = 0;
}

// num_units can be zero when 0 x 0.
BinNum::BinNum(int num_units) {
  DCHECK_GE(num_units, 0);
  DCHECK_LE(num_units, int(ARRAYSIZE(units_)));
  num_units_ = num_units;
  FillZero();
}

// Convert a significant of IEEE754 decimal128 encoded data into billion
// base multiple precision integer. Each unit can hold 9 deccimal digits.
// 4 units (4x9=36 digits) can hold 34 digits in IEEE754 decimal128.
BinNum::BinNum(const Decimal128Impl::EncodedData& data) {
  FillZero();

  auto k = 0;
  for (auto i = 0; i < kUnits - 1; ++i) {
    units_[i] = PackDpdInBillion(
        data.dpd_at(k),
        data.dpd_at(k + 1),
        data.dpd_at(k + 2));
    k += 3;
  }

  units_[3] = kDpdTo999[data.dpd_at(k)]
      + kDpdTo999[data.dpd_at(k + 1)] * 1000
      + data.coefficent_msd() * 1000 * 1000;

  // Remove leading zeros
  num_units_ = 4;
  while (num_units_ > 0) {
    if (units_[num_units_ - 1]) {
      break;
    }
    --num_units_;
  }
}

// digit_index = 0 == least significant digit
int BinNum::digit_at(int const digit_index) const {
  DCHECK_LT(uint(digit_index), uint(num_units_ * kDigitsInUnit));
  auto const unit_index = digit_index / kDigitsInUnit;
  auto const digit_column = digit_index % kDigitsInUnit;
  return (units_[unit_index] / kPowers[digit_column]) % 10;
}

// Reader of nth unit from most significant unit.
uint32 BinNum::msu(int const k) const {
  DCHECK_GE(k, 0);
  DCHECK_LT(k, num_units());
  return units_[num_units() - k - 1];
}

// Calculate estimated quotient from most significand unit and most
// signficant 2bit from next unit of divisor.
uint32 BinNum::CalculateEstimatedDivisor(const BinNum& divisor) {
  auto estimate = divisor.msu(0) * 4;
  if (divisor.num_units() == 1) {
    return estimate;
  }

  auto rest = divisor.msu(1);
  if (divisor.msu(1) >= 750 * kMillion) {
    estimate += 3;
    rest -= 750 * kMillion;

  } else if (divisor.msu(1) >= 500 * kMillion) {
    estimate += 2;
    rest -= 500 * kMillion;

  } else if (divisor.msu(1) >= 250 * kMillion) {
    ++estimate;
    rest -= 250 * kMillion;
  }

  if (rest) {
    return estimate + 1;
  }

  for (auto i = divisor.num_units() - 2; i >= 0; --i) {
    if (divisor.units_[i]) {
      ++estimate;
      break;
    }
  }

  return estimate;
}

// Convert BinNum from most significant unit to least signficant unit into
// BcdNum and returns number of digits not to store into BcdNum.
int BinNum::ConvertTo(BcdNum& result) const {
  // Skip leading zeros
  auto bin_digit_index = num_units() * kDigitsInUnit - 1;
  while (bin_digit_index > 0) {
    if (digit_at(bin_digit_index)) {
      break;
    }
    --bin_digit_index;
  }

  // Take kDigit+1 from BinNum.
  auto bcd_digit_index = Decimal128Impl::kDigits;
  while (bcd_digit_index >= 0 && bin_digit_index >= 0) {
    auto const digit = digit_at(bin_digit_index);
    result.set_digit_at(bcd_digit_index, digit);
    --bin_digit_index;
    --bcd_digit_index;
  }

  if (bin_digit_index < 0) {
    // We took all digits from bin.
    result.ShiftRight(bcd_digit_index + 1);
  } else {
    result.ShiftRightWithRound(bcd_digit_index + 1);
  }

  return bin_digit_index + 1;
}

// Divide base billion integer by base billion integer.
uint32 BinNum::Divide(
      uint32 const high,
      uint32 const low,
      uint64 const divisor) {
  return uint32((uint64(high) * kBillion + low) / divisor);
}

// Divide BinNum by BinNum. divisor must not be zero.
Decimal128Impl BinNum::Divide(
    const BinNum& divisor,
    int const quotient_exponent,
    int const quotient_sign,
    DivideResult const quotient_type) const {
  auto& dividend = *this;
  auto const divisor64 =
    divisor.num_units() > 2
        ? uint64(divisor.msu(0)) * kBillion + divisor.msu(1) + 1
        : divisor.num_units() > 1
            ? uint64(divisor.msu(0)) * kBillion + divisor.msu(1)
            : 0;

  auto const estimated_divisor = CalculateEstimatedDivisor(divisor);
  DCHECK_GT(estimated_divisor, 0u);
  auto const inverse_estimated_divisor = kFourBillion / estimated_divisor;

  BinNum quotient(kUnits + 1);
  auto quotient_unit_index = quotient.num_units() - 1;

  // Generate quotient from most significant unit to least significant unit
  // until:
  //  o Get enough units of quotient, or
  //  o The accumulator is smaller than divisor.
  // At the end of loop, accumulator contains remainder.
  DivisionAccumulator accumulator(dividend, divisor);
  for (;;) {
    // Subtract quotient times divisor from accumlator until accumulator
    // is greater than divisor.
    for (;;) {
      accumulator.RemoveLeadingZeros();
      if (accumulator.is_zero()) {
        break;
      }

      auto const diff = accumulator.CompareTo(divisor);
      if (diff < 0) {
        break;
      }

      if (!diff) {
        quotient.units_[quotient_unit_index] += 1;
        accumulator.Zero();
        break;
      }

      uint32 const partial_quotient =
        accumulator.num_units() == divisor.num_units()
            ? divisor.num_units() > 1
                ? Divide(accumulator.msu(0), accumulator.msu(1), divisor64)
                : accumulator.msu(0) / divisor.msu(0)
        : accumulator.msu(0) * inverse_estimated_divisor;

      auto const increment_amount = max(partial_quotient, 1u);
      quotient.units_[quotient_unit_index] += increment_amount;
      accumulator.MultiplyAndSubtract(divisor, increment_amount);
    }

    // quotient.msu(0) can be zero if dividend is smaller than divisor,
    // e.g. 1/2.
    if (quotient.msu(0)) {
      --quotient_unit_index;
      if (quotient_unit_index < 0) {
        break;
      }
    }

    if (!accumulator.msu(0) && !accumulator.HasMoreUnit()) {
      accumulator.RemoveLeadingZeros();
      if (accumulator.is_zero()) {
        break;
      }
    }

    // virtual accumulator *= kBillion
    accumulator.Extend();
  }

  quotient.ShiftRigh(quotient_unit_index + 1);

  BcdNum result;
  auto const overflow = quotient.ConvertTo(result);
  auto result_exponent = quotient_exponent + overflow;

  auto const k = accumulator.lsu_min_ptr() - accumulator.lsu_ptr();
  if (k > 0) {
    result_exponent -= k * kDigitsInUnit;
    if (!accumulator.msu(0)) {
      // When quotient is exact, there may be up to 8 extra trailing
      // zeros.
      result_exponent += result.RemoveTrailingZeros();
    }
  }

  switch (quotient_type) {
    case FloatResult:
      return Decimal128Impl(result.Encode(result_exponent, quotient_sign));

    case IntegerResult: {
      auto const num_quotient_digits = result.CountDigits();
      if (num_quotient_digits + result_exponent > Decimal128Impl::kDigits) {
        // Quotient is too large for integer.
        return Decimal128Impl::QuietNaN(0);
      }

      if (result_exponent >= 0) {
        result.ShiftLeft(result_exponent);
        return Decimal128Impl(result.Encode(0, quotient_sign));
      }

      // Drop fraction of quotient.
      auto const num_drop = -result_exponent;
      if (num_quotient_digits > num_drop) {
        result.ShiftRight(num_drop);
        return Decimal128Impl(result.Encode(0, quotient_sign));
      }

      return Decimal128Impl::Zero(quotient_sign);
    }

    default:
      return Decimal128Impl::QuietNaN(0);
  }
}

void BinNum::FillZero() {
  ::ZeroMemory(units_, sizeof(units_));
}

// Multiply lhs by rhs into product.
Decimal128Impl BinNum::Multiply(
    const BinNum& rhs,
    int const product_exponent,
    int const result_sign) const {
  BinNum product(num_units() + rhs.num_units());
  auto product_unit_index = 0;
  for (auto i = 0; i < num_units(); i++) {
    uint32 carry = 0;
    auto k = product_unit_index;
    for (auto j = 0; j < rhs.num_units(); j++) {
      auto const u64 = uint64(units_[i]) * rhs.units_[j]
          + carry + product.units_[k];
      carry = uint32(u64 / kBillion);
      product.units_[k] = uint32(u64 % kBillion);
      ++k;
    }
    product.units_[k] = carry;
    ++product_unit_index;
  }

  BcdNum result;
  auto const overflow = product.ConvertTo(result);
  auto const result_exponent = product_exponent + overflow;

  // Enoce method will round kDigit+1 digits to kDigit.
  return Decimal128Impl(result.Encode(result_exponent, result_sign));
}

uint32 BinNum::PackDpdInBillion(uint dpd0, uint dpd1, uint dpd2) {
  auto const a = kDpdTo999[dpd0];
  auto const b = kDpdTo999[dpd1];
  auto const c = kDpdTo999[dpd2];
  return a + (b * 1000) + (c * kMillion);
}

void BinNum::ShiftRigh(int shift_amount) {
  DCHECK_GE(shift_amount, 0);
  DCHECK_LE(shift_amount, num_units_);
  auto i = 0;
  while (i + shift_amount < num_units_) {
    units_[i] = units_[i + shift_amount];
    ++i;
  }
  while (i < num_units_) {
    units_[i] = 0;
    ++i;
  }
  num_units_ -= shift_amount;
}

template<typename T>
static T Power(T const x, uint n) {
  T y = 1;
  T z = x;
  for (;;) {
    if (n & 1) {
      y = y * z;
    }

    n >>= 1;
    if (!n) {
      return y;
    }

    z = z * z;
  }
}

} // namespace

int Decimal128Impl::EncodedData::coefficent_msd() const {
  return kCombinationFieldMap[combination_field()].coefficent;
}

int Decimal128Impl::EncodedData::combination_field() const {
  return (words_[3] >> 26) & kCombinationFieldMask;
}

// Extract DPD(10bit word) from 32bit word vector.
int Decimal128Impl::EncodedData::dpd_at(int const dpd_index) const {
  DCHECK_GE(dpd_index, 0);
  DCHECK_LE(dpd_index, kDpdInData);
  auto const bit_index = dpd_index * kDpdBits;
  auto const word_index = bit_index / 32;
  auto const low_shift = bit_index % 32;

  if (low_shift + kDpdBits <= 32) {
    // DPB in a word.
    return (words_[word_index] >> low_shift) & kDpdMask;
  }

  // DPB in two words.
  auto const high_shift = 32 - low_shift;
  return ((words_[word_index] >> low_shift)
            | (words_[word_index + 1] << high_shift))
                & kDpdMask;
}

int Decimal128Impl::EncodedData::exponent_biased() const {
  return (exponent_msb() << kExponentContBits) | exponent_continuation();
}

int Decimal128Impl::EncodedData::exponent_continuation() const {
  return (words_[3] >> kExponentContShift) & kExponentContMask;
}

int Decimal128Impl::EncodedData::exponent_msb() const {
  return kCombinationFieldMap[combination_field()].exponent;
}

bool Decimal128Impl::EncodedData::is_finite() const {
  return !is_special();
}

bool Decimal128Impl::EncodedData::is_special() const {
  return (words_[3] & kSpecialValueMask) == kSpecialValueMask;
}

bool Decimal128Impl::EncodedData::is_zero() const {
  return !words_[0] && !words_[1] && !words_[2]
    // Check coefficient msd and not special value
    && !(words_[3] & 0x1C003FFF)
    // Check coefficient msd
    && (words_[3] & 0x60000000) != 0x60000000;
}

int Decimal128Impl::EncodedData::sign() const {
  return int(words_[3]) < 0;
}

void Decimal128Impl::EncodedData::set_combination_field(int const value) {
  DCHECK_LE(uint(value), 0x1Fu);
  words_[3] &= ~(kCombinationFieldMask << kCombinationFieldShift);
  words_[3] |= value << kCombinationFieldShift;
}

void Decimal128Impl::EncodedData::set_combination_field(
    int const exponent_msb,
    int const coefficient_msd) {
  DCHECK_LE(uint(coefficient_msd), 9u);
  DCHECK_LE(uint(exponent_msb), 2u);
  auto const value = coefficient_msd < 8
    ? (exponent_msb << 3) | coefficient_msd
    : 0x18 | (exponent_msb << 1) | coefficient_msd;
  set_combination_field(value);
}

void Decimal128Impl::EncodedData::set_dpd_at(int const dpd_index, int const dpd) {
  DCHECK_GE(dpd_index, 0);
  DCHECK_LE(dpd_index, kDpdInData);
  DCHECK_LE(uint(dpd), 0x3FFu);
  auto const bit_index = dpd_index * kDpdBits;
  auto const word_index = bit_index / 32;
  auto const low_shift = bit_index % 32;

  // Set lower patt
  words_[word_index] &= ~(kDpdMask << low_shift);
  words_[word_index] |= dpd << low_shift;

  if (low_shift + kDpdBits > 32) {
    // Set hight part
    auto const high_shift = 32 - low_shift;
    words_[word_index + 1] &= ~(kDpdMask >> high_shift);
    words_[word_index + 1] |= dpd >> high_shift;
  }
}

void Decimal128Impl::EncodedData::set_exponent(int const exponent) {
  auto const biased = exponent + kEbias;
  set_exponent_continuation(biased & kExponentContMask);
  set_combination_field(
      (biased >> kExponentContBits) & 3,
      coefficent_msd());
}

void Decimal128Impl::EncodedData::set_exponent_continuation(int const value) {
  DCHECK_LE(uint(value), kExponentContMask);
  words_[3] &= ~(kExponentContMask << kExponentContShift);
  words_[3] |= value << kExponentContShift;
}

void Decimal128Impl::EncodedData::set_sign(int value) {
  if (value) {
    words_[3] |= 1u << 31;
  } else {
    words_[3] &= (1u << 31) - 1;
  }
}

Decimal128Impl::Decimal128Impl() {
  ::ZeroMemory(data_.words_, sizeof(data_.words_));
}

// Construct special value.
Decimal128Impl::Decimal128Impl(
    int const sign,
    uint32 const positive_high_word,
    uint32 const negative_high_word) {
  data_.words_[0] = 0;
  data_.words_[1] = 0;
  data_.words_[2] = 0;
  data_.words_[3] = sign ? negative_high_word : positive_high_word;
}

Decimal128Impl::Decimal128Impl(int const sign, uint64 const u64)
    : data_(BcdNum(u64).Encode(0, sign)) {}

// Convert float64 into Decimal128.
Decimal128Impl::Decimal128Impl(float64 const f64) {
  Float64Impl value(f64);
  switch (value.ieee_class()) {
    case Class_Infinity:
      *this = Infinity(value.sign());
      return;

    case Class_Normal: {
      Decimal128Impl signficand(value.sign(), value.significand());
      auto const exponent = value.exponent() - Float64Impl::kSignificandBits;
      if (!exponent) {
        *this = signficand;
        return;
      }

      if (exponent > 0) {
        auto const power2 = Power(Decimal128Impl(2), exponent);
        *this = (signficand * power2).RoundAt(Float64Impl::kDigits);
        return;
      }

      auto const power2 = Power(Decimal128Impl(2), -exponent);
      *this = (signficand / power2).RoundAt(Float64Impl::kDigits);
      return;
    }

    case Class_QuietNaN:
      *this = QuietNaN(value.sign());
      return;

    case Class_SignalingNaN:
      *this = SignalingNaN(value.sign());
      return;

    case Class_Subnormal: {
      Decimal128Impl signficand(value.sign(), value.raw_significand());
      auto const power2 = Power(Decimal128Impl(2), -Float64Impl::kEsubnormal);
      *this = (signficand / power2).RoundAt(Float64Impl::kDigits);
      return;
    }

    case Class_Zero:
      *this = Zero(value.sign());
      return;

    default:
      CAN_NOT_HAPPEN();
  }
}

Decimal128Impl::Decimal128Impl(int32 const i32)
    : data_(i32 >= 0
                ? BcdNum(uint32(i32)).Encode(0, 0)
                : BcdNum(uint32(-i32)).Encode(0, 1)) {}

Decimal128Impl::Decimal128Impl(int64 const i64)
    : data_(i64 >= 0
                ? BcdNum(uint64(i64)).Encode(0, 0)
                : BcdNum(uint64(-i64)).Encode(0, 1)) {}

Decimal128Impl::Decimal128Impl(uint32 const u32)
    : data_(BcdNum(u32).Encode(0, 0)) {}

Decimal128Impl::Decimal128Impl(uint64 const u64)
    : data_(BcdNum(u64).Encode(0, 0)) {}

Decimal128Impl::Decimal128Impl(const EncodedData& data)
    : data_(data) {}

Decimal128Impl::Decimal128Impl(const Decimal128Impl& rhs)
    : data_(rhs.data_) {}

// operators
Decimal128Impl::operator float64() const {
  switch (ieee_class()) {
    case Class_Infinity:
      return Float64Impl(sign()
          ? Float64Impl::kNegativeInfinityPattern
          : Float64Impl::kPositiveInfinityPattern);

    case Class_QuietNaN:
      return Float64Impl(sign()
          ? Float64Impl::kNegativeQuietNaNPattern
          : Float64Impl::kPositiveQuietNaNPattern);

    case Class_SignalingNaN:
      return Float64Impl(sign()
          ? Float64Impl::kNegativeSignalingNaNPattern
          : Float64Impl::kPositiveSignalingNaNPattern);

    case Class_Normal: {
      auto const exp = exponent();
      if (exp < DBL_MIN_10_EXP) {
        return Float64Impl(sign()
            ? Float64Impl::kNegativeZeroPattern
            : Float64Impl::kPositiveZeroPattern);
      }

      if (exp > DBL_MAX_10_EXP) {
        return Float64Impl(sign()
            ? Float64Impl::kNegativeInfinityPattern
            : Float64Impl::kPositiveInfinityPattern);
      }

      BcdNum bcd_num(data_);
      auto const num_digits = bcd_num.CountDigits();
      float64 x = 0;
      for (auto i = 1; i <= num_digits; i++) {
        x *= 10;
        x += bcd_num.digit_at(num_digits - i);
      }

      auto const y = exp >= 0 ? x * Power(10.0, exp) : x / Power(10.0, -exp);
      return sign() ? -y : y;
    }

    case Class_Subnormal:
      // Subnormal value of Decimal128 is too small for float64. We return
      // +0 or -0.
      return Float64Impl(sign()
          ? Float64Impl::kNegativeZeroPattern
          : Float64Impl::kPositiveZeroPattern);

    case Class_Zero:
      return Float64Impl(sign()
          ? Float64Impl::kNegativeZeroPattern
          : Float64Impl::kPositiveZeroPattern);

    default:
      CAN_NOT_HAPPEN();
  }
}

Decimal128Impl& Decimal128Impl::operator =(const Decimal128Impl& rhs) {
  data_ = rhs.data_;
  return *this;
}

Decimal128Impl Decimal128Impl::operator +(const Decimal128Impl& rhs) const {
  auto& lhs = *this;
  auto const lhs_sign = lhs.sign();
  auto const rhs_sign = rhs.sign();

  SpecialValueHandler handler(lhs, rhs);
  switch (handler.Handle()) {
    case SpecialValueHandler::BothFinite:
      break;

    case SpecialValueHandler::BothInfinity:
      return lhs_sign == rhs_sign ? lhs : QuietNaN(0);

    case SpecialValueHandler::EitherNaN:
      return handler.value();

    case SpecialValueHandler::LhsIsInfinity:
      return lhs;

    case SpecialValueHandler::RhsIsInfinity:
      return rhs;
  }

  DCHECK(lhs.is_finite() && rhs.is_finite());

  const AlignedOperands aligned(lhs, rhs);

  auto result = lhs_sign == rhs_sign
      ? aligned.lhs_bcd.Add(aligned.rhs_bcd, aligned.exponent, lhs_sign)
      : aligned.lhs_bcd.Subtract(aligned.rhs_bcd, aligned.exponent, lhs_sign);

  if (lhs_sign && !rhs_sign && result.is_zero()) {
    result.set_sign(0);
  }
  return Decimal128Impl(result);
}

Decimal128Impl Decimal128Impl::operator -(const Decimal128Impl& rhs) const {
  auto& lhs = *this;
  auto const lhs_sign = lhs.sign();
  auto const rhs_sign = rhs.sign();

  SpecialValueHandler handler(lhs, rhs);
  switch (handler.Handle()) {
    case SpecialValueHandler::BothFinite:
      break;

    case SpecialValueHandler::BothInfinity:
      return lhs_sign == rhs_sign ? QuietNaN(0) : lhs;

    case SpecialValueHandler::EitherNaN:
      return handler.value();

    case SpecialValueHandler::LhsIsInfinity:
      return lhs;

    case SpecialValueHandler::RhsIsInfinity:
      return Infinity(rhs_sign ^ 1);
  }

  DCHECK(lhs.is_finite() && rhs.is_finite());

  const AlignedOperands aligned(lhs, rhs);

  auto const result = lhs_sign == rhs_sign
      ? aligned.lhs_bcd.Subtract(aligned.rhs_bcd, aligned.exponent, lhs_sign)
      : aligned.lhs_bcd.Add(aligned.rhs_bcd, aligned.exponent, lhs_sign);
  return Decimal128Impl(result);
}

Decimal128Impl Decimal128Impl::operator *(const Decimal128Impl& rhs) const {
  auto& lhs = *this;
  auto const lhs_sign = lhs.sign();
  auto const rhs_sign = rhs.sign();
  auto const result_sign = lhs_sign ^ rhs_sign;

  SpecialValueHandler handler(lhs, rhs);
  switch (handler.Handle()) {
    case SpecialValueHandler::BothFinite:
      return BinNum(lhs.data_).Multiply(
        BinNum(rhs.data_),
        lhs.exponent() + rhs.exponent(),
        result_sign);

    case SpecialValueHandler::BothInfinity:
      return Infinity(result_sign);

    case SpecialValueHandler::EitherNaN:
      return handler.value();

    case SpecialValueHandler::LhsIsInfinity:
      return rhs.is_zero() ? QuietNaN(0) : Infinity(result_sign);

    case SpecialValueHandler::RhsIsInfinity:
      return lhs.is_zero() ? QuietNaN(0) : Infinity(result_sign);
  }

  CAN_NOT_HAPPEN();
}

Decimal128Impl Decimal128Impl::operator /(const Decimal128Impl& rhs) const {
  return Divide(rhs, FloatResult);
}

bool Decimal128Impl::operator ==(const Decimal128Impl& rhs) const {
  return FastEquals(rhs) || CompareTo(rhs).is_zero();
}

bool Decimal128Impl::operator !=(const Decimal128Impl& rhs) const {
  if (FastEquals(rhs)) return false;
  auto const diff = CompareTo(rhs);
  return diff.is_finite() && !diff.is_zero();
}

bool Decimal128Impl::operator <(const Decimal128Impl& rhs) const {
  auto const diff = CompareTo(rhs);
  return diff.is_finite() && diff.is_negative();
}

bool Decimal128Impl::operator <=(const Decimal128Impl& rhs) const {
  if (FastEquals(rhs)) return true;
  auto const diff = CompareTo(rhs);
  return diff.is_finite() && (diff.is_zero() || diff.is_negative());
}

bool Decimal128Impl::operator >(const Decimal128Impl& rhs) const {
  auto const diff = CompareTo(rhs);
  return diff.is_finite() && diff.is_positive();
}

bool Decimal128Impl::operator >=(const Decimal128Impl& rhs) const {
  if (FastEquals(rhs)) return true;
  auto const diff = CompareTo(rhs);
  return diff.is_finite() && (diff.is_zero() || diff.is_positive());
}

// properties
int Decimal128Impl::exponent() const {
  return is_finite() ? data_.exponent_biased() - kEbias : 0;
}

IeeeClass Decimal128Impl::ieee_class() const {
  switch (data_.words_[3] & kClassMask) {
    case kPositiveInfinityPattern:
    case kNegativeInfinityPattern:
      return Class_Infinity;

    case kPositiveQuietNaNPattern:
    case kNegativeQuietNaNPattern:
      return Class_QuietNaN;

    case kPositiveSignalingNaNPattern:
    case kNegativeSignalingNaNPattern:
      return Class_SignalingNaN;
  }

  BcdNum bcd_num(data_);
  auto const coefficient_length = bcd_num.CountDigits();
  if (!coefficient_length) {
    return Class_Zero;
  }

  auto const adjusted_exponent = exponent() + coefficient_length - 1;
  if (adjusted_exponent < kEmin) {
    return Class_Subnormal;
  }

  return Class_Normal;
}

int Decimal128Impl::sign() const {
  return data_.sign();
}

void Decimal128Impl::set_sign(int value) {
  data_.set_sign(value);
}

// [C]
Decimal128Impl Decimal128Impl::CompareTo(const Decimal128Impl& rhs) const {
  auto const result = *this - rhs;
  switch (result.ieee_class()) {
    case Class_Infinity:
      return result.sign() ? Decimal128Impl(-1) : Decimal128Impl(1);

    case Class_Normal:
    case Class_QuietNaN:
    case Class_SignalingNaN:
    case Class_Subnormal:
      return result;

    case Class_Zero:
      return Zero(0);

    default:
      CAN_NOT_HAPPEN();
  }
}

// [D]
Decimal128Impl Decimal128Impl::Divide(
    const Decimal128Impl& rhs,
    DivideResult const result_type) const {
  auto& lhs = *this;
  auto const lhs_sign = lhs.sign();
  auto const rhs_sign = rhs.sign();
  auto const result_sign = lhs_sign ^ rhs_sign;

  SpecialValueHandler handler(lhs, rhs);
  switch (handler.Handle()) {
    case SpecialValueHandler::BothFinite:
      break;

    case SpecialValueHandler::BothInfinity:
      return QuietNaN(0);

    case SpecialValueHandler::EitherNaN:
      return handler.value();

    case SpecialValueHandler::LhsIsInfinity:
      return Infinity(result_sign);

    case SpecialValueHandler::RhsIsInfinity:
      return Zero(result_sign);
  }

  DCHECK(lhs.is_finite() && rhs.is_finite());

  if (rhs.is_zero()) {
    return lhs.is_zero() ? QuietNaN(0) : Infinity(result_sign);
  }

  auto const result_exponent = lhs.exponent() - rhs.exponent();

  if (lhs.is_zero()) {
    // 0/non-zero => 0
    return result_type == FloatResult
      ? Decimal128Impl(BcdNum().Encode(result_exponent, result_sign))
      : Zero(result_sign);
  }

  return BinNum(lhs.data_).Divide(
    BinNum(rhs.data_),
    result_exponent,
    result_sign,
    result_type);
}

// [F]
// Compares two Decimal128Impl number as binary string. This method is
// used for equality of infinity operands which substract operation
// returns non-zero.
bool Decimal128Impl::FastEquals(const Decimal128Impl& rhs) const {
  auto const lhs_word = data_.words_[3];
  auto const rhs_word = rhs.data_.words_[3];

  if (lhs_word != rhs_word) {
    return false;
  }

  if ((lhs_word & kSpecialValueMask) != kSpecialValueMask) {
    return this == &rhs
      || !::memcmp(data_.words_,
                   rhs.data_.words_,
                   sizeof(data_.words_));
  }

  return lhs_word == kNegativeInfinityPattern
      || lhs_word == kPositiveInfinityPattern;
}

Decimal128Impl Decimal128Impl::FromString(const String& str) {
  auto exponent = 0;
  auto exponent_sign = 0;
  auto minus = false;
  auto num_digits = 0;
  auto num_digits_after_dot = 0;
  auto num_extra_digits = 0;
  auto signaling = false;

  enum {
    State_Digit,
    State_Dot,
    State_Dot_Digit,
    State_E,
    State_E_Digit,
    State_E_Sign,
    State_I,
    State_IN,
    State_INF,
    State_INFI,
    State_INFIN,
    State_INFINI,
    State_INFINIT,
    State_INFINITY,
    State_N,
    State_NA,
    State_NAN,
    State_S,
    State_Sign,
    State_Start,
    State_Zero,
  } state = State_Start;

  #define HandleChar(pattern, nextState, ...) \
    if (ch == pattern) { \
      __VA_ARGS__; \
      state = nextState; \
      break; \
    }

  #define HandleBothCaseChar(upperCase, nextState, ...) \
    if (ch == upperCase || ch == upperCase - 'A' + 'a') { \
      __VA_ARGS__; \
      state = nextState; \
      break; \
    }

  #define ExpectBothCaseChar(expected, nextState) \
    HandleBothCaseChar(expected, nextState); \
    return QuietNaN(0);

  BcdNum accumulator;
  for (auto it = str.begin(); it != str.end(); ++it) {
    auto const ch = *it;
    switch (state) {
      case State_Digit:
        if (ch >= '0' && ch <= '9') {
          if (num_digits < Decimal128Impl::kDigits) {
            ++num_digits;
            accumulator.set_digit_at(kDigits - num_digits, uint8(ch - '0'));
          } else {
            if (!num_extra_digits && ch >= '5') {
              accumulator.AddOne();
            }
            ++num_extra_digits;
          }
          break;
        }

        HandleChar('.', State_Dot);
        ExpectBothCaseChar('E', State_E);

      case State_Dot:
        if (ch >= '0' && ch <= '9') {
          if (num_digits < Decimal128Impl::kDigits) {
            ++num_digits;
            ++num_digits_after_dot;
            accumulator.set_digit_at(kDigits - num_digits, uint8(ch - '0'));
          }
          state = State_Dot_Digit;
          break;
        }

      case State_Dot_Digit:
        if (ch >= '0' && ch <= '9') {
          if (num_digits < Decimal128Impl::kDigits) {
            ++num_digits;
            ++num_digits_after_dot;
            accumulator.set_digit_at(kDigits - num_digits, uint8(ch - '0'));
          }
          break;
        }

        ExpectBothCaseChar('E', State_E);

      case State_E:
        if (ch >= '0' && ch <= '9') {
          exponent = ch - '0';
          state = State_E_Digit;
          break;
        }

        HandleChar('+', State_E_Sign, exponent_sign = 0);
        HandleChar('-', State_E_Sign, exponent_sign = 1);
        return QuietNaN(0);

       case State_E_Digit:
        if (ch >= '0' && ch <= '9') {
          exponent *= 10;
          exponent += ch - '0';
          if (exponent > Decimal128Impl::kEmax + kDigits) {
            return exponent_sign ? Zero(0) : Infinity(minus);
          }
          state = State_E_Digit;
          break;
        }

        return QuietNaN(0);

      case State_E_Sign:
        if (ch >= '0' && ch <= '9') {
          exponent = ch - '0';
          state = State_E_Digit;
          break;
        }

        return QuietNaN(0);

      case State_I: ExpectBothCaseChar('N', State_IN);
      case State_IN: ExpectBothCaseChar('F', State_INF);
      case State_INF: ExpectBothCaseChar('I', State_INFI);
      case State_INFI: ExpectBothCaseChar('N', State_INFIN);
      case State_INFIN: ExpectBothCaseChar('I', State_INFINI);
      case State_INFINI: ExpectBothCaseChar('T', State_INFINIT);
      case State_INFINIT: ExpectBothCaseChar('Y', State_INFINITY);
      case State_INFINITY: return QuietNaN(0);

      case State_N: ExpectBothCaseChar('A', State_NA);
      case State_NA: ExpectBothCaseChar('N', State_NAN);
      case State_NAN: return QuietNaN(0);

      case State_S:
        signaling = true;
        ExpectBothCaseChar('N', State_N);

      case State_Sign:
        if (ch >= '1' && ch <= '9') {
          accumulator.set_digit_at(kDigits -1, uint8(ch - '0'));
          num_digits = 1;
          state = State_Digit;
          break;
        }

        HandleChar('0', State_Zero);
        HandleBothCaseChar('I', State_I);
        HandleBothCaseChar('N', State_N);
        HandleBothCaseChar('S', State_S);
        return QuietNaN(0);

      case State_Start:
        if (ch >= '1' && ch <= '9') {
          accumulator.set_digit_at(kDigits -1, uint8(ch - '0'));
          num_digits = 1;
          state = State_Digit;
          break;
        }

        HandleChar('+', State_Sign, minus = false);
        HandleChar('-', State_Sign, minus = true);
        HandleChar('0', State_Zero);
        HandleBothCaseChar('I', State_I);
        HandleBothCaseChar('N', State_N);
        HandleBothCaseChar('S', State_S);
        return QuietNaN(0);

      case State_Zero:
        if (ch >= '1' && ch <= '9') {
          accumulator.set_digit_at(kDigits - 1, uint8(ch - '0'));
          num_digits = 1;
          state = State_Digit;
          break;
        }

        HandleChar('.', State_Dot);
        HandleChar('0', State_Zero);
        ExpectBothCaseChar('E', State_E);

      default:
        CAN_NOT_HAPPEN();
    }
  }

  if (state == State_INF || state == State_INFINITY) {
    return Infinity(minus);
  }

  if (state == State_NAN) {
    return signaling ? SignalingNaN(minus) : QuietNaN(minus);
  }

  if (state == State_Zero) {
    return Zero(minus);
  }

  if (state == State_Digit
        || state == State_E_Digit
        || state == State_Dot_Digit) {
    accumulator.ShiftRight(kDigits - num_digits);
    auto result_exponent = exponent * (1 - exponent_sign * 2)
        - num_digits_after_dot + num_extra_digits;

    auto const biased_exponent = result_exponent + kEbias;
    if (biased_exponent < 0) {
      return Zero(0);
    }

    auto const overflow = biased_exponent - kElimit;
    if (overflow > 0) {
      if (overflow + num_digits - num_digits_after_dot > kDigits) {
        return Infinity(minus);
      }
      accumulator.ShiftLeft(overflow);
      result_exponent -= overflow;
    }

    return Decimal128Impl(accumulator.Encode(result_exponent, minus));
  }

  return QuietNaN(0);
}

// [I]
Decimal128Impl Decimal128Impl::Infinity(int const sign) {
  return Decimal128Impl(
    sign,
    kPositiveInfinityPattern,
    kNegativeInfinityPattern);
}

// [Q]
Decimal128Impl Decimal128Impl::QuietNaN(int const sign) {
  return Decimal128Impl(
      sign,
      kPositiveQuietNaNPattern,
      kNegativeQuietNaNPattern);
}

// [R]
Decimal128Impl Decimal128Impl::Remainder(const Decimal128Impl& rhs) const {
  auto const quotient = Divide(rhs, IntegerResult);
  if (quotient.is_special()) {
    return quotient;
  }

  auto rem = *this - quotient * rhs;
  rem.set_sign(sign());
  return rem;
}

Decimal128Impl Decimal128Impl::RoundAt(int const num_digits) {
  BcdNum bcd_num(data_);
  auto const cur_num_digits = bcd_num.CountDigits();
  bcd_num.RoundAt(num_digits);
  if (exponent() < 0) {
    bcd_num.RemoveTrailingZeros();
  }
  auto const new_num_digits = bcd_num.CountDigits();
  return Decimal128Impl(
      bcd_num.Encode(exponent() - new_num_digits + cur_num_digits,
                     sign()));
}

// [S]
Decimal128Impl Decimal128Impl::SignalingNaN(int const sign) {
  return Decimal128Impl(
      sign,
      kPositiveSignalingNaNPattern,
      kNegativeSignalingNaNPattern);
}

// [T]
String Decimal128Impl::ToString() const {
  switch (ieee_class()) {
    case Class_Infinity:
      return sign() ? "-Infinity" : "Infinity";

    case Class_QuietNaN:
      return sign() ? "-NaN" : "NaN";

    case Class_SignalingNaN:
      return sign() ? "-sNaN" : "sNaN";

    case Class_Normal:
    case Class_Subnormal:
    case Class_Zero:
      break;
  }

  StringBuilder builder;
  if (sign()) {
    builder.Append('-');
  }

  // convert to scientific-string
  BcdNum bcd_num(data_);
  auto const coefficient_length = max(bcd_num.CountDigits(), 1);
  auto const original_exponent = exponent();
  auto const adjusted_exponent = original_exponent + coefficient_length - 1;
  if (original_exponent <= 0 && adjusted_exponent >= -6) {
    if (!original_exponent) {
      for (auto i = 1; i <= coefficient_length; ++i) {
        builder.Append(bcd_num.digit_char_at(coefficient_length - i));
      }
      return builder.ToString();
    }

    if (adjusted_exponent >= 0) {
      for (auto i = 1; i <= coefficient_length; ++i) {
        builder.Append(bcd_num.digit_char_at(coefficient_length - i));
        if (i == adjusted_exponent + 1) {
          builder.Append('.');
        }
      }
      return builder.ToString();
    }

    builder.Append("0.");
    for (auto i = adjusted_exponent + 1; i < 0; ++i) {
      builder.Append('0');
    }

    for (auto i = 1; i <= coefficient_length; ++i) {
      builder.Append(bcd_num.digit_char_at(coefficient_length - i));
    }

  } else {
    auto k = coefficient_length - 1;
    builder.Append(bcd_num.digit_char_at(k));
    --k;
    if (k >= 0) {
      builder.Append('.');
      while (k >= 0) {
        builder.Append(bcd_num.digit_char_at(k));
        --k;
      }
    }

    if (adjusted_exponent) {
      builder.AppendFormat("E%+d", adjusted_exponent);
    }
  }
  return builder.ToString();
}

// [Z]
Decimal128Impl Decimal128Impl::Zero(int sign) {
  return Decimal128Impl(sign, kPositiveZeroPattern, kNegativeZeroPattern);
}

} // Numerics
} // Common
