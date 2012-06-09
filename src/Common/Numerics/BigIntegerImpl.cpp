#include "precomp.h"
// @(#)$Id$
//
// Evita Common - CommonInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// Reference:
//  [Warrn03] Henry S. Warren, Jr. "Hacker's Delight". Addison Wesley. 2003
//  [White86] Jon L White, Reconfigurable, Retargetable Bignums: A Case Study
//  in Efficient, Portable Lisp System Building, Proceedings of the ACM
//  conference on Lisp & FP, 1986.

#include "./BigIntegerImpl.h"

#include "./Float64Impl.h"
#include "../StringBuilder.h"
#include "../Collections/Stack_.h"

namespace Common {
namespace Numerics {

using Common::Collections::Stack_;

namespace {

template<typename T> T Ceiling(T a, T b) { return (a + b - 1) / b; }

typedef BigIntegerImpl::DoubleUnit DoubleUnit;
typedef BigIntegerImpl::DoubleSignedUnit DoubleSignedUnit;
typedef BigIntegerImpl::SignedUnit SignedUnit;
typedef BigIntegerImpl::Unit Unit;
static int const kUnitSize = BigIntegerImpl::kUnitSize;

// This class encapsulates signed double unit arithmetic, e.g. 64 bit
// arithmetic if Unit is uint32.
class SignedAccumulator {
  private: DoubleSignedUnit value_;

  public: explicit SignedAccumulator(Unit high, Unit low)
    : value_((DoubleUnit(high) << kUnitSize) | low) {}

  public: explicit SignedAccumulator(SignedUnit unit) : value_(unit) {}

  public: operator BigIntegerImpl() const {
    return BigIntegerImpl(value_);
  }

  public: operator int64() const { return value_; }

  public: SignedAccumulator& operator +=(SignedUnit unit) {
    value_ += unit;
    return *this;
  }

  public: SignedAccumulator& operator -=(SignedUnit unit) {
    value_ -= unit;
    return *this;
  }

  public: SignedAccumulator& operator *=(SignedUnit unit) {
    value_ *= unit;
    return *this;
  }
};

// This class encapsulates unsigned double unit arithmetic, e.g. 64 bit
// arithmetic if Unit is uint32.
class UnsignedAccumulator {
  private: DoubleUnit value_;
  public: explicit UnsignedAccumulator(Unit low) : value_(low) {}

  public: explicit UnsignedAccumulator(Unit high, Unit low)
    : value_((DoubleUnit(high) << kUnitSize) | low) {}

  // operators
  public: operator BigIntegerImpl() const {
    return BigIntegerImpl(value_);
  }

  public: operator uint64() const {
    return value_;
  }

  public: Unit operator /(Unit low) const { return Unit(value_ / low); }
  public: Unit operator %(Unit low) const { return Unit(value_ % low); }

  public: bool operator <=(const UnsignedAccumulator r) const {
    return value_ <= r.value_;
  }

  public: UnsignedAccumulator& operator +=(Unit low) {
    value_ += low;
    return *this;
  }

  private: UnsignedAccumulator& operator -=(const UnsignedAccumulator& rhs) {
    value_ -= rhs.value_;
    return *this;
  }

  public: UnsignedAccumulator& operator -=(Unit low) {
    value_ -= low;
    return *this;
  }

  public: UnsignedAccumulator& operator *=(Unit low) {
    value_ *= low;
    return *this;
  }

  public: UnsignedAccumulator& operator /=(Unit low) {
    value_ /= low;
    return *this;
  }

  public: UnsignedAccumulator& operator |=(Unit low) {
    value_ |= low;
    return *this;
  }

  public: UnsignedAccumulator& operator <<=(int shift_amount) {
    value_ <<= shift_amount;
    return *this;
  }

  public: UnsignedAccumulator& operator >>=(int shift_amount) {
    value_ >>= shift_amount;
    return *this;
  }

  // properties
  public: Unit high() const { return Unit((value_ >> kUnitSize) & Unit(-1)); }
  public: Unit low() const { return Unit(value_ & Unit(-1)); }
};

// This class encapsulates multiple-precision arithmetic on Vector_<Unit>.
class Accumulator {
  private: Vector_<Unit> units_;

  // ctor
  public: explicit Accumulator(int size) : units_(size) {}

  // operators
  public: Unit& operator[](int const index) {
    return units_[index];
  }

  public: Unit operator[](int const index) const {
    return units_[index];
  }

  public: operator Vector_<Unit>() const {
    return units_;
  }

  // Bitwise shift left
  public: Accumulator& operator <<=(int shift_amount) {
    DCHECK_GE(shift_amount, 1);
    DCHECK_LT(shift_amount, kUnitSize);
    auto carry = Unit(0);
    for (auto it = begin(); it != end(); ++it) {
      auto const high = *it >> (kUnitSize  - shift_amount);
      *it = (*it << shift_amount) | carry;
      carry = high;
    }
    return *this;
  }

  // Bitwise shift right
  public: Accumulator& operator >>=(int shift_amount) {
    DCHECK_GE(shift_amount, 1);
    DCHECK_LT(shift_amount, kUnitSize);
    auto const mask = (1 << shift_amount) - 1;
    auto carry = Unit(0);
    for (auto it = rbegin(); it != rend(); ++it) {
      auto const low = *it & mask;
      *it = (*it >> shift_amount) | (carry << (kUnitSize - shift_amount));
      carry = low;
    }
    return *this;
  }

  // properties
  public: Vector_<Unit>::Iterator begin() { return units_.begin(); }
  public: Vector_<Unit>::ConstIterator begin() const { return units_.begin(); }
  public: Vector_<Unit>::Iterator end() { return units_.end(); }
  public: Vector_<Unit>::ConstIterator end() const { return units_.end(); }
  public: Vector_<Unit>::ReverseIterator rbegin() { return units_.rbegin(); }
  public: Vector_<Unit>::ReverseIterator rend() { return units_.rend(); }
  public: int size() const { return units_.size(); }
  public: Vector_<Unit> units() { return units_; }

  // [C]
  // Returns mnimum number of units needed for representing two's complement
  // multiple-precision integer. Implementations of arithmetic operation
  // produces non-canonical form of vector of units which most significand
  // unit (MSU) is an redundnat 0 (if positive number) or -1 (if negative
  // number).
  private: static int CountUnits(const Vector_<Unit> units) {
    auto num_units = units.size();
    auto it = units.rbegin();
    if (SignedUnit(units[num_units - 1]) < 0) {
      while (it != units.rend()) {
        if (*it != Unit(-1)) {
          if (int32(*it) >= 0) {
            ++num_units;
          }
          break;
        }
        ++it;
        --num_units;
      }
    } else {
      while (it != units.rend()) {
        if (*it) {
          if (int32(*it) < 0) {
            ++num_units;
          }
          break;
        }
        ++it;
        --num_units;
      }
    }
    return num_units;
  }

  // [F]
  public: void Fill(int const offset, int const size, Unit value) {
    units_.Fill(offset, size, value);
  }

  // [I]
  private: bool IsMinus() const {
    return SignedUnit(units_[units_.size() - 1]) < 0;
  }

  // [N]
  // Returns two's complement of specified unsigned integer.
  public: static BigIntegerImpl Negate(const Vector_<Unit>& units) {
    Accumulator result(units.size() + 1);
    auto carry = Unit(1);
    auto result_it = result.begin();
    for (auto it = units.begin(); it != units.end(); ++it) {
      UnsignedAccumulator accumulator(~*it);
      accumulator += carry;
      *result_it = accumulator.low();
      carry = accumulator.high();
      ++result_it;
    }
    *result_it = carry + ~0;
    return BigIntegerImpl(result);
  }

  // Returns normalized vector of units.
  public: static Vector_<Unit> Normalize(const Vector_<Unit>& v) {
    if (!v.size()) {
      return Vector_<Unit>(0);
    }
    auto const num_units = Accumulator::CountUnits(v);
    DCHECK_GE(num_units, 0);
    return Vector_<Unit>(v.begin(), v.begin() + max(num_units, 1));
  }

  // [S]
  public: void Set(int const offset, const Vector_<Unit> units) {
    DCHECK_GE(offset, 0);
    DCHECK_LT(offset, units_.size());
    DCHECK_LE(offset + units.size(),  units_.size());
    auto i = offset;
    for (auto it = units.begin(); it != units.end(); ++it) {
      units_[i] = *it;
      ++i;
    }
  }

  public: void Set(
      int const offset,
      const Vector_<Unit>::Iterator& begin,
      const Vector_<Unit>::Iterator& end) {
    DCHECK_GE(offset, 0);
    DCHECK_LE(offset + (end - begin),  units_.size());
    auto i = offset;
    for (auto it = begin; it != end; ++it) {
      units_[i] = *it;
      ++i;
    }
  }

  public: void SetAndExtend(const BigIntegerImpl& rhs) {
    Set(0, rhs.units());
    Fill(rhs.size(), size() - rhs.size(), rhs.IsMinus() ? Unit(-1) : 0);
  }

  // Subtract rhs x (kUnit ** shift_amount).
  // Subtract and multiply uses this method.
  public: void SubtractWithShift(
      const BigIntegerImpl& rhs,
      int const shift_amount) {
    DCHECK_GE(shift_amount, 0);
    DCHECK_LT(shift_amount, size());
    auto result_it = begin() + shift_amount;
    auto borrow = Unit(0);
    for (auto rhs_it = rhs.begin(); rhs_it != rhs.end(); ++rhs_it) {
      UnsignedAccumulator accumulator(*result_it);
      accumulator -= *rhs_it;
      accumulator -= borrow;
      borrow = -static_cast<SignedUnit>(accumulator.high());
      *result_it = accumulator.low();
      ++result_it;
    }

    auto const unit = rhs.IsMinus() ? Unit(-1) : 0;
    if (borrow != unit) {
      while (result_it != end()) {
        UnsignedAccumulator accumulator(*result_it);
        accumulator -= unit;
        accumulator -= borrow;
        borrow = -static_cast<SignedUnit>(accumulator.high());
        *result_it = accumulator.low();
        ++result_it;
      }
    }
  }
};

static Vector_<Unit> MakeUnits(int64 i64) {
  auto const high = uint32(uint64(i64) >> 32);
  auto const low = uint32(i64 & uint32(-1));

  if (!high && int32(low) >= 0) {
    Vector_<Unit> units(1);
    units[0] = low;
    return units;
  }

  if (high == uint32(-1) && int32(low) < 0) {
    Vector_<Unit> units(1);
    units[0] = low;
    return units;
  }

  Vector_<Unit> units(2);
  units[0] = low;
  units[1] = high;
  return units;
}

static Vector_<Unit> MakeUnits(uint32 u32) {
  if (int32(u32) >= 0) {
    Vector_<Unit> units(1);
    units[0] = u32;
    return units;
  }

  Vector_<Unit> units(2);
  units[0] = u32;
  units[1] = 0;
  return units;
}

static Vector_<Unit> MakeUnits(uint64 u64) {
  auto const high = uint32(u64 >> 32);
  auto const low = uint32(u64 & uint32(-1));
  if (!high) {
    if (int32(low) >= 0) {
      return MakeUnits(low);
    }
    Vector_<Unit> units(2);
    units[0] = low;
    units[1] = 0;
    return units;
  }

  if (int32(high) > 0) {
    Vector_<Unit> units(2);
    units[0] = low;
    units[1] = high;
    return units;
  }

  Vector_<Unit> units(3);
  units[0] = low;
  units[1] = high;
  units[2] = 0;
  return units;
}

static Vector_<Unit> MakeVector(SignedUnit unit) {
  Vector_<Unit> vector(1);
  vector[0] = unit;
  return vector;
}

static Vector_<Unit> MakeUnits(float64 f64) {
  Float64Impl value(f64);
  switch (value.ieee_class()) {
    case Class_Infinity:
    case Class_QuietNaN:
    case Class_SignalingNaN:
    case Class_Subnormal:
    case Class_Zero:
      return MakeVector(0);

    case Class_Normal: {
      auto const exponent = value.exponent() - Float64Impl::kSignificandBits;
      if (exponent < -Float64Impl::kSignificandBits) {
        return MakeVector(0);
      }

      auto const significand = BigIntegerImpl(
          int64(value.significand()) * (1 - 2 * value.sign()));

      return exponent > 0
          ? (significand * (BigIntegerImpl(1) << exponent)).units()
          : (significand / (BigIntegerImpl(1) << -exponent)).units();
    }

    default:
      CAN_NOT_HAPPEN();
  }
}

static Vector_<Unit> MakeUnits(float32 f32) {
  // TODO(yosi) We should implement float32 specific version of MakeUnits.
  return MakeUnits(float64(f32));
}

} // namespace

// ctor
BigIntegerImpl::BigIntegerImpl(int32 i32) : units_(1) {
  units_[0] = Unit(i32);
}

BigIntegerImpl::BigIntegerImpl(int64 i64) : units_(MakeUnits(i64)) {}
BigIntegerImpl::BigIntegerImpl(float32 f32) : units_(MakeUnits(f32)) {}
BigIntegerImpl::BigIntegerImpl(float64 f64) : units_(MakeUnits(f64)) {}
BigIntegerImpl::BigIntegerImpl(uint32 u32) : units_(MakeUnits(u32)) {}
BigIntegerImpl::BigIntegerImpl(uint64 u64) : units_(MakeUnits(u64)) {}

BigIntegerImpl::BigIntegerImpl(const BigIntegerImpl& rhs)
    : units_(rhs.units_) {}

BigIntegerImpl::BigIntegerImpl(BigIntegerImpl&& rhs)
    : units_(rhs.units_) {}

BigIntegerImpl::BigIntegerImpl(const Vector_<Unit>& units)
    : units_(Accumulator::Normalize(units)) {}

BigIntegerImpl::BigIntegerImpl(Vector_<Unit>&& units)
    : units_(Accumulator::Normalize(units)) {}

// operators
BigIntegerImpl& BigIntegerImpl::operator =(const BigIntegerImpl& rhs) {
  units_ = rhs.units_;
  return *this;
}

BigIntegerImpl& BigIntegerImpl::operator =(BigIntegerImpl&& rhs) {
  units_ = rhs.units_;
  return *this;
}

BigIntegerImpl::operator bool() const { return !IsZero(); }

BigIntegerImpl::operator float32() const {
  // TODO(yosi) 2012-04-07 We should have float32 specific version of
  // BigInteger::operator float32
  return float32(float64(*this));
}

// Returns float64 equivalent value or positiv/negative float64 infinity if
// this BigInteger is a too large/small for float64. If bit length of
// this BigInteger is greater than 53 (float64 precision), resulted float64
// is an inexact.
BigIntegerImpl::operator float64() const {
  if (IsMinus()) {
    return -float64(-*this);
  }

  if (IsZero()) {
    return 0;
  }

  // TODO(yosi) 2012-04-07 Make sMostPositiveFloat64 thread safe.
  static BigIntegerImpl* sMostPositiveFloat64;
  if (!sMostPositiveFloat64) {
    // ((1<<53)-1) << 971 == 0x1FFFFFFFFFFFFF << 971
    Float64Impl most_positive_float64(Float64Impl::kMostPositivePattern);
    sMostPositiveFloat64 = new BigIntegerImpl(float64(most_positive_float64));
  }

  if (*this > *sMostPositiveFloat64) {
    return Float64Impl(Float64Impl::kPositiveInfinityPattern);
  }

  auto const num_bits = bit_length();
  auto const shift_amount = num_bits - Float64Impl::kPrecision;
  auto const biased_exponent = num_bits - 1 + Float64Impl::kEbias;
  auto const exponent_pattern =
      uint64(biased_exponent) << Float64Impl::kSignificandBits;
  auto const significand = shift_amount >= 0
      ? uint64(*this >> shift_amount)
      : uint64(*this << -shift_amount);
  return Float64Impl(
      exponent_pattern | (significand & Float64Impl::kSignificandMask));
}

// Returns this BigInteger as int32 if possible.
BigIntegerImpl::operator int32() const {
  DCHECK_EQ(size(), 1);
  return units_[0];
}

// Returns this BigInteger as int64 if possible.
BigIntegerImpl::operator int64() const {
  DCHECK_LE(size(), 2);
  return size() == 1 ? units_[0] : SignedAccumulator(units_[1], units_[0]);
}

// Returns this BigInteger as uint32 if possible.
BigIntegerImpl::operator uint32() const {
  DCHECK(!IsMinus());
  DCHECK_LE(size(), 2);
  if (size() == 1) {
    return units_[0];
  }
  DCHECK_EQ(units_[1], Unit(0));
  return units_[0];
}

// Returns this BigInteger as uint64 if possible.
BigIntegerImpl::operator uint64() const {
  DCHECK(!IsMinus());
  DCHECK_LE(size(), 3);

  if (size() == 1) {
    return units_[0];
  }

  if (size() == 2) {
    return UnsignedAccumulator(units_[1], units_[0]);
  }

  DCHECK_EQ(units_[2], Unit(0));
  return int64(UnsignedAccumulator(units_[1], units_[0]));
}

bool BigIntegerImpl::operator !() const { return IsZero(); }

// Returns one's complement or bitwise not.
BigIntegerImpl BigIntegerImpl::operator ~() const {
  Vector_<Unit> result(units_);
  auto dst = result.begin();
  for (auto src = begin(); src != end(); ++src) {
    *dst = ~*src;
    ++dst;
  }
  return BigIntegerImpl(result);
}

// Returns an negate integer or two's complement value.
BigIntegerImpl BigIntegerImpl::operator -() const {
  return BigIntegerImpl(0) - *this;
}

// Returns addition of two BigInteger's.
// Special cases:
//  lhs == 0  -> rhs
//  rhs == 0  -> lhs
//  both lhs and rhs are single unit -> use signed double units addition.
// Normal case:
//  This method uses m+n+1 units for storing result of addition. One extra
//  unit represents carry and will be dropped by normalization.
BigIntegerImpl BigIntegerImpl::operator +(const BigIntegerImpl& rhs) const {
  if (IsZero()) {
    return rhs;
  }

  if (rhs.IsZero()) {
    return *this;
  }

  if (size() + rhs.size() == 2) {
    SignedAccumulator accumulator(units_[0]);
    accumulator += rhs.units_[0];
    return accumulator;
  }

  Accumulator result(max(size(), rhs.size()) + 1);
  result.SetAndExtend(*this);

  auto result_it = result.begin();

  auto carry = Unit(0);
  for (auto rhs_it = rhs.begin(); rhs_it != rhs.end(); ++rhs_it) {
    UnsignedAccumulator accumulator(*result_it);
    accumulator += *rhs_it;
    accumulator += carry;
    *result_it = accumulator.low();
    carry = accumulator.high();
    ++result_it;
  }

  if (rhs.IsMinus() || carry) {
    auto const unit = rhs.IsMinus() ? Unit(-1) : 0;
    while (result_it != result.end()) {
      UnsignedAccumulator accumulator(*result_it);
      accumulator += unit;
      accumulator += carry;
      *result_it = accumulator.low();
      carry = accumulator.high();
      ++result_it;
    }
  }

  return BigIntegerImpl(result);
}

// Returns subtration.
// Special cases:
//  rhs == 0 -> lhs
//  both lhs and rhs are signle unit => signed double units subtraction.
// Normal case:
//  This methods uses m+n+1 units as same as addition.
BigIntegerImpl BigIntegerImpl::operator -(const BigIntegerImpl& rhs) const {
  if (rhs.IsZero()) {
    return *this;
  }

  if (size() + rhs.size() == 2) {
    SignedAccumulator accumulator(units_[0]);
    accumulator -= rhs.units_[0];
    return accumulator;
  }

  Accumulator result(max(size(), rhs.size()) + 1);
  result.SetAndExtend(*this);
  result.SubtractWithShift(rhs, 0);
  return BigIntegerImpl(result);
}

// Returns product.
// Special case:
//  lhs == 0 -> 0
//  rhs == 0 -> 0
//  both lhs and rhs are single unit -> double units multiplication
//  rhs is a single unit -> one unit multiplication
//  m < n -> swap lhs and rhs
// Normale case:
//  The classic algorithm uses m+n units for working and storing product for
//  calculating unsigned product. Once we get unsigned product, we correct
//  it for subtracting lhs or rhs if they are negative numbers.
BigIntegerImpl BigIntegerImpl::operator *(const BigIntegerImpl& rhs) const {
  if (IsZero() || rhs.IsZero()) {
    return BigIntegerImpl(0);
  }

  if (size() + rhs.size() == 2) {
    SignedAccumulator accumulator(units_[0]);
    accumulator *= rhs.units_[0];
    return accumulator;
  }

  if (size() < rhs.size()) {
    return rhs * *this;
  }

  // [White86] m x n requires m+n-1 or m+n bigits.
  // We need m+n bigits only if a=2^n and b=-2^m.
  Accumulator result(size() + rhs.size());

  if (rhs.size() == 1) {
    // m unit x 1 unit
    result[size()] = 0;
    auto result_it = result.begin();
    auto carry = Unit(0);
    auto rhs_unit = rhs.units_[0];
    for (auto lhs_it = begin(); lhs_it != end(); ++lhs_it) {
      UnsignedAccumulator accumulator(*lhs_it);
      accumulator *= rhs_unit;
      accumulator += carry;
      carry = accumulator.high();
      *result_it= accumulator.low();
      ++result_it;
    }
    *result_it = carry;

  } else {
    // m unit x n unit
    result.Fill(0, size() + rhs.size(), 0);
    auto result_it = result.begin();
    for (auto lhs_it = begin(); lhs_it != end(); ++lhs_it) {
      auto part_it = result_it;
      auto carry = Unit(0);
      for (auto rhs_it = rhs.begin(); rhs_it != rhs.end(); ++rhs_it) {
        UnsignedAccumulator accumulator(*lhs_it);
        accumulator *= *rhs_it;
        accumulator += *part_it;
        accumulator += carry;
        carry = accumulator.high();
        *part_it = accumulator.low();
        ++part_it;
      }
      *part_it = carry;
      ++result_it;
    }
  }

  // Now we have unsigned product. Correct by
  //  subtracting v x 2^(32+m) if u < 0 and
  //  subtracting u x 2^(32+n) if v < 0
  if (IsMinus()) {
    result.SubtractWithShift(rhs, size());
  }

  if (rhs.IsMinus()) {
    result.SubtractWithShift(*this, rhs.size());
  }

  return BigIntegerImpl(result);
}

BigIntegerImpl BigIntegerImpl::operator /(const BigIntegerImpl& rhs) const {
  return Truncate(rhs).first();
}

BigIntegerImpl BigIntegerImpl::operator %(const BigIntegerImpl& rhs) const {
  return Truncate(rhs).second();
}

template<class Operation>
struct LogOperation_ {
  static BigIntegerImpl Operate(
      const BigIntegerImpl& lhs,
      const BigIntegerImpl& rhs) {
    Accumulator result(max(lhs.size(), rhs.size()));
    auto it = result.begin();

    if (lhs.size() >= rhs.size()) {
      auto lhs_it = lhs.begin();
      for (auto rhs_it = rhs.begin(); rhs_it != rhs.end(); ++rhs_it) {
        *it = Operation::Operate(*lhs_it, *rhs_it);
        ++it;
        ++lhs_it;
      }
      auto rhs_unit = rhs.IsMinus() ? Unit(-1) : 0;
      while (lhs_it != lhs.end()) {
        *it = Operation::Operate(*lhs_it, rhs_unit);
        ++it;
        ++lhs_it;
      }
      return BigIntegerImpl(result);
    }

    auto rhs_it = rhs.begin();
    for (auto lhs_it = lhs.begin(); lhs_it != lhs.end(); ++lhs_it) {
      *it = Operation::Operate(*lhs_it, *rhs_it);
      ++it;
      ++rhs_it;
    }
    auto lhs_unit = lhs.IsMinus() ? Unit(-1) : 0;
    while (rhs_it != rhs.end()) {
      *it = Operation::Operate(lhs_unit, *rhs_it);
      ++it;
      ++rhs_it;
    }
    return BigIntegerImpl(result);
  }
};

#define DEFINE_BITWISE_OPERATION(name, op) \
  struct Operation  ## name{ \
    static BigIntegerImpl::Unit Operate( \
        BigIntegerImpl::Unit lhs, \
        BigIntegerImpl::Unit rhs) { \
      return lhs op rhs; \
    } \
  }; \
  BigIntegerImpl BigIntegerImpl::operator op( \
      const BigIntegerImpl& rhs) const { \
    return LogOperation_<Operation ## name>::Operate(*this, rhs); \
  }

DEFINE_BITWISE_OPERATION(And, &)
DEFINE_BITWISE_OPERATION(Ior, |)
DEFINE_BITWISE_OPERATION(Xor, ^)

BigIntegerImpl BigIntegerImpl::operator <<(int const shift_amount) const {
  if (!shift_amount) {
    return *this;
  }

  if (shift_amount < 0) {
    return *this >> -shift_amount;
  }

  auto const num_bits = shift_amount + size() * kUnitSize;
  auto const num_units = Ceiling(num_bits, kUnitSize);
  auto const unit_index = shift_amount / kUnitSize;
  auto const bit_index = shift_amount % kUnitSize;

  Accumulator result(num_units);
  result.Fill(0, unit_index, 0);
  result[num_units - 1] = IsPlus() ? 0 : Unit(-1);
  result.Set(unit_index, units_);

  if (!bit_index) {
    return BigIntegerImpl(result);
  }

  auto carry = Unit(0);
  for (
      auto result_it = result.begin() + unit_index;
      result_it != result.end();
      ++result_it) {
    UnsignedAccumulator accumulator(*result_it);
    accumulator <<= bit_index;
    accumulator |= carry;
    *result_it = accumulator.low();
    carry = accumulator.high();
  }
  return BigIntegerImpl(result);
}

BigIntegerImpl BigIntegerImpl::operator >>(int const shift_amount) const {
  if (!shift_amount) {
    return *this;
  }

  if (shift_amount < 0) {
    return *this << -shift_amount;
  }

  auto const num_bits = size() * kUnitSize - shift_amount;
  if (num_bits <= 0) {
    return BigIntegerImpl(0);
  }

  auto const num_units = Ceiling(num_bits, kUnitSize);
  auto const unit_index = shift_amount / kUnitSize;
  auto const bit_index = shift_amount % kUnitSize;

  Accumulator result(num_units);
  result.Set(0, begin() + unit_index, end());

  if (!bit_index) {
    return BigIntegerImpl(result);
  }

  auto const mask = (Unit(1) << bit_index) - 1;
  auto carry = IsMinus() ? Unit(-1) & mask : 0;
  for (auto it = result.rbegin(); it != result.rend(); ++it) {
    auto const low = *it & mask;
    *it = (*it >> bit_index) | (carry << (kUnitSize - bit_index));
    carry = low;
  }
  return BigIntegerImpl(result);
}

bool BigIntegerImpl::operator ==(const BigIntegerImpl& rhs) const {
  return this == &rhs || !CompareTo(rhs);
}

bool BigIntegerImpl::operator !=(const BigIntegerImpl& rhs) const {
  return !operator ==(rhs);
}

bool BigIntegerImpl::operator <(const BigIntegerImpl& rhs) const {
  return !operator >=(rhs);
}

bool BigIntegerImpl::operator <=(const BigIntegerImpl& rhs) const {
  return this == &rhs || CompareTo(rhs) <= 0;
}

bool BigIntegerImpl::operator >(const BigIntegerImpl& rhs) const {
  return !operator <=(rhs);
}

bool BigIntegerImpl::operator >=(const BigIntegerImpl& rhs) const {
  return this == &rhs || CompareTo(rhs) >= 0;
}

BigIntegerImpl& BigIntegerImpl::operator <<=(int n) {
  return *this = operator <<(n);
}

BigIntegerImpl& BigIntegerImpl::operator >>=(int n) {
  return *this = operator >>(n);
}

// properties
// BigInteger to float64 conversion requires bit_length.
// -1.bit_length() == 0
int BigIntegerImpl::bit_length() const {
  struct Local {
    static int CountBits(SignedUnit const unit) {
      DCHECK_LT(unit, 0);
      auto num_bits = 0;
      auto x = unit;
      while (x != -1) {
        x >>= 1;
        ++num_bits;
      }
      return num_bits;
    }

    static int CountBits(Unit const unit) {
      auto num_bits = 0;
      auto x = unit;
      while (x) {
        x >>= 1;
        ++num_bits;
      }
      return num_bits;
    }
  };

  auto const num_units = size();
  auto const msu = units_[num_units - 1];
  auto const num_lsu_bits = (num_units - 1) * kUnitSize;
  if (IsMinus()) {
    return Local::CountBits(SignedUnit(msu)) + num_lsu_bits;
  }

  return !msu && size() > 1
      ? Local::CountBits(units_[num_units - 2]) +  num_lsu_bits - kUnitSize
      : Local::CountBits(msu) +  num_lsu_bits;
}

// [C]
// Compare two BigInteger's.
int BigIntegerImpl::CompareTo(const BigIntegerImpl& rhs) const {
  if (IsPlus()) {
    if (rhs.IsMinus()) {
      return 1;
    }

    if (auto diff = size() - rhs.size()) {
      return diff;
    }

    auto rhs_it = rhs.rbegin();
    for (auto lhs_it = rbegin(); lhs_it != rend(); ++lhs_it) {
      if (*lhs_it < *rhs_it) {
        return -1;
      }

      if (*lhs_it > *rhs_it) {
        return 1;
      }
      ++rhs_it;
    }
    return 0;
  }

  if (IsMinus()) {
    if (rhs.IsPlus()) {
      return -1;
    }

    if (auto diff = rhs.size() - size()) {
      return diff;
    }

    auto rhs_it = rhs.rbegin();
    for (auto lhs_it = rbegin(); lhs_it != rend(); ++lhs_it) {
      if (*lhs_it < *rhs_it) {
        return -1;
      }

      if (*lhs_it > *rhs_it) {
        return 1;
      }
      ++rhs_it;
    }
    return 0;
  }

  return rhs.IsPlus() ? -1 : rhs.IsMinus() ? 1 : 0;
}

// [I]
bool BigIntegerImpl::IsMinus() const {
  return int32(units_[size() - 1]) < 0;
}

bool BigIntegerImpl::IsMinusOne() const {
  return size() == 1 && units_[0] == Unit(-1);
}

bool BigIntegerImpl::IsOne() const {
  return size() == 1 && units_[0] == 1;
}

bool BigIntegerImpl::IsPlus() const {
  return int32(units_[size() - 1]) >= 0;
}

bool BigIntegerImpl::IsZero() const {
  return size() == 1 && !units_[0];
}

// [P]
BigIntegerImpl BigIntegerImpl::Pow(int n) const {
  DCHECK_GE(n, 0);
  BigIntegerImpl y(1);
  BigIntegerImpl z(*this);
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

// [T]
// Convert binary to decimal string.
String BigIntegerImpl::ToString() const {
  if (IsZero()) {
    return L"0";
  }

  Stack_<char> digits;
  auto work = IsMinus() ? -*this : *this;

  while (work.size() > 1) {
    auto result = work.Truncate(10);
    DCHECK(result.second() <= BigIntegerImpl(9));
    DCHECK(result.first() < work);
    digits.Push(char(result.second().units_[0] + '0'));
    DCHECK_LE(digits.Count(), 20);
    work = result.first();
  }

  auto x = work.units_[0];
  while (x > 0) {
    digits.Push(char(x % 10 + '0'));
    x /= 10;
  }

  StringBuilder builder;
  if (IsMinus()) {
    builder.Append('-');
  }

  while (!digits.IsEmpty()) {
    builder.Append(digits.Pop());
  }
  return builder.ToString();
}

// Truncates and returns quotient and remainder in Pair using the classic
// algorithm.
Pair_<BigIntegerImpl> BigIntegerImpl::Truncate(
    const BigIntegerImpl& divisor) const {

  class Local {
    // This is D6(Add back) step of the classic algorithm. This method does
    // U[j+n...j] := U[j+n...j] + V[n-1...0].
    private: static void AddBack(
        Accumulator& uacc,
        const Accumulator vacc,
        int const j) {
      auto carry = Unit(0);
      auto uacc_it = uacc.begin() + j;
      for (auto vacc_it = vacc.begin(); vacc_it != vacc.end(); ++vacc_it) {
        UnsignedAccumulator accumulator(*uacc_it);
        accumulator += *vacc_it;
        accumulator += carry;
        carry = accumulator.high();
        *uacc_it = accumulator.low();
        ++uacc_it;
      }
      *uacc_it += carry;
    }

    // [C]
    // This is D3(calculate qhat) of the classic algorithm.
    private: static Unit CalculateQhat(
        const Accumulator& uacc,
        const Accumulator& vacc,
        int const j) {
      auto const n = vacc.size();
      DCHECK_GE(n, 2);
      auto vacc1 = vacc[n - 1];
      auto vacc2 = vacc[n - 2];
      DCHECK_NE(vacc1, Unit(0));
      auto qhat = Unit(0);
      auto rhat = Unit(0);

      if (uacc[j + n] >= vacc1) {
        qhat = Unit(-1);
        rhat = uacc[j + n - 1];
      } else {
        UnsignedAccumulator high_low(uacc[j + n], uacc[j + n -1]);
        qhat = high_low / vacc1;
        rhat = high_low % vacc1;
      }

      while (uint64(rhat) + vacc1 <= Unit(-1)) {
        UnsignedAccumulator lhs(qhat);
        lhs *= vacc2;
        UnsignedAccumulator rhs(rhat, uacc[j + n - 2]);
        if (lhs <= rhs) {
          break;
        }
        --qhat;
        rhat += vacc1;
      }

      return qhat;
    }

    // Calculates shift amount to make most significant bit of divisor to
    // one. This is part of D1(Normalization) step.
    private: static int CalculateShiftAmount(const Accumulator& vacc) {
      auto const n = vacc.size();
      auto divisor_msu = vacc[n - 1];
      DCHECK_NE(divisor_msu, Unit(0));
      auto const kBaseDiv2 = Unit(1) << (kUnitSize - 1);
      auto shift_amount = 0;
      while (divisor_msu < kBaseDiv2) {
        divisor_msu <<= 1;
        ++shift_amount;
      }
      DCHECK_LT(shift_amount, kUnitSize);
      return shift_amount;
    }

    // Skip leading zero introduced by Accumulator::Negative.
    private: static int SkipLeadingZero(const Vector_<Unit>& v) {
      auto const n = v.size();
      return v[n - 1] ? n : n - 1;
    }

    // [M]
    // U[j+n...j] := U[j+n..j] - qhat * V[n-1...0]
    // Returns true if borrow is occured.
    private: static bool MultiplyAndSubtract(
        Accumulator& uacc,
        const Accumulator& vacc,
        Unit const qhat,
        int const j) {
      if (!qhat) {
        return false;
      }

      auto borrow = Unit(0);
      auto uacc_it = uacc.begin() + j;
      for (auto vacc_it = vacc.begin(); vacc_it != vacc.end(); ++vacc_it) {
        UnsignedAccumulator product(*vacc_it);
        product *= qhat;
        UnsignedAccumulator result(*uacc_it);
        result -= product.low();
        result -= borrow;
        borrow = product.high() - result.high();
        *uacc_it = result.low();
        ++uacc_it;
      }
      *uacc_it -= borrow;
      return SignedUnit(*uacc_it) < 0;
    }

    // [T]
    // Truncates positive dividend by positive divisor.
    public: static Pair_<Vector_<Unit>> Truncate(
        const BigIntegerImpl& dividend,
        const BigIntegerImpl& divisor) {

      if (auto const diff = dividend.CompareTo(divisor)) {
        if (diff < 0) {
          // dividend < dividend -> 0, dividend
          return Pair_<Vector_<Unit>>(MakeVector(0), dividend.units());
        }
      } else {
        // dividend == divisor -> 1, 0
        return Pair_<Vector_<Unit>>(MakeVector(1), MakeVector(0));
      }

      auto const n = SkipLeadingZero(divisor.units_);
      if (n == 1) {
        return Truncate(dividend, divisor.units_[0]);
      }

      // Following algorithm works only n >= 2 and m >= n.
      auto const m = SkipLeadingZero(dividend.units_);
      DCHECK_GE(m, n);
      DCHECK_GE(n, 2);

      Accumulator uacc(m + 1);
      uacc.Set(0, dividend.begin(), dividend.begin() + m);
      uacc[m] = 0;

      Accumulator vacc(n);
      vacc.Set(0, divisor.begin(), divisor.begin() + n);

      Accumulator qacc(m - n + 1);

      // D1: Normalie
      auto const shift_amount = CalculateShiftAmount(vacc);
      if (shift_amount) {
        uacc <<= shift_amount;
        vacc <<= shift_amount;
      }

      // D2: Q[m-n-1...0] := U[m-1...n] / V[n-1...0]
      for (auto j = m - n; j >= 0; --j) {
        auto const qhat = CalculateQhat(uacc, vacc, j);
        qacc[j] = qhat;

        // D4: Multiply and Subtract
        //   U[j+n...j] := U[j+n..j] - qhat * V[n-1...0]
        if (MultiplyAndSubtract(uacc, vacc, qhat, j)) {
          --qacc[j];
          // D6: Add back
          //   U[j+n...j] := U[j+n...j] + V[n-1...0]
          AddBack(uacc, vacc, j);
        }
      }

      // D8: Shift right for remainder
      if (shift_amount) {
        uacc >>= shift_amount;
      }

      return Pair_<Vector_<Unit>>(qacc, uacc);
    }

    // Truncates BigInteger by one unit.
    private: static Pair_<Vector_<Unit>> Truncate(
        const BigIntegerImpl& dividend,
        Unit const divisor) {
      Accumulator quotient(dividend.size());
      auto quotient_it = quotient.rbegin();
      auto remainder = Unit(0);
      for (auto it = dividend.rbegin(); it != dividend.rend(); ++it) {
        UnsignedAccumulator accumulator(remainder, *it);
        remainder = accumulator % divisor;
        accumulator /= divisor;
        *quotient_it = accumulator.low();
        ++quotient_it;
      }
      return Pair_<Vector_<Unit>>(quotient, MakeVector(remainder));
    }
  };

  DCHECK(!divisor.IsZero());
  if (IsZero()) {
    return Pair_<BigIntegerImpl>(0, 0);
  }

  if (size() + divisor.size() == 2) {
    auto const dividend_unit = SignedUnit(units_[0]);
    auto const divisor_unit = SignedUnit(divisor.units_[0]);
    return Pair_<BigIntegerImpl>(
        BigIntegerImpl(dividend_unit / divisor_unit),
        BigIntegerImpl(dividend_unit % divisor_unit));
  }

  // Adjust sign of dividend, divisor and quotient and remainder.
  if (IsPlus()) {
      if (IsPlus()) {
        // P/P => P, P
        auto result = Local::Truncate(*this, divisor);
        return Pair_<BigIntegerImpl>(
            BigIntegerImpl(result.first()),
            BigIntegerImpl(result.second()));
      }
      // P/N => N,P
      auto result = Local::Truncate(*this, -divisor);
      return Pair_<BigIntegerImpl>(
          Accumulator::Negate(result.first()),
          BigIntegerImpl(result.second()));
  }

  if (divisor.IsMinus()) {
    // N/N => P,N
    auto result = Local::Truncate(-*this, -divisor);
    return Pair_<BigIntegerImpl>(
        BigIntegerImpl(result.first()),
        Accumulator::Negate(result.second()));
  }

  // N/P => N, N
  auto result = Local::Truncate(-*this, divisor);
  return Pair_<BigIntegerImpl>(
      Accumulator::Negate(result.first()),
      Accumulator::Negate(result.second()));
}

} // Numerics
} // Common
