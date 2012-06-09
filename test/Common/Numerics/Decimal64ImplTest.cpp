#include "precomp.h"
// @(#)$Id$
//
// Util Test for Decimal64Impl
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCommonTest.h"

#include "Common/Numerics/Decimal64Impl.h"

namespace Common {
namespace Numerics {
::std::ostream& operator<<(::std::ostream& os, const Decimal64Impl& d) {
  return os << String::Format("EncodedData(%d, %d, %d)",
      d.value().coefficent(),
      d.value().exponent(),
      d.value().sign());
}
} // Numerics
} // Common

namespace CommonTest {

using namespace Common::Numerics;

class Float64 {
  private: union Float64UInt64 {
    float64 float64_;
    uint64 uint64_;
  } value_;

  public: Float64(
      uint64 const signficand,
      int const exponent,
      int const sign) {
    value_.uint64_ = signficand
        | (uint64(exponent + 1023) << 52)
        | (uint64(sign) << 63);
  }

  public: Float64(float64 const f64) {
    value_.float64_ = f64;
  }

  public: operator float64() const { return value_.float64_; }

  public: bool is_nan(int const sign) const {
    auto const msb16 = uint32(value_.uint64_ >> 48);
    return sign ? msb16 == 0xFFF8 : msb16 == 0x7FF8;
  }
};

class Decimal64ImplTest : public AbstractCommonTest {
  protected: typedef Decimal64Impl Dec64;

  protected: Decimal64Impl Encode(uint64 coefficient, int exponent,
                                  int sign) {
    return Decimal64Impl(Decimal64Impl::EncodedData(coefficient, exponent,
                                                    sign));
  }

  protected: Decimal64Impl FromFloat64(
      uint64 const signficand,
      int const exponent,
      int const sign) {
    return Decimal64Impl(float64(Float64(signficand, exponent, sign)));
  }

  protected: Decimal64Impl FromString(const String& string) {
    return Decimal64Impl::FromString(string);
  }
};

#if 0
// http://www.whatwg.org/specs/web-apps/current-work/multipage/common-input-element-attributes.html
// <input name=opacity type=range min=0 max=1 step=0.00392156863>
// http://jsfiddle.net/hk6WU/
TEST_F(Decimal64ImplTest, AppOpacityStepRange) {
    EXPECT_EQ(Float64(1), Float64(stepUp(DecimalStepRange(0, 1, 0.00392156863), 0, 255)));
}
#endif

TEST_F(Decimal64ImplTest, EncodedData) {
  EXPECT_EQ(Encode(0, 0, 0), Encode(0, 0, 0));
  EXPECT_EQ(Encode(0, 0, 1), Encode(0, 0, 1));
  EXPECT_EQ(Encode(1, 0, 0), Encode(1, 0, 0));
  EXPECT_EQ(Encode(1, 0, 1), Encode(1, 0, 1));
  EXPECT_EQ(Dec64::Infinity(0), Encode(1, 2000, 0));
  EXPECT_EQ(Dec64::Zero(0), Encode(1, -2000, 0));
}

TEST_F(Decimal64ImplTest, FromFloa64) {
  EXPECT_EQ(Encode(0, 0, 0), Dec64(0.0));
  EXPECT_EQ(Encode(0, 0, 1), Dec64(-0.0));
  EXPECT_EQ(Encode(1, 0, 0), Dec64(1.0));
  EXPECT_EQ(Encode(1, 0, 1), Dec64(-1.0));

  for (auto i = 1; i <= 9; ++i) {
    EXPECT_EQ(Encode(i, 0, 0), Dec64(i * 1.0));
    EXPECT_EQ(Encode(i, 1, 0), Dec64(i * 10.0));
    EXPECT_EQ(Encode(i, 2, 0), Dec64(i * 100.0));
    EXPECT_EQ(Encode(i, 3, 0), Dec64(i * 1000.0));
    EXPECT_EQ(Encode(i, 4, 0), Dec64(i * 10000.0));
    EXPECT_EQ(Encode(i, 5, 0), Dec64(i * 100000.0));

    EXPECT_EQ(Encode(i, -1, 0), Dec64(i * 0.1));
    EXPECT_EQ(Encode(i, -2, 0), Dec64(i * 0.01));
    EXPECT_EQ(Encode(i, -3, 0), Dec64(i * 0.001));
    EXPECT_EQ(Encode(i, -4, 0), Dec64(i * 0.0001));
    EXPECT_EQ(Encode(i, -5, 0), Dec64(i * 0.00001));
    EXPECT_EQ(Encode(i, -6, 0), Dec64(i * 0.000001));
  }

  EXPECT_EQ(Encode(12345, -2, 0), Dec64(123.45));
  EXPECT_EQ(Encode(12345, -2, 1), Dec64(-123.45));

  // 4503599627370496.0
  EXPECT_EQ(Encode(450359962737049, 1, 0), FromFloat64(0, 52, 0));

  // 9007199254740992.0
  EXPECT_EQ(Encode(900719925474098, 1, 0), FromFloat64(0, 53, 0));

  // e=2.7182818284590452353602874713526624977572470936999595
  EXPECT_STREQ(
      L"2.71828182845904",
      FromFloat64(0x5bf0a8b145769, 1, 0).ToString().value());

  // pi=3.1415926535897932384626433832795028841971693993751058
  EXPECT_STREQ(
      L"3.1415926535898",
      FromFloat64(0x921FB54442D18, 1, 0).ToString().value());
}

TEST_F(Decimal64ImplTest, FromFloa64SpecialValues) {
  EXPECT_EQ(Dec64::Infinity(0), FromFloat64(0, 1024, 0));
  EXPECT_EQ(Dec64::Infinity(1), FromFloat64(0, 1024, 1));
  EXPECT_EQ(Dec64::QuietNaN(0), FromFloat64(uint64(1) << 51, 1024, 0));
  EXPECT_EQ(Dec64::QuietNaN(1), FromFloat64(uint64(1) << 51, 1024, 1));
}

TEST_F(Decimal64ImplTest, FromFloa64Subnormals) {
  // least-positive-double-float(5E-324)
  // MSVS2010 4.940656458412e-324#DEN
  // 4.940656458412465E-324
  EXPECT_EQ(Encode(49406564584124, -337, 0), FromFloat64(1, -1023, 0));
  EXPECT_EQ(Encode(49406564584124, -337, 1), FromFloat64(1, -1023, 1));
}

TEST_F(Decimal64ImplTest, FromInt32) {
  EXPECT_EQ(Encode(0, 0, 0), Dec64(0));
  EXPECT_EQ(Encode(1, 0, 0), Dec64(1));
  EXPECT_EQ(Encode(1, 0, 1), Dec64(-1));
  EXPECT_EQ(Encode(100, 0, 0), Dec64(100));
  EXPECT_EQ(Encode(100, 0, 1), Dec64(-100));
}

TEST_F(Decimal64ImplTest, FromString) {
  EXPECT_EQ(Encode(0, 0, 0), FromString("0"));
  EXPECT_EQ(Encode(0, 0, 1), FromString("-0"));
  EXPECT_EQ(Encode(1, 0, 0), FromString("1"));
  EXPECT_EQ(Encode(1, 0, 1), FromString("-1"));
  EXPECT_EQ(Encode(1, 0, 0), FromString("01"));
  EXPECT_EQ(Encode(0, 3, 0), FromString("0E3"));
  EXPECT_EQ(Encode(100, 0, 0), FromString("100"));
  EXPECT_EQ(Encode(100, 0, 1), FromString("-100"));
  EXPECT_EQ(Encode(123, -2, 0), FromString("1.23"));
  EXPECT_EQ(Encode(123, -2, 1), FromString("-1.23"));
  EXPECT_EQ(Encode(123, 8, 0), FromString("1.23E10"));
  EXPECT_EQ(Encode(123, 8, 1), FromString("-1.23E10"));
  EXPECT_EQ(Encode(123, 8, 0), FromString("1.23E+10"));
  EXPECT_EQ(Encode(123, 8, 1), FromString("-1.23E+10"));
  EXPECT_EQ(Encode(123, -12, 0), FromString("1.23E-10"));
  EXPECT_EQ(Encode(123, -12, 1), FromString("-1.23E-10"));
  EXPECT_EQ(Encode(123, -3, 0), FromString("0.123"));
  EXPECT_EQ(Encode(0, -2, 0), FromString("00.00"));
  EXPECT_EQ(Encode(1, 2, 0), FromString("1E2"));
  EXPECT_EQ(Dec64::Infinity(0), FromString("1E20000"));
  EXPECT_EQ(Dec64::Zero(0), FromString("1E-20000"));
  EXPECT_EQ(Encode(1000, 1023, 0), FromString("1E1026"));
  EXPECT_EQ(Dec64::Zero(0), FromString("1E-1026"));
  EXPECT_EQ(Dec64::Infinity(0), FromString("1234567890E1036"));
}

TEST_F(Decimal64ImplTest, FromStringSpecailValues) {
  EXPECT_EQ(Dec64::Infinity(0), FromString("Infinity"));
  EXPECT_EQ(Dec64::Infinity(0), FromString("Inf"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("NaN"));

  EXPECT_EQ(Dec64::Infinity(1), FromString("-Infinity"));
  EXPECT_EQ(Dec64::Infinity(1), FromString("-Inf"));
  EXPECT_EQ(Dec64::QuietNaN(1), FromString("-NaN"));
}

TEST_F(Decimal64ImplTest, FromStringTruncated) {
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("x"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("0."));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("1x"));

  EXPECT_EQ(Dec64::QuietNaN(0), FromString("1Ex"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("1E2x"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("1E+x"));

  EXPECT_EQ(Dec64::QuietNaN(0), FromString("+Ix"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("+Nx"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("+Sx"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("+x"));

  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Sx"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Nx"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Nax"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("NaNx"));

  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Ix"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Inx"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Infx"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Infix"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Infinx"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Infinix"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Infinitx"));
  EXPECT_EQ(Dec64::QuietNaN(0), FromString("Infinityx"));
}

TEST_F(Decimal64ImplTest, OpAdd) {
  EXPECT_EQ(Encode(0, 0, 0), Dec64(0) + Dec64(0));
  EXPECT_EQ(Encode(1, 0, 0), Dec64(2) + Dec64(-1));
  EXPECT_EQ(Encode(1, 0, 0), Dec64(-1) + Dec64(2));
  EXPECT_EQ(Encode(100, 0, 0), Dec64(99) + Dec64(1));
  EXPECT_EQ(Encode(100, 0, 1), Dec64(-50) + Dec64(-50));
  EXPECT_EQ(
      Encode(1000000000000000, 35, 0),
      Encode(1, 50, 0) + Encode(1, 0, 0));
  EXPECT_EQ(
      Encode(1000000000000000, 35, 0),
      Encode(1, 0, 0) + Encode(1, 50, 0));
  EXPECT_EQ(Encode(10000000001, 0, 0), Encode(1, 10, 0) + Encode(1, 0, 0));
  EXPECT_EQ(Encode(10000000001, 0, 0), Encode(1, 0, 0) + Encode(1, 10, 0));
}

TEST_F(Decimal64ImplTest, OpAddSpecialValues) {
  auto const inf = Dec64::Infinity(0);
  auto const minf = Dec64::Infinity(1);
  auto const nan = Dec64::QuietNaN(0);
  auto const mnan = Dec64::QuietNaN(1);
  Dec64 ten(10);

  EXPECT_EQ(inf, inf + inf);
  EXPECT_EQ(nan, inf + minf);
  EXPECT_EQ(nan, minf + inf);
  EXPECT_EQ(minf, minf + minf);

  EXPECT_EQ(inf, inf + ten);
  EXPECT_EQ(inf, ten + inf);
  EXPECT_EQ(minf, minf + ten);
  EXPECT_EQ(minf, ten + minf);

  EXPECT_EQ(nan, nan + nan);
  EXPECT_EQ(nan, nan + mnan);
  EXPECT_EQ(nan, nan + ten);

  EXPECT_EQ(mnan, mnan + nan);
  EXPECT_EQ(mnan, mnan + mnan);
  EXPECT_EQ(mnan, mnan + ten);

  EXPECT_EQ(nan, ten + nan);
  EXPECT_EQ(mnan, ten + mnan);
}

TEST_F(Decimal64ImplTest, OpCompare) {
  EXPECT_TRUE(Dec64(0) == Dec64(0));
  EXPECT_TRUE(Dec64(0) != Dec64(1));
  EXPECT_TRUE(Dec64(0) < Dec64(1));
  EXPECT_TRUE(Dec64(0) <= Dec64(0));
  EXPECT_TRUE(Dec64(0) > Dec64(-1));
  EXPECT_TRUE(Dec64(0) >= Dec64(0));

  EXPECT_FALSE(Dec64(1) == Dec64(2));
  EXPECT_FALSE(Dec64(1) != Dec64(1));
  EXPECT_FALSE(Dec64(1) < Dec64(0));
  EXPECT_FALSE(Dec64(1) <= Dec64(0));
  EXPECT_FALSE(Dec64(1) > Dec64(2));
  EXPECT_FALSE(Dec64(1) >= Dec64(2));
}

TEST_F(Decimal64ImplTest, OpCompareSpecialValues) {
  EXPECT_TRUE(Dec64(1) < Dec64::Infinity(0));
  EXPECT_TRUE(Dec64(1) > Dec64::Infinity(1));
}

TEST_F(Decimal64ImplTest, OpDiv) {
  EXPECT_EQ(Encode(0, 0, 0), Dec64(0) / Dec64(1));
  EXPECT_EQ(Encode(2, 0, 1), Dec64(2) / Dec64(-1));
  EXPECT_EQ(Encode(5, -1, 1), Dec64(-1) / Dec64(2));
  EXPECT_EQ(Encode(99, 0, 0), Dec64(99) / Dec64(1));
  EXPECT_EQ(Encode(1, 0, 0), Dec64(-50) / Dec64(-50));
  EXPECT_EQ(Encode(3333333333333333, -16, 0), Dec64(1) / Dec64(3));
  EXPECT_EQ(
      Encode(12345678901234, -1, 0),
      Encode(12345678901234, 0, 0) / Dec64(10));
}

TEST_F(Decimal64ImplTest, OpDivSpecialValues) {
  auto const inf = Dec64::Infinity(0);
  auto const minf = Dec64::Infinity(1);
  auto const nan = Dec64::QuietNaN(0);
  auto const mnan = Dec64::QuietNaN(1);
  auto const zero = Dec64::Zero(0);
  auto const mzero = Dec64::Zero(1);
  Dec64 ten(10);
  Dec64 mten(-10);

  EXPECT_EQ(nan, zero / zero);
  EXPECT_EQ(nan, zero / mzero);
  EXPECT_EQ(nan, mzero / zero);
  EXPECT_EQ(nan, mzero / mzero);

  EXPECT_EQ(inf, ten / zero);
  EXPECT_EQ(minf, ten / mzero);
  EXPECT_EQ(minf, mten / zero);
  EXPECT_EQ(inf, mten / mzero);

  EXPECT_EQ(inf, inf / zero);
  EXPECT_EQ(minf, inf / mzero);
  EXPECT_EQ(minf, minf / zero);
  EXPECT_EQ(inf, minf / mzero);

  EXPECT_EQ(nan, inf / inf);
  EXPECT_EQ(nan, inf / minf);
  EXPECT_EQ(nan, minf / inf);
  EXPECT_EQ(nan, minf / minf);

  EXPECT_EQ(zero, ten / inf);
  EXPECT_EQ(mzero, ten / minf);
  EXPECT_EQ(mzero, mten / inf);
  EXPECT_EQ(zero, mten / minf);

  EXPECT_EQ(nan, nan / nan);
  EXPECT_EQ(nan, nan / mnan);
  EXPECT_EQ(nan, nan / ten);

  EXPECT_EQ(mnan, mnan / nan);
  EXPECT_EQ(mnan, mnan / mnan);
  EXPECT_EQ(mnan, mnan / ten);

  EXPECT_EQ(nan, ten / nan);
  EXPECT_EQ(mnan, ten / mnan);
}

TEST_F(Decimal64ImplTest, OpMul) {
  EXPECT_EQ(Encode(0, 0, 0), Dec64(0) * Dec64(0));
  EXPECT_EQ(Encode(2, 0, 1), Dec64(2) * Dec64(-1));
  EXPECT_EQ(Encode(2, 0, 1), Dec64(-1) * Dec64(2));
  EXPECT_EQ(Encode(99, 0, 0), Dec64(99) * Dec64(1));
  EXPECT_EQ(Encode(2500, 0, 0), Dec64(-50) * Dec64(-50));
}

TEST_F(Decimal64ImplTest, OpMulSpecialValues) {
  auto const inf = Dec64::Infinity(0);
  auto const minf = Dec64::Infinity(1);
  auto const nan = Dec64::QuietNaN(0);
  auto const mnan = Dec64::QuietNaN(1);
  Dec64 ten(10);
  Dec64 mten(-10);
  auto const zero = Dec64::Zero(0);
  auto const mzero = Dec64::Zero(1);

  EXPECT_EQ(inf, inf * inf);
  EXPECT_EQ(minf, inf * minf);
  EXPECT_EQ(minf, minf * inf);
  EXPECT_EQ(inf, minf * minf);

  EXPECT_EQ(nan, inf * zero);
  EXPECT_EQ(nan, zero * minf);
  EXPECT_EQ(nan, minf * zero);
  EXPECT_EQ(nan, minf * zero);

  EXPECT_EQ(nan, inf * mzero);
  EXPECT_EQ(nan, mzero * minf);
  EXPECT_EQ(nan, minf * mzero);
  EXPECT_EQ(nan, minf * mzero);

  EXPECT_EQ(inf, inf * ten);
  EXPECT_EQ(inf, ten * inf);
  EXPECT_EQ(minf, minf * ten);
  EXPECT_EQ(minf, ten * minf);

  EXPECT_EQ(minf, inf * mten);
  EXPECT_EQ(minf, mten * inf);
  EXPECT_EQ(inf, minf * mten);
  EXPECT_EQ(inf, mten * minf);

  EXPECT_EQ(nan, nan * nan);
  EXPECT_EQ(nan, nan * mnan);
  EXPECT_EQ(nan, nan * ten);

  EXPECT_EQ(mnan, mnan * nan);
  EXPECT_EQ(mnan, mnan * mnan);
  EXPECT_EQ(mnan, mnan * ten);

  EXPECT_EQ(nan, ten * nan);
  EXPECT_EQ(mnan, ten * mnan);
}

TEST_F(Decimal64ImplTest, OpSub) {
  EXPECT_EQ(Encode(0, 0, 0), Dec64(0) - Dec64(0));
  EXPECT_EQ(Encode(3, 0, 0), Dec64(2) - Dec64(-1));
  EXPECT_EQ(Encode(3, 0, 1), Dec64(-1) - Dec64(2));
  EXPECT_EQ(Encode(98, 0, 0), Dec64(99) - Dec64(1));
  EXPECT_EQ(Encode(0, 0, 0), Dec64(-50) - Dec64(-50));
  EXPECT_EQ(
      Encode(1000000000000000, 35, 0),
      Encode(1, 50, 0) - Encode(1, 0, 0));
  EXPECT_EQ(
      Encode(1000000000000000, 35, 1),
      Encode(1, 0, 0) - Encode(1, 50, 0));
}

TEST_F(Decimal64ImplTest, OpSubSpecialValues) {
  auto const inf = Dec64::Infinity(0);
  auto const minf = Dec64::Infinity(1);
  auto const nan = Dec64::QuietNaN(0);
  auto const mnan = Dec64::QuietNaN(1);
  Dec64 ten(10);

  EXPECT_EQ(nan, inf - inf);
  EXPECT_EQ(inf, inf - minf);
  EXPECT_EQ(minf, minf - inf);
  EXPECT_EQ(nan, minf - minf);

  EXPECT_EQ(inf, inf - ten);
  EXPECT_EQ(minf, ten - inf);
  EXPECT_EQ(minf, minf - ten);
  EXPECT_EQ(inf, ten - minf);

  EXPECT_EQ(nan, nan - nan);
  EXPECT_EQ(nan, nan - mnan);
  EXPECT_EQ(nan, nan - ten);

  EXPECT_EQ(mnan, mnan - nan);
  EXPECT_EQ(mnan, mnan - mnan);
  EXPECT_EQ(mnan, mnan - ten);

  EXPECT_EQ(nan, ten - nan);
  EXPECT_EQ(mnan, ten - mnan);
}

TEST_F(Decimal64ImplTest, Properties) {
  EXPECT_TRUE(Dec64(0).value().is_finite());
  EXPECT_FALSE(Dec64(0).value().is_special());
}

TEST_F(Decimal64ImplTest, PropertiesSpecial) {
  EXPECT_TRUE(Dec64::Infinity(0).is_special());
  EXPECT_FALSE(Dec64::Infinity(0).is_finite());

  EXPECT_TRUE(Dec64::Infinity(1).is_special());
  EXPECT_FALSE(Dec64::Infinity(1).is_finite());

  EXPECT_TRUE(Dec64::QuietNaN(0).is_special());
  EXPECT_FALSE(Dec64::QuietNaN(0).is_finite());

  EXPECT_TRUE(Dec64::QuietNaN(1).is_special());
  EXPECT_FALSE(Dec64::QuietNaN(1).is_finite());
}

TEST_F(Decimal64ImplTest, Remainder) {
  EXPECT_EQ(Encode(21, -1, 0), Encode(21, -1, 0).Remainder(3));
  EXPECT_EQ(Encode(1, 0, 0), Dec64(10).Remainder(3));
  EXPECT_EQ(Encode(1, 0, 1), Dec64(-10).Remainder(3));
  EXPECT_EQ(Encode(2, -1, 0), Encode(102, -1, 0).Remainder(1));
  EXPECT_EQ(Encode(1, -1, 0), Dec64(10).Remainder(Encode(3, -1, 0)));
  EXPECT_EQ(
      Encode(10, -1, 0),
      Encode(36, -1, 0).Remainder(Encode(13, -1, 0)));
}

TEST_F(Decimal64ImplTest, RemainderSpecialValues) {
  EXPECT_EQ(Dec64::Infinity(0), Dec64::Infinity(0).Remainder(1));
  EXPECT_EQ(Dec64::QuietNaN(0), Dec64::QuietNaN(0).Remainder(1));
}

TEST_F(Decimal64ImplTest, ToFloat64) {
  EXPECT_EQ(0.0, float64(Encode(0, 0, 0)));
  EXPECT_EQ(-0.0, float64(Encode(0, 0, 1)));

  EXPECT_EQ(1.0, float64(Encode(1, 0, 0)));
  EXPECT_EQ(-1.0, float64(Encode(1, 0, 1)));

  EXPECT_EQ(0.1, float64(Dec64(0.1)));
  EXPECT_EQ(-0.1, float64(Dec64(-0.1)));
  EXPECT_EQ(0.6, float64(Dec64(0.6)));
  EXPECT_EQ(-0.6, float64(Dec64(-0.6)));
  EXPECT_EQ(0.7, float64(Dec64(0.7)));
  EXPECT_EQ(-0.7, float64(Dec64(-0.7)));

  // DBL_MAX_10_EXP(308)
  EXPECT_EQ(float64(Float64(0, 1024, 0)), float64(Encode(1, 400, 0)));

  // DBL_MIN_10_EXP(-307)
  EXPECT_EQ(0.0, float64(Encode(1, -400, 0)));
}

TEST_F(Decimal64ImplTest, ToFloat64SpecialValues) {
  EXPECT_EQ(float64(Float64(0, 1024, 0)), float64(Dec64::Infinity(0)));
  EXPECT_EQ(float64(Float64(0, 1024, 1)), float64(Dec64::Infinity(1)));

  EXPECT_TRUE(Float64(float64(Dec64::QuietNaN(0))).is_nan(0));
  EXPECT_TRUE(Float64(float64(Dec64::QuietNaN(1))).is_nan(1));
}

TEST_F(Decimal64ImplTest, ToString) {
  EXPECT_STREQ(L"0", Dec64(0).ToString().value());
  EXPECT_STREQ(L"1", Dec64(1).ToString().value());
  EXPECT_STREQ(L"-1", Dec64(-1).ToString().value());
  EXPECT_STREQ(L"0.001", Encode(1, -3, 0).ToString().value());
  EXPECT_STREQ(L"1E+10", Encode(1, 10, 0).ToString().value());
  EXPECT_STREQ(L"-1E+10", Encode(1, 10, 1).ToString().value());
  EXPECT_STREQ(L"1E-10", Encode(1, -10, 0).ToString().value());
  EXPECT_STREQ(L"-1E-10", Encode(1, -10, 1).ToString().value());
  EXPECT_STREQ(L"12.34", Encode(1234, -2, 0).ToString().value());
  EXPECT_STREQ(L"-12.34", Encode(1234, -2, 1).ToString().value());
  EXPECT_STREQ(L"3.456E+7", Encode(3456, 4, 0).ToString().value());
}

TEST_F(Decimal64ImplTest, ToStringSpecial) {
  EXPECT_STREQ(L"Infinity", Dec64::Infinity(0).ToString().value());
  EXPECT_STREQ(L"-Infinity", Dec64::Infinity(1).ToString().value());

  EXPECT_STREQ(L"NaN", Dec64::QuietNaN(0).ToString().value());
  EXPECT_STREQ(L"-NaN", Dec64::QuietNaN(1).ToString().value());
}

TEST_F(Decimal64ImplTest, Truncate) {
  EXPECT_EQ(25, Dec64(5).Truncate(Encode(200, -3, 0)));
  EXPECT_EQ(2, Dec64(5).Truncate(Dec64(2)));
  EXPECT_EQ(0, Dec64(2).Truncate(Dec64(3)));
  EXPECT_EQ(3, Dec64(10).Truncate(Dec64(3)));
  EXPECT_EQ(3, Dec64(1).Truncate(Encode(3, -1, 0)));
  EXPECT_EQ(10, Encode(1, 0, 0).Truncate(Encode(1, -1, 0)));
  EXPECT_EQ(Dec64::QuietNaN(0), Encode(1, 100, 0).Truncate(Dec64(1)));
}

} // CommonTest
