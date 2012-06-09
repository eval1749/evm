// @(#)$Id$
//
// Evita Lexer Checker
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_AbstractDecimal128Test_h)
#define INCLUDE_test_AbstractDecimal128Test_h

#include "../AbstractCommonTest.h"

#include "Common/Numerics/Decimal128.h"

namespace Common {
namespace Numerics {
::std::ostream& operator<<(::std::ostream& os, const Decimal128& d);
} // Numerics
} // Common

namespace CommonTest {

using namespace Common;
using Common::Numerics::Decimal128;

union Float64UInt64 {
  float64 f64_;
  uint64 u64_;
};

class AbstractDecimal128Test : public ::testing::Test {
  protected: String Add(const Decimal128& lhs, const Decimal128& rhs) {
    return (lhs + rhs).ToString();
  }

  protected: String Add(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    auto const result = lhs + rhs;
    return result.ToString();
  }

  protected: String Div(const Decimal128& lhs, const Decimal128& rhs) {
    return (lhs / rhs).ToString();
  }

  protected: String Div(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    auto const result = lhs / rhs;
    return result.ToString();
  }

  protected: String DivInt(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    auto const result = lhs.Truncate(rhs);
    return result.ToString();
  }

  protected: bool Equal(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    return lhs == rhs;
  }

  protected: String Float64(float64 f64) {
    return Decimal128(f64).ToString();
  }

  protected: Decimal128 FromString(const String str) {
    return Decimal128::FromString(str);
  }

  protected: bool GreaterThan(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    return lhs > rhs;
  }

  protected: Decimal128 Infinity(int sign = 0) {
    return sign ? FromString("-Inf") : FromString("Inf");
  }

  protected: bool IsFloat64QuietNaN(float64 x, int sign) {
    Float64UInt64 fu64;
    fu64.f64_ = x;
    auto const high16 = uint32(fu64.u64_ >> 48);
    return sign ? high16 == 0xFFF8 : high16 == 0x7FF8;
  }

  protected: bool IsFloat64SignalingNaN(float64 x, int sign) {
    Float64UInt64 fu64;
    fu64.f64_ = x;
    auto const high16 = uint32(fu64.u64_ >> 48) & 0xFFF8;
    auto const low51 = fu64.u64_ & ((uint64(1) << 51) - 1);
    return (sign ? high16 == 0xFFF0 : high16 == 0x7FF0) && low51 != 0;
  }

  protected: bool LessThan(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    return lhs < rhs;
  }

  protected: float64 MakeFloat64(
      uint const sign,
      uint64 const raw_significand,
      uint const raw_exponent) {
    ASSERT(raw_exponent <= 2047);
    ASSERT(raw_significand <= (uint64(1) << 52) - 1);
    Float64UInt64 fu64;
    fu64.u64_ = (uint64(sign) << 63) 
        | (uint64(raw_exponent) << 52) 
        | raw_significand;
    return fu64.f64_;
  }

  protected: String Mul(const Decimal128& lhs, const Decimal128& rhs) {
    return (lhs * rhs).ToString();
  }

  protected: String Mul(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    auto const result = lhs * rhs;
    return result.ToString();
  }

  protected: Decimal128 QuietNaN(int sign = 0) {
    return sign
        ? Decimal128::FromString("-NaN")
        : Decimal128::FromString("NaN");
  }

  protected: String Remainder(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    auto const result = lhs.Remainder(rhs);
    return result.ToString();
  }

  // [S]
  protected: Decimal128 SignalingNaN(int sign = 0) {
    return sign
        ? Decimal128::FromString("-sNaN")
        : Decimal128::FromString("sNaN");
  }

  protected: String Sub(const Decimal128& lhs, const Decimal128& rhs) {
    return (lhs - rhs).ToString();
  }

  protected: String Sub(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    auto const result = lhs - rhs;
    return result.ToString();
  }

  protected: String Truncate(const String lhs_str, const String rhs_str) {
    auto const lhs = FromString(lhs_str);
    auto const rhs = FromString(rhs_str);
    auto const result = lhs.Truncate(rhs);
    return result.ToString();
  }

  protected: Decimal128 Zero(int sign = 0) {
    return sign ? Decimal128::FromString("-0") : Decimal128::FromString("0");
  }
};

} // CommonTest

#endif // !defined(INCLUDE_test_AbstractDecimal128Test_h)
