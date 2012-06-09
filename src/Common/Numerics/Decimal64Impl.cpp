#include "precomp.h"
// @(#)$Id$
//
// Evita Common - CommonInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Decimal64Impl.h"

#include <float.h>

#include "../Box.h"
#include "../DebugHelper.h"
#include "../StringBuilder.h"

#include "./Float64Impl.h"

namespace Common {
namespace Numerics {

#include "./DecimalTables.h"

namespace {

static const uint64 ClassMask = uint64(0x7FFF) << 48;
static const uint64 ExponentMask = uint64(0x7FF0) << 16;
static const uint64 SpecialValueMask = uint64(0x7FF0) << 16;
static const uint64 ZeroMask = uint64(0xFFF0) << 48;
static const int NumberOfExponentBits = 11;
static const int NumberOfCoefficientBits = 52;
static const int ExponentShift = NumberOfCoefficientBits;
static const uint64 MaxCoefficient = (uint64(1) << NumberOfCoefficientBits) - 1;

static const uint64 NegativeInfinityPattern = uint64(0xFFF0) << 48;
static const uint64 PositiveInfinityPattern = uint64(0x7FF0) << 48;

static const uint64 NegativeQuietNaNPattern = uint64(0xFFF1) << 48;
static const uint64 PositiveQuietNaNPattern = uint64(0x7FF1) << 48;

static const uint64 NegativeSignalingNaNPattern = uint64(0xFFF1) << 48;
static const uint64 PositiveSignalingNaNPattern = uint64(0x7FF1) << 48;

static const uint64 NegativeZeroPattern = uint64(0xBFF0) << 48;
static const uint64 PositiveZeroPattern = uint64(0x3FF0) << 48;

static const uint64 NegativePattern = uint64(0x8000) << 48;
static const uint64 PositivePattern = 0;

class SpecialValueHandler {
  public: enum HandleResult {
    BothFinite,
    BothInfinity,
    EitherNaN,
    LhsIsInfinity,
    RhsIsInfinity,
  };

  private: enum Result {
    ResultIsLhs,
    ResultIsRhs,
    ResultIsUnknown,
  };

  private: const Decimal64Impl& lhs_;
  private: Result result_;
  private: const Decimal64Impl& rhs_;

  public: SpecialValueHandler(
    const Decimal64Impl& lhs,
    const Decimal64Impl& rhs)
        : lhs_(lhs), result_(ResultIsUnknown), rhs_(rhs) {}

  public: Decimal64Impl value() const {
    switch (result_) {
      case ResultIsLhs:
        return lhs_;
      case ResultIsRhs:
        return rhs_;
      case ResultIsUnknown:
      default:
        CAN_NOT_HAPPEN();
    }
  }

  public: HandleResult Handle() {
    if (lhs_.is_finite() && rhs_.is_finite()) {
      return BothFinite;
    }

    auto const lhs_class = lhs_.ieee_class();
    auto const rhs_class = rhs_.ieee_class();
    if (lhs_class == Class_QuietNaN) {
      result_ = ResultIsLhs;
      return EitherNaN;
     }

    if (rhs_class == Class_QuietNaN) {
      result_ = ResultIsRhs;
      return EitherNaN;
    }

    if (lhs_class == Class_Infinity) {
      return rhs_class == Class_Infinity ? BothInfinity : LhsIsInfinity;
    }

    if (rhs_class == Class_Infinity) {
      return RhsIsInfinity;
    }

    CAN_NOT_HAPPEN();
  }

  DISALLOW_COPY_AND_ASSIGN(SpecialValueHandler);
};

class UInt128 {
  private: uint64 high_;
  private: uint64 low_;
  public: UInt128(uint64 low, uint64 high) : high_(high), low_(low) {}

  public: UInt128& operator /=(uint32);

  public: uint64 high() const { return high_; }
  private: bool is_zero() const { return !low_ && !high_; }
  public: uint64 low() const { return low_; }

  public: static UInt128 Multiply(uint64, uint64);
};

static int CountDigits(uint64 x) {
  auto num_digits = 0;
  while (x > 0) {
    x /= 10;
    ++num_digits;
  }
  return num_digits;
}
static uint64 MultiplyHigh(uint64 u, uint64 v) {
  auto const n = 64;
  auto const n2 = n / 2;
  auto const mask = (uint64(1) << n2) - 1;
  uint64 u0 = u & mask;
  uint64 u1 = u >> n2;
  uint64 v0 = v & mask;
  uint64 v1 = v >> n2;
  uint64 w0 = u0 * v0;
  uint64 tt = u1 * v0 + (w0 >> n2);
  uint64 w1 = tt & mask;
  uint64 w2 = tt >> n2;
  w1 = u0 * v1 + w1;
  return u1 * v1 + w2 + (w1 >> n2);
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

UInt128& UInt128::operator /=(uint32 const divisor) {
  if (!high_) {
    low_ /= divisor;
    return *this;
  }

  uint32 dividend[4];
  dividend[0] = uint32(low_ & uint32(-1));
  dividend[1] = uint32(low_ >> 32);
  dividend[2] = uint32(high_ & uint32(-1));
  dividend[3] = uint32(high_ >> 32);

  uint32 quotient[4];
  uint32 remainder = 0;
  for (auto i = 3; i >= 0; --i) {
    uint64 const work = (uint64(remainder) << 32) | dividend[i];
    remainder = uint32(work % divisor);
    quotient[i] = uint32(work / divisor);
  }
  low_ = quotient[0] | (uint64(quotient[1]) << 32);
  high_ = quotient[2] | (uint64(quotient[3]) << 32);
  return *this;
}

UInt128 UInt128::Multiply(uint64 u, uint64 v) {
  return UInt128(u * v, MultiplyHigh(u, v));
}

Decimal64Impl::EncodedData::EncodedData(uint64 u64) : u64_(u64) {}

Decimal64Impl::EncodedData::EncodedData(
    uint64 const value,
    int const exponent,
    int const sign) {
  auto raw_exponent = exponent + Decimal64Impl::Ebias;
  auto significand = value;
  while (significand > MaxCoefficient) {
    significand /= 10;
    ++raw_exponent;
  }

  if (raw_exponent < 0) {
    u64_ = sign ? NegativeZeroPattern : PositiveZeroPattern;
    return;
  }

  if (raw_exponent >= Decimal64Impl::Emax) {
    u64_ = sign ? NegativeInfinityPattern : PositiveInfinityPattern;
    return;
  }

  u64_ = (sign ? NegativePattern : PositivePattern)
      | (uint64(raw_exponent) << ExponentShift)
      | significand;
}

Decimal64Impl::EncodedData::EncodedData(
    uint64 low,
    uint64 high,
    int exponent,
    int const sign) {
  UInt128 work(low, high);
  while (work.high()) {
    work /= uint32(10);
    ++exponent;
  }

  auto coefficent = work.low();
  while (coefficent > MaxCoefficient) {
    coefficent /= 10;
    ++exponent;
  }

  *this = EncodedData(coefficent, exponent, sign);
}

uint64 Decimal64Impl::EncodedData::coefficent() const {
  return u64_ & ~ZeroMask;
}

int Decimal64Impl::EncodedData::exponent() const {
  return raw_exponent() - Decimal64Impl::Ebias;
}

bool Decimal64Impl::EncodedData::is_finite() const {
  return !is_special();
}

bool Decimal64Impl::EncodedData::is_special() const {
  return raw_exponent() == Decimal64Impl::Emax;
}

bool Decimal64Impl::EncodedData::is_zero() const {
  return is_finite() && !(u64_ & ~ZeroMask);
}

int Decimal64Impl::EncodedData::raw_exponent() const {
  return int(u64_ >> ExponentShift) & ((1 << NumberOfExponentBits) - 1);
}

int Decimal64Impl::EncodedData::sign() const {
  return int64(u64_) < 0;
}

void Decimal64Impl::EncodedData::set_sign(int const sign) {
  DCHECK_LE(uint(sign), uint(1));
  if (sign) {
    u64_ |= uint64(1) << 63;
  } else {
    u64_ &= ~(uint64(1) << 63);
  }
}

// Convert float64 into Decimal64.
Decimal64Impl::Decimal64Impl(float64 const f64) : data_(0) {
  Float64Impl value(f64);
  switch (value.ieee_class()) {
    case Class_Infinity:
      *this = Infinity(value.sign());
      return;

    case Class_Normal: {
      Decimal64Impl signficand(value.significand(), 0, value.sign());
      auto const exponent = value.exponent() - Float64Impl::kSignificandBits;
      if (!exponent) {
        *this = signficand;
        return;
      }

      if (exponent > 0) {
        auto const power2 = Power(Decimal64Impl(2), exponent);
        *this = signficand * power2;
        return;
      }

      auto const power2 = Power(Decimal64Impl(2), -exponent);
      *this = (signficand / power2).RoundAt(Float64Impl::kDigits);
      return;
    }

    case Class_QuietNaN:
    case Class_SignalingNaN:
      *this = QuietNaN(value.sign());
      return;

    case Class_Subnormal: {
      Decimal64Impl signficand(value.raw_significand(), 0, value.sign());
      auto const power2 = Power(Decimal64Impl(2), -Float64Impl::kEsubnormal);
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

Decimal64Impl::Decimal64Impl(int32 const i32)
  : data_(i32 < 0 ? -i32 : i32, 0, i32 < 0) {}

Decimal64Impl::Decimal64Impl(const EncodedData& data) : data_(data) {}
Decimal64Impl::Decimal64Impl(const Decimal64Impl& rhs) : data_(rhs.data_) {}

Decimal64Impl::Decimal64Impl(
    uint64 const coefficent,
    int const exponent,
    int sing)
    : data_(coefficent, exponent, sing) {}

// operators
Decimal64Impl::operator float64() const {
  switch (ieee_class()) {
    case Class_Infinity:
      return Float64Impl(sign()
          ? Float64Impl::kNegativeInfinityPattern
          : Float64Impl::kPositiveInfinityPattern);

    case Class_QuietNaN:
      return Float64Impl(sign()
          ? Float64Impl::kNegativeQuietNaNPattern
          : Float64Impl::kPositiveQuietNaNPattern);

    case Class_Normal: {
      auto const exp = data_.exponent();
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

      auto const x = float64(data_.coefficent());
      auto const y = exp >= 0 ? x * Power(10.0, exp) : x / Power(10.0, -exp);
      return sign() ? -y : y;
    }

    case Class_Zero:
      return Float64Impl(sign()
          ? Float64Impl::kNegativeZeroPattern
          : Float64Impl::kPositiveZeroPattern);

    case Class_SignalingNaN:
    case Class_Subnormal:
    default:
      CAN_NOT_HAPPEN();
  }
}

Decimal64Impl& Decimal64Impl::operator =(const Decimal64Impl& rhs) {
  data_ = rhs.data_;
  return *this;
}

Decimal64Impl Decimal64Impl::operator +(const Decimal64Impl& rhs) const {
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

  auto const lhs_exp = lhs.exponent();
  auto const rhs_exp = rhs.exponent();
  auto result_exp = min(lhs_exp, rhs_exp);
  auto lhs_data = lhs.data_.coefficent();
  auto rhs_data = rhs.data_.coefficent();

  // Make coefficent aligned.
  if (lhs_exp > rhs_exp) {
    auto const lhs_num_digits = CountDigits(lhs_data);
    if (lhs_num_digits) {
      auto const lhs_shift = lhs_exp - rhs_exp;
      auto const overflow = lhs_num_digits + lhs_shift - (NumberOfDigits + 1);
      if (overflow <= 0) {
        lhs_data *= Power(uint64(10), lhs_shift);
      } else {
        lhs_data *= Power(uint64(10), lhs_shift - overflow);
        rhs_data /= Power(uint64(10), overflow);
        result_exp += overflow;
      }
    }

  } if (lhs_exp < rhs_exp) {
    auto const rhs_num_digits = CountDigits(rhs_data);
    if (rhs_num_digits) {
      auto const rhs_shift = rhs_exp - lhs_exp;
      auto const overflow = rhs_num_digits + rhs_shift - (NumberOfDigits + 1);
      if (overflow <= 0) {
        rhs_data *= Power(uint64(10), rhs_shift);
      } else {
        rhs_data *= Power(uint64(10), rhs_shift - overflow);
        lhs_data /= Power(uint64(10), overflow);
        result_exp += overflow;
      }
    }
  }

  auto const result = lhs_sign == rhs_sign
      ? lhs_data + rhs_data
      : lhs_data - rhs_data;
  return lhs_sign && !rhs_sign && !result
      ? Decimal64Impl(0, result_exp, 0)
      : int64(result) >= 0
            ? Decimal64Impl(result, result_exp, lhs_sign)
            : Decimal64Impl(-int64(result), result_exp, lhs_sign ^ 1);
}

Decimal64Impl Decimal64Impl::operator -(const Decimal64Impl& rhs) const {
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

  auto lhs_data = lhs.data_.coefficent();
  auto rhs_data = rhs.data_.coefficent();

  auto const lhs_exp = lhs.exponent();
  auto const rhs_exp = rhs.exponent();
  auto result_exp = min(lhs_exp, rhs_exp);

  // Make coefficent aligned.
  if (lhs_exp > rhs_exp) {
    auto const lhs_num_digits = CountDigits(lhs_data);
    if (lhs_num_digits) {
      auto const lhs_shift = lhs_exp - rhs_exp;
      auto const overflow = lhs_num_digits + lhs_shift - (NumberOfDigits + 1);
      if (overflow <= 0) {
        lhs_data *= Power(uint64(10), lhs_shift);
      } else {
        lhs_data *= Power(uint64(10), lhs_shift - overflow);
        rhs_data /= Power(uint64(10), overflow);
        result_exp += overflow;
      }
    }

  } if (lhs_exp < rhs_exp) {
    auto const rhs_num_digits = CountDigits(rhs_data);
    if (rhs_num_digits) {
      auto const rhs_shift = rhs_exp - lhs_exp;
      auto const overflow = rhs_num_digits + rhs_shift - (NumberOfDigits + 1);
      if (overflow <= 0) {
        rhs_data *= Power(uint64(10), rhs_shift);
      } else {
        rhs_data *= Power(uint64(10), rhs_shift - overflow);
        lhs_data /= Power(uint64(10), overflow);
        result_exp += overflow;
      }
    }
  }

  auto const result = lhs_sign == rhs_sign
      ? lhs_data - rhs_data
      : lhs_data + rhs_data;
  return lhs_sign && rhs_sign && !result
      ? Decimal64Impl(0, result_exp, 0)
      : int64(result) >= 0
            ? Decimal64Impl(result, result_exp, lhs_sign)
            : Decimal64Impl(-int64(result), result_exp, lhs_sign ^ 1);
}

Decimal64Impl Decimal64Impl::operator *(const Decimal64Impl& rhs) const {
  auto& lhs = *this;
  auto const lhs_sign = lhs.sign();
  auto const rhs_sign = rhs.sign();
  auto const result_sign = lhs_sign ^ rhs_sign;

  SpecialValueHandler handler(lhs, rhs);
  switch (handler.Handle()) {
    case SpecialValueHandler::BothFinite: {
      auto const lhs_data = lhs.data_.coefficent();
      auto const rhs_data = rhs.data_.coefficent();
      auto const low = lhs_data * rhs_data;
      auto const high = MultiplyHigh(lhs_data, rhs_data);
      auto const result_exp = lhs.exponent() + rhs.exponent();
      return Decimal64Impl(EncodedData(low, high, result_exp, result_sign));
    }

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

Decimal64Impl Decimal64Impl::operator /(const Decimal64Impl& rhs) const {
  return Divide(rhs, FloatResult);
}

bool Decimal64Impl::operator==(const Decimal64Impl& rhs) const {
  return FastEquals(rhs) || CompareTo(rhs).is_zero();
}

bool Decimal64Impl::operator!=(const Decimal64Impl& rhs) const {
  return !operator ==(rhs);
}

bool Decimal64Impl::operator <(const Decimal64Impl& rhs) const {
  return !operator >=(rhs);
}

bool Decimal64Impl::operator <=(const Decimal64Impl& rhs) const {
  if (FastEquals(rhs)) return true;
  auto const result = CompareTo(rhs);
  return result.is_zero() || result.sign();
}

bool Decimal64Impl::operator >(const Decimal64Impl& rhs) const {
  return !operator <=(rhs);
}

bool Decimal64Impl::operator >=(const Decimal64Impl& rhs) const {
  if (FastEquals(rhs)) return true;
  auto const result = CompareTo(rhs);
  return result.is_zero() || !result.sign();
}

// properties
int Decimal64Impl::exponent() const {
  return is_finite() ? data_.exponent() : 0;
}

IeeeClass Decimal64Impl::ieee_class() const {
  switch (data_.u64_ & ClassMask) {
    case PositiveInfinityPattern:
    case NegativeInfinityPattern:
      return Class_Infinity;

    case PositiveQuietNaNPattern:
    case NegativeQuietNaNPattern:
      return Class_QuietNaN;
  }

  return data_.is_zero() ? Class_Zero : Class_Normal;
}

int Decimal64Impl::sign() const {
  return data_.sign();
}

void Decimal64Impl::set_sign(int value) {
  data_.set_sign(value);
}

// [C]
Decimal64Impl Decimal64Impl::CompareTo(const Decimal64Impl& rhs) const {
  auto const result = *this - rhs;
  return result.is_zero()
      ? Zero(0)
      : result.is_finite()
          ? result
          : result.sign() ? Decimal64Impl(-1) : Decimal64Impl(1);
}

// [D]
Decimal64Impl Decimal64Impl::Divide(
    const Decimal64Impl& rhs,
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

  auto result_exponent = lhs.exponent() - rhs.exponent();

  if (lhs.is_zero()) {
    // 0/non-zero => 0
    return result_type == FloatResult
      ? Decimal64Impl(0, result_exponent, result_sign)
      : Zero(result_sign);
  }

  auto remainder = lhs.data_.coefficent();
  auto const divisor =  rhs.data_.coefficent();
  auto quotient = uint64(0);

  while (quotient < MaxCoefficient / 10) {
    while (remainder < divisor) {
      remainder *= 10;
      quotient *= 10;
      --result_exponent;
    }

    quotient += remainder / divisor;
    remainder %= divisor;

    if (!remainder) {
      break;
    }
  }

  if (remainder > divisor / 2) {
    ++quotient;
  }

  switch (result_type) {
    case FloatResult:
      return Decimal64Impl(quotient, result_exponent, result_sign);

    case IntegerResult: {
      auto const num_digits = CountDigits(quotient);
      if (num_digits + result_exponent > Decimal64Impl::Precision) {
        // QUotient is too large for integer
        return Decimal64Impl::QuietNaN(0);
      }

      if (result_exponent >= 0) {
        quotient *= Power(uint64(10), result_exponent);
        return Decimal64Impl(quotient, 0, result_sign);
      }

      auto const num_drop = -result_exponent;
      if (num_digits > num_drop) {
        quotient /= Power(uint64(10), num_drop);
        return  Decimal64Impl(quotient, 0, result_sign);
      }

      return Zero(result_sign);
    }

    default:
      CAN_NOT_HAPPEN();
  }
}

// [F]
// Compares two Decimal64Impl number as binary string. This method is
// used for equality of infinity operands which substract operation
// returns non-zero.
bool Decimal64Impl::FastEquals(const Decimal64Impl& rhs) const {
  return data_.u64_ == rhs.data_.u64_;
}

Decimal64Impl Decimal64Impl::FromString(const String& str) {
  auto exponent = 0;
  auto exponent_sign = 0;
  auto minus = false;
  auto num_digits = 0;
  auto num_digits_after_dot = 0;

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
    State_Sign,
    State_Start,
    State_Zero,
  } state = State_Start;

  #define HandleChar(ch1, next, ...) \
    if (ch == ch1 || ch == ch1 - 'A' + 'a') { \
      __VA_ARGS__; state = next; break; \
    }

  #define ExpectChar(ch1, next, ...) \
    HandleChar(ch1, next, __VA_ARGS__); return QuietNaN(0);

  auto accumulator = uint64(0);
  for (auto it = str.begin(); it != str.end(); ++it) {
    auto const ch = *it;
    switch (state) {
      case State_Digit:
        if (ch >= '0' && ch <= '9') {
          if (num_digits < Decimal64Impl::NumberOfDigits) {
            ++num_digits;
            accumulator *= 10;
            accumulator += ch - '0';
          }
          break;
        }

        if (ch == '.') {
          state = State_Dot;
          break;
        }

        ExpectChar('E', State_E);

      case State_Dot:
        if (ch >= '0' && ch <= '9') {
          if (num_digits < Decimal64Impl::NumberOfDigits) {
            ++num_digits;
            ++num_digits_after_dot;
            accumulator *= 10;
            accumulator += ch - '0';
          }
          state = State_Dot_Digit;
          break;
        }

      case State_Dot_Digit:
        if (ch >= '0' && ch <= '9') {
          if (num_digits < Decimal64Impl::NumberOfDigits) {
            ++num_digits;
            ++num_digits_after_dot;
            accumulator *= 10;
            accumulator += ch - '0';
          }
          break;
        }

        ExpectChar('E', State_E);

      case State_E:
        if (ch == '+') {
          exponent_sign = 0;
          state = State_E_Sign;
          break;
        }

        if (ch == '-') {
          exponent_sign = 1;
          state = State_E_Sign;
          break;
        }

        if (ch >= '0' && ch <= '9') {
          exponent = ch - '0';
          state = State_E_Digit;
          break;
        }

        return QuietNaN(0);

       case State_E_Digit:
        if (ch >= '0' && ch <= '9') {
          exponent *= 10;
          exponent += ch - '0';
          if (exponent > Decimal64Impl::Emax + NumberOfDigits) {
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

      case State_I: ExpectChar('N', State_IN);
      case State_IN: ExpectChar('F', State_INF);
      case State_INF: ExpectChar('I', State_INFI);
      case State_INFI: ExpectChar('N', State_INFIN);
      case State_INFIN: ExpectChar('I', State_INFINI);
      case State_INFINI: ExpectChar('T', State_INFINIT);
      case State_INFINIT: ExpectChar('Y', State_INFINITY);
      case State_INFINITY: return QuietNaN(0);

      case State_N: ExpectChar('A', State_NA);
      case State_NA: ExpectChar('N', State_NAN);
      case State_NAN: return QuietNaN(0);

      case State_Sign:
        if (ch == '0') {
          state = State_Zero;
          break;
        }

        if (ch >= '1' && ch <= '9') {
          accumulator = ch - '0';
          num_digits = 1;
          state = State_Digit;
          break;
        }

        HandleChar('I', State_I);
        HandleChar('N', State_N);
        return QuietNaN(0);

      case State_Start:
        if (ch == '0') {
          state = State_Zero;
          break;
        }

        if (ch >= '1' && ch <= '9') {
          accumulator = ch - '0';
          num_digits = 1;
          state = State_Digit;
          break;
        }

        if (ch == '-') {
          minus = true;
          state = State_Sign;
          break;
        }

        if (ch == '+') {
          minus = false;
          state = State_Sign;
          break;
        }

        HandleChar('I', State_I);
        HandleChar('N', State_N);
        return QuietNaN(0);

      case State_Zero:
        if (ch == '0') {
          break;
        }

        if (ch >= '1' && ch <= '9') {
          accumulator = ch - '0';
          num_digits = 1;
          state = State_Digit;
          break;
        }

        if (ch == '.') {
          state = State_Dot;
          break;
        }

        ExpectChar('E', State_E);

      default:
        CAN_NOT_HAPPEN();
    }
  }

  if (state == State_INF || state == State_INFINITY) {
    return Infinity(minus);
  }

  if (state == State_NAN) {
    return QuietNaN(minus);
  }

  if (state == State_Zero) {
    return Zero(minus);
  }

  if (state == State_Digit
      || state == State_E_Digit
      || state == State_Dot_Digit) {
    auto result_exponent = exponent * (1 - exponent_sign * 2)
        - num_digits_after_dot;

    auto const biased_exponent = result_exponent + Ebias;
    if (biased_exponent < 0) {
      return Zero(0);
    }

    auto const overflow = biased_exponent - Emax + 1;
    if (overflow > 0) {
      if (overflow + num_digits - num_digits_after_dot > NumberOfDigits) {
        return Infinity(minus);
      }
      accumulator *= Power(uint64(10), overflow);
      result_exponent -= overflow;
    }

    return Decimal64Impl(accumulator, result_exponent, minus);
  }

  return QuietNaN(0);
}

// [I]
Decimal64Impl Decimal64Impl::Infinity(int const sign) {
  return Decimal64Impl(
      EncodedData(
        sign ? NegativeInfinityPattern : PositiveInfinityPattern));
}

// [Q]
Decimal64Impl Decimal64Impl::QuietNaN(int const sign) {
  return Decimal64Impl(
      EncodedData(sign ? NegativeQuietNaNPattern : PositiveQuietNaNPattern));
}

// [R]
Decimal64Impl Decimal64Impl::Remainder(const Decimal64Impl& rhs) const {
  auto const quotient = Divide(rhs, IntegerResult);
  if (quotient.is_special()) {
    return quotient;
  }

  auto rem = *this - quotient * rhs;
  rem.set_sign(sign());
  return rem;
}

Decimal64Impl Decimal64Impl::RoundAt(int const precision) {
  auto coefficent = data_.coefficent();
  auto exponent = data_.exponent();

  if (coefficent >= Power(uint64(10), precision - 2)) {
    coefficent /= 10;
    ++exponent;
    if (coefficent % 10 >= 5) {
      coefficent = (coefficent + 1) / 10;
      ++exponent;
    }
  }

  while (coefficent && (coefficent % 10) == 0) {
    coefficent /= 10;
    ++exponent;
  }

  return Decimal64Impl(coefficent, exponent, data_.sign());
}

// [T]
String Decimal64Impl::ToString() const {
  switch (ieee_class()) {
    case Class_Infinity:
      return sign() ? "-Infinity" : "Infinity";

    case Class_QuietNaN:
      return sign() ? "-NaN" : "NaN";

    case Class_Normal:
    case Class_Zero:
      break;

    case Class_SignalingNaN:
    case Class_Subnormal:
    default:
      CAN_NOT_HAPPEN();
  }

  StringBuilder builder;
  if (sign()) {
    builder.Append('-');
  }

  // convert to scientific-string
  auto const digits = String::Format("%u", data_.coefficent());
  auto const coefficent_length = int(digits.length());
  auto const original_exponent = exponent();
  auto const adjusted_exponent = original_exponent + coefficent_length - 1;
  if (original_exponent <= 0 && adjusted_exponent >= -6) {
    if (!original_exponent) {
      builder.Append(digits);
      return builder.ToString();
    }

    if (adjusted_exponent >= 0) {
      for (auto i = 0; i < coefficent_length; ++i) {
        builder.Append(digits[i]);
        if (i == adjusted_exponent) {
          builder.Append('.');
        }
      }
      return builder.ToString();
    }

    builder.Append("0.");
    for (auto i = adjusted_exponent + 1; i < 0; ++i) {
      builder.Append('0');
    }

    builder.Append(digits);

  } else {
    builder.Append(digits[0]);
    if (coefficent_length >= 2) {
      builder.Append('.');
      for (auto it = digits.begin() + 1; it != digits.end(); ++it) {
        builder.Append(*it);
      }
    }

    if (adjusted_exponent) {
      builder.AppendFormat("E%+d", adjusted_exponent);
    }
  }
  return builder.ToString();
}

// [Z]
Decimal64Impl Decimal64Impl::Zero(int sign) {
  return Decimal64Impl(
      EncodedData(sign ? NegativeZeroPattern : PositiveZeroPattern));
}

} // Numerics
} // Common
