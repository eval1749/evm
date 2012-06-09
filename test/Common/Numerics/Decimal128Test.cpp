#include "precomp.h"
// @(#)$Id$
//
// Util Test for Decimal128
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractDecimal128Test.h"

namespace Common {
namespace Numerics {
::std::ostream& operator<<(::std::ostream& os, const Decimal128& d) {
  return os << d.ToString();
}
} // Numerics
} // Common

namespace CommonTest {

class Bin128 {
  private: uint32 words_[4];

  public: Bin128(uint32 a) {
    words_[3] = a;
    words_[2] = 0;
    words_[1] = 0;
    words_[0] = 0;
  }

  public: Bin128(uint32 a, uint32 b, uint32 c, uint d) {
    words_[3] = a;
    words_[2] = b;
    words_[1] = c;
    words_[0] = d;
  }

  public: Bin128(const Decimal128& d128) {
    ::CopyMemory(words_, d128.value().value().words_, sizeof(words_));
  }

  public: Bin128(const String& str) {
    auto d128 = Decimal128::FromString(str);
    ::CopyMemory(words_, d128.value().value().words_, sizeof(words_));
  }

  public: bool operator ==(const Bin128& rhs) const {
    return !::memcmp(words_, rhs.words_, sizeof(words_));
  }

  public: Decimal128 ToDecimal128() const {
    Decimal128 d128(0);
    ::CopyMemory(
      const_cast<uint32*>(d128.value().value().words_),
      words_,
      sizeof(words_));
    return d128;
  }

  public: String ToString() const {
    return String::Format("Bin128(0x%X, 0x%X, 0x%X, 0x%X)=%s",
        words_[3], words_[2], words_[1], words_[0],
        ToDecimal128());
  }
};

::std::ostream& operator<<(::std::ostream& os, const Bin128& d) {
  return os << d.ToString();
}

class Decimal128Test : public AbstractDecimal128Test {
  public: String ToString(uint32 a, uint32 b, uint32 c, uint32 d) {
    return Bin128(a, b, c, d).ToDecimal128().ToString();
  }
};

TEST_F(Decimal128Test, Compare) {
  EXPECT_TRUE(LessThan("2.1", "3"));
  EXPECT_TRUE(Equal("2.1", "2.1"));
  EXPECT_TRUE(Equal("2.1", "2.10"));
  EXPECT_TRUE(GreaterThan("3", "2.1"));
  EXPECT_TRUE(GreaterThan("2.1", "-3"));
  EXPECT_TRUE(LessThan("-3", "2.1"));
}

TEST_F(Decimal128Test, CompareSpecialValues) {
  const Decimal128 ninf(Decimal128::FromString("-Inf"));
  const Decimal128 pinf(Decimal128::FromString("+Inf"));

  const Decimal128 nnan(Decimal128::FromString("-NaN"));
  const Decimal128 pnan(Decimal128::FromString("+NaN"));

  const Decimal128 none(Decimal128::FromString("-1"));
  const Decimal128 pone(Decimal128::FromString("+1"));

  EXPECT_TRUE(ninf == ninf);
  EXPECT_FALSE(ninf != ninf);
  EXPECT_FALSE(ninf < ninf);
  EXPECT_TRUE(ninf <= ninf);
  EXPECT_FALSE(ninf > ninf);
  EXPECT_TRUE(ninf >= ninf);

  EXPECT_FALSE(ninf == none);
  EXPECT_TRUE(ninf != none);
  EXPECT_TRUE(ninf < none);
  EXPECT_TRUE(ninf <= none);
  EXPECT_FALSE(ninf > none);
  EXPECT_FALSE(ninf >= none);

  EXPECT_FALSE(ninf == nnan);
  EXPECT_FALSE(ninf != nnan);
  EXPECT_FALSE(ninf < nnan);
  EXPECT_FALSE(ninf <= nnan);
  EXPECT_FALSE(ninf > nnan);
  EXPECT_FALSE(ninf >= nnan);

  EXPECT_TRUE(pinf == pinf);
  EXPECT_FALSE(pinf != pinf);
  EXPECT_FALSE(pinf < pinf);
  EXPECT_TRUE(pinf <= pinf);
  EXPECT_FALSE(pinf > pinf);
  EXPECT_TRUE(pinf >= pinf);

  EXPECT_FALSE(pinf == pone);
  EXPECT_TRUE(pinf != pone);
  EXPECT_FALSE(pinf < pone);
  EXPECT_FALSE(pinf <= pone);
  EXPECT_TRUE(pinf > pone);
  EXPECT_TRUE(pinf >= pone);

  EXPECT_FALSE(pinf == nnan);
  EXPECT_FALSE(pinf != nnan);
  EXPECT_FALSE(pinf < nnan);
  EXPECT_FALSE(pinf <= nnan);
  EXPECT_FALSE(pinf > nnan);
  EXPECT_FALSE(pinf >= nnan);

  EXPECT_FALSE(nnan == nnan);
  EXPECT_FALSE(nnan != nnan);
  EXPECT_FALSE(nnan < nnan);
  EXPECT_FALSE(nnan <= nnan);
  EXPECT_FALSE(nnan > nnan);
  EXPECT_FALSE(nnan >= nnan);

  EXPECT_FALSE(nnan == none);
  EXPECT_FALSE(nnan != none);
  EXPECT_FALSE(nnan < none);
  EXPECT_FALSE(nnan <= none);
  EXPECT_FALSE(nnan > none);
  EXPECT_FALSE(nnan >= none);

  EXPECT_FALSE(pnan == pnan);
  EXPECT_FALSE(pnan != pnan);
  EXPECT_FALSE(pnan < pnan);
  EXPECT_FALSE(pnan <= pnan);
  EXPECT_FALSE(pnan > pnan);
  EXPECT_FALSE(pnan >= pnan);

  EXPECT_FALSE(pnan == pone);
  EXPECT_FALSE(pnan != pone);
  EXPECT_FALSE(pnan < pone);
  EXPECT_FALSE(pnan <= pone);
  EXPECT_FALSE(pnan > pone);
  EXPECT_FALSE(pnan >= pone);

  EXPECT_TRUE(Equal("+0", "-0"));
}

TEST_F(Decimal128Test, CtroInt32) {
  EXPECT_STREQ(L"0", Decimal128(0).ToString().value());
  EXPECT_STREQ(L"12345", Decimal128(12345).ToString().value());
  EXPECT_STREQ(L"-12345", Decimal128(-12345).ToString().value());
  EXPECT_STREQ(
      L"2147483647", 
      Decimal128(0x7FFFFFFF).ToString().value());
  EXPECT_STREQ(
      L"-2147483648", 
      Decimal128(-1 << 31).ToString().value());
  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x50), Bin128(Decimal128(50)));
}

TEST_F(Decimal128Test, CtroInt64) {
  EXPECT_STREQ(
      L"1125899906842624", 
      Decimal128(int64(1) << 50).ToString().value());
  EXPECT_STREQ(
      L"9223372036854775807", 
      Decimal128(0x7FFFFFFFFFFFFFFF).ToString().value());
  EXPECT_STREQ(
      L"-9223372036854775808", 
      Decimal128(int64(-1) << 63).ToString().value());
}

TEST_F(Decimal128Test, CtroFloat64) {
  EXPECT_STREQ(L"0", Float64(0.0).value());
  EXPECT_STREQ(L"1", Float64(1.0).value());
  EXPECT_STREQ(L"5", Float64(5.0).value());
  EXPECT_STREQ(L"-6", Float64(-6.0).value());
  EXPECT_STREQ(L"1.23", Float64(1.23).value());
  EXPECT_STREQ(L"-567.89", Float64(-567.89).value());
  EXPECT_STREQ(L"0.1", Float64(0.1).value());
  EXPECT_EQ(Bin128(0x2207C000, 0, 0, 0x1), Bin128(Decimal128(0.1)));
  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x50), Bin128(Decimal128(50.0)));

  EXPECT_STREQ(L"-0", Float64(MakeFloat64(1, 0, 0)).value());
  EXPECT_STREQ(L"Infinity", Float64(MakeFloat64(0, 0, 2047)).value());
  EXPECT_STREQ(L"NaN", Float64(MakeFloat64(0, uint64(1) << 51, 2047)).value());

  Float64UInt64 snan64;
  snan64.u64_ = (uint64(0x7FF00000) << 32) | 1;
  EXPECT_STREQ(L"sNaN", Float64(snan64.f64_).value());

  Float64UInt64 e64;
  e64.u64_ = 0x4005bf0a8b145769;
  // 2.7182818284590452353602874713526624977572470936999595
  EXPECT_STREQ(L"2.718281828459045", Float64(e64.f64_).value());

  Float64UInt64 pi64;
  pi64.u64_ = 0x400921FB54442D18u;
  // 3.1415926535897932384626433832795028841971693993751058
  EXPECT_STREQ(L"3.141592653589793", Float64(pi64.f64_).value());

  // least-positive-normalized-double-float
  EXPECT_STREQ(
      L"2.225073858507201E-308",
      Float64(MakeFloat64(0, 0, 1)).value());

  // most-positive-double-float
  EXPECT_STREQ(
      L"1.797693134862316E+308",
      Float64(MakeFloat64(0, (uint64(1) << 52) - 1, 2046)).value());

  //  double-float-epsilon
  EXPECT_STREQ(
      L"1.110223024625157E-16",
      Float64(MakeFloat64(0, 1, 970)).value());

  // Subnormal
  // least-positive-double-float(5E-324)
  // MSVS2010 4.940656458412e-324#DEN
  EXPECT_STREQ(
      L"4.940656458412465E-324",
      Float64(MakeFloat64(0, 1, 0)).value());
}

// Note: All comparison must be done in binary format. Decimal128 operator ==
// checkes numeric equality.
TEST_F(Decimal128Test, FromString) {
  EXPECT_EQ(Bin128(0x22034000, 0, 0, 0), Bin128("0E-19"));
  EXPECT_EQ(Bin128(0x2208C000, 0, 0, 0), Bin128("0E+3"));
  EXPECT_EQ(Bin128(0x22070000, 0, 0, 0x1400), Bin128("0.5000"));
  EXPECT_EQ(Bin128(0xA2078000, 0, 0, 0x3D0), Bin128("-7.50"));
  EXPECT_EQ(Bin128(0xA2080000, 0, 0, 0x3D0), Bin128("-750"));

  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x00), Bin128("0"));
  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x05), Bin128("5"));
  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x10), Bin128("10"));
  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x77), Bin128("77"));

  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x2A0), Bin128("520"));
  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x06E), Bin128("888"));
  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x29E), Bin128("994"));
  EXPECT_EQ(Bin128(0x22080000, 0, 0, 0x0FF), Bin128("999"));

  EXPECT_EQ(Bin128(0xA2084000, 0, 0, 5), Bin128("-5E1"));

  // Near Emax
  EXPECT_EQ(Bin128(0x47FFC000, 0, 0, 0), Bin128("1E6144"));
  EXPECT_EQ(
    Bin128(0x77FFCFF3, 0xFCFF3FCF, 0xF3FCFF3F, 0xCFF3FCFF),
    Bin128("9.999999999999999999999999999999999E+6144"));

  EXPECT_EQ(Bin128(0x47FFC000, 0, 0, 0x00), Bin128("1E6144"));
  EXPECT_EQ(Bin128(0x43FFC800, 0, 0, 0x00), Bin128("1E6143"));
  EXPECT_EQ(Bin128(0x43FFC000, 0, 0, 0x80), Bin128("1E6113"));
  EXPECT_EQ(Bin128(0x43FFC000, 0, 0, 0x10), Bin128("1E6112"));
  EXPECT_EQ(Bin128(0x43FFC000, 0, 0, 0x01), Bin128("1E6111"));

  // Near Emin
  EXPECT_EQ(Bin128(0x00084000, 0, 0, 1), Bin128("1E-6143"));
  EXPECT_EQ(
    Bin128(0x04000000, 0, 0, 0),
    Bin128("1.000000000000000000000000000000000E-6143"));
  EXPECT_EQ(
    Bin128(0x00000800, 0, 0, 0),
    Bin128("0.100000000000000000000000000000000E-6143"));

  // Normal
  EXPECT_EQ(Bin128(0x220B0000, 0, 0, 5), Bin128("5E12"));
  EXPECT_EQ(Bin128(0x220B0000, 0, 0, 5), Bin128("5E+12"));
  EXPECT_EQ(
    Bin128(0x2608134B, 0x9C1E28E5, 0x6F3C1271, 0x77823534),
    Bin128("1234567890123456789012345678901234"));
  EXPECT_EQ(
    Bin128(0xA608134B, 0x9C1E28E5, 0x6F3C1271, 0x77823534),
    Bin128("-1234567890123456789012345678901234"));

  EXPECT_EQ(
    Bin128(0x26080912, 0x44912449, 0x12449124, 0x49124491),
    Bin128("1111111111111111111111111111111111"));

  // 2^1024
  // 1.7976931348623157d308
  // 1.797693134862315907729305190789025E+308
  EXPECT_EQ(
    Bin128(0x264CFFBC, 0xECB45B19, 0x5E37A961, 0x49AF3C25),
    Bin128("179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216"));
}

TEST_F(Decimal128Test, FromStringSubnormal) {
  EXPECT_EQ(Bin128(0, 0, 0, 1), Bin128("1e-6176"));
  EXPECT_EQ(
    Bin128(0x00000FF3, 0xFCFF3FCF, 0xF3FCFF3F, 0xCFF3FCFF),
    Bin128("999999999999999999999999999999999e-6176"));
}

TEST_F(Decimal128Test, FromStringSpecialvalues) {
  EXPECT_EQ(Bin128(0x78000000), Bin128("Infinity"));
  EXPECT_EQ(Bin128(0x78000000), Bin128("Inf"));
  EXPECT_EQ(Bin128(0x7C000000), Bin128("NaN"));
  EXPECT_EQ(Bin128(0x7E000000), Bin128("sNaN"));

  EXPECT_EQ(Bin128(0xF8000000), Bin128("-Infinity"));
  EXPECT_EQ(Bin128(0xF8000000), Bin128("-Inf"));
  EXPECT_EQ(Bin128(0xFC000000), Bin128("-NaN"));
  EXPECT_EQ(Bin128(0xFE000000), Bin128("-sNaN"));
}

TEST_F(Decimal128Test, FromStringError) {
  EXPECT_STREQ(L"NaN", FromString("x").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("0.").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("1x").ToString().value());

  EXPECT_STREQ(L"NaN", FromString("1Ex").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("1E2x").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("1E+x").ToString().value());

  EXPECT_STREQ(L"NaN", FromString("+Ix").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("+Nx").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("+Sx").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("+x").ToString().value());

  EXPECT_STREQ(L"NaN", FromString("Sx").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("Nx").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("Nax").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("NaNx").ToString().value());

  EXPECT_STREQ(L"NaN", FromString("Ix").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("Inx").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("Infx").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("Infix").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("Infinx").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("Infinix").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("Infinitx").ToString().value());
  EXPECT_STREQ(L"NaN", FromString("Infinityx").ToString().value());
}

TEST_F(Decimal128Test, OpAdd) {
  Decimal128 one(1);
  Decimal128 two(2);
  EXPECT_EQ(Decimal128(2), one + one);
  EXPECT_EQ(Decimal128(0), one - one);
  EXPECT_EQ(Decimal128(3), one + two);
  EXPECT_EQ(Decimal128(-1), one - two);

  EXPECT_STREQ(L"-0.0", Add("-0.0", "-0.0").value());
  EXPECT_STREQ(L"0.0", Add("-0.0", "+0.0").value());
  EXPECT_STREQ(L"0.0", Add("+0.0", "-0.0").value());
  EXPECT_STREQ(L"0.0", Add("+0.0", "+0.0").value());

  EXPECT_STREQ(L"1.01E+4", Add("1E+2", "1E+4").value());
  EXPECT_STREQ(L"2", Add("5", "-3").value());
  EXPECT_STREQ(L"19.00", Add("12", "7.00").value());

  EXPECT_STREQ(L"9.05", Add("5.75", "3.3").value());
  EXPECT_EQ(String("-8"), Add("-5", "-3"));
  EXPECT_EQ(String("1.0"), Add("0.7", "0.3"));
  EXPECT_EQ(String("2.50"), Add("1.25", "1.25"));
  EXPECT_EQ(String("2.23456789"), Add("1.23456789", "1.00000000"));
}

TEST_F(Decimal128Test, OpAddSpecialValues) {
  auto const inf = Infinity();
  auto const minf = Infinity(1);
  auto const nan = QuietNaN();
  auto const mnan = QuietNaN(1);
  Decimal128 ten(10);

  EXPECT_STREQ(L"Infinity", Add(inf, inf).value());
  EXPECT_STREQ(L"NaN", Add(inf, minf).value());
  EXPECT_STREQ(L"NaN", Add(minf, inf).value());
  EXPECT_STREQ(L"-Infinity", Add(minf, minf).value());

  EXPECT_STREQ(L"Infinity", Add(inf, ten).value());
  EXPECT_STREQ(L"Infinity", Add(ten, inf).value());
  EXPECT_STREQ(L"-Infinity", Add(minf, ten).value());
  EXPECT_STREQ(L"-Infinity", Add(ten, minf).value());

  EXPECT_STREQ(L"NaN", Add(nan, nan).value());
  EXPECT_STREQ(L"NaN", Add(nan, mnan).value());
  EXPECT_STREQ(L"NaN", Add(nan, ten).value());

  EXPECT_STREQ(L"-NaN", Add(mnan, nan).value());
  EXPECT_STREQ(L"-NaN", Add(mnan, mnan).value());
  EXPECT_STREQ(L"-NaN", Add(mnan, ten).value());

  EXPECT_STREQ(L"NaN", Add(ten, nan).value());
  EXPECT_STREQ(L"-NaN", Add(ten, mnan).value());
}

TEST_F(Decimal128Test, OpDiv) {
  EXPECT_STREQ(L"0", Div("0", "1").value());
  EXPECT_STREQ(L"0.00", Div("0.00", "1").value());
  EXPECT_STREQ(L"1000", Div("1000", "1").value());
  EXPECT_STREQ(L"1", Div("12", "12").value());
  EXPECT_STREQ(L"4.00", Div("8.00", "2").value());
  EXPECT_STREQ(L"1.20", Div("2.400", "2.0").value());
  EXPECT_STREQ(L"10", Div("1000", "100").value());
  EXPECT_STREQ(L"2.5", Div("5", "2").value());
  EXPECT_STREQ(L"0.1", Div("1", "10").value());
  EXPECT_STREQ(L"1.20E+6", Div("2.40E+6", "2").value());
  EXPECT_STREQ(
    L"0.6666666666666666666666666666666667",
    Div("2", "3").value());
  EXPECT_STREQ(
    L"0.3333333333333333333333333333333333",
    Div("1", "3").value());
}

TEST_F(Decimal128Test, OpDivSpecialValues) {
  auto const inf = Infinity();
  auto const minf = Infinity(1);
  auto const nan = QuietNaN();
  auto const mnan = QuietNaN(1);
  auto const zero = Zero();
  auto const mzero = Zero(1);
  Decimal128 ten(10);
  Decimal128 mten(-10);

  EXPECT_STREQ(L"NaN", Div(zero, zero).value());
  EXPECT_STREQ(L"NaN", Div(zero, mzero).value());
  EXPECT_STREQ(L"NaN", Div(mzero, zero).value());
  EXPECT_STREQ(L"NaN", Div(mzero, mzero).value());

  EXPECT_STREQ(L"Infinity", Div(ten, zero).value());
  EXPECT_STREQ(L"-Infinity", Div(ten, mzero).value());
  EXPECT_STREQ(L"-Infinity", Div(mten, zero).value());
  EXPECT_STREQ(L"Infinity", Div(mten, mzero).value());

  EXPECT_STREQ(L"Infinity", Div(inf, zero).value());
  EXPECT_STREQ(L"-Infinity", Div(inf, mzero).value());
  EXPECT_STREQ(L"-Infinity", Div(minf, zero).value());
  EXPECT_STREQ(L"Infinity", Div(minf, mzero).value());

  EXPECT_STREQ(L"NaN", Div(inf, inf).value());
  EXPECT_STREQ(L"NaN", Div(inf, minf).value());
  EXPECT_STREQ(L"NaN", Div(minf, inf).value());
  EXPECT_STREQ(L"NaN", Div(minf, minf).value());

  EXPECT_STREQ(L"0", Div(ten, inf).value());
  EXPECT_STREQ(L"-0", Div(ten, minf).value());
  EXPECT_STREQ(L"-0", Div(mten, inf).value());
  EXPECT_STREQ(L"0", Div(mten, minf).value());

  EXPECT_STREQ(L"NaN", Div(nan, nan).value());
  EXPECT_STREQ(L"NaN", Div(nan, mnan).value());
  EXPECT_STREQ(L"NaN", Div(nan, ten).value());

  EXPECT_STREQ(L"-NaN", Div(mnan, nan).value());
  EXPECT_STREQ(L"-NaN", Div(mnan, mnan).value());
  EXPECT_STREQ(L"-NaN", Div(mnan, ten).value());

  EXPECT_STREQ(L"NaN", Div(ten, nan).value());
  EXPECT_STREQ(L"-NaN", Div(ten, mnan).value());
}

TEST_F(Decimal128Test, OpMul) {
  EXPECT_STREQ(L"428135971041", Mul("654321", "654321").value());
  EXPECT_STREQ(L"3.60", Mul("1.20", "3").value());
  EXPECT_STREQ(L"21", Mul("7", "3").value());
  EXPECT_STREQ(L"0.72", Mul("0.9", "0.8").value());
  EXPECT_STREQ(L"0.6", Mul("6", "0.1").value());

  EXPECT_STREQ(
    // 1524157875323883675049535156256666792303015211342784374345526722756
    L"1.524157875323883675049535156256667E+66",
    Mul("1234567890123456789012345678901234",
        "1234567890123456789012345678901234").value());
}

TEST_F(Decimal128Test, OpMulSpecialValues) {
  auto const inf = Infinity();
  auto const minf = Infinity(1);
  auto const nan = QuietNaN();
  auto const mnan = QuietNaN(1);
  Decimal128 ten(10);
  Decimal128 mten(-10);
  auto const zero = Zero();
  auto const mzero = Zero(1);

  EXPECT_STREQ(L"Infinity", Mul(inf, inf).value());
  EXPECT_STREQ(L"-Infinity", Mul(inf, minf).value());
  EXPECT_STREQ(L"-Infinity", Mul(minf, inf).value());
  EXPECT_STREQ(L"Infinity", Mul(minf, minf).value());

  EXPECT_STREQ(L"NaN", Mul(inf, zero).value());
  EXPECT_STREQ(L"NaN", Mul(zero, minf).value());
  EXPECT_STREQ(L"NaN", Mul(minf, zero).value());
  EXPECT_STREQ(L"NaN", Mul(minf, zero).value());

  EXPECT_STREQ(L"NaN", Mul(inf, mzero).value());
  EXPECT_STREQ(L"NaN", Mul(mzero, minf).value());
  EXPECT_STREQ(L"NaN", Mul(minf, mzero).value());
  EXPECT_STREQ(L"NaN", Mul(minf, mzero).value());

  EXPECT_STREQ(L"Infinity", Mul(inf, ten).value());
  EXPECT_STREQ(L"Infinity", Mul(ten, inf).value());
  EXPECT_STREQ(L"-Infinity", Mul(minf, ten).value());
  EXPECT_STREQ(L"-Infinity", Mul(ten, minf).value());

  EXPECT_STREQ(L"-Infinity", Mul(inf, mten).value());
  EXPECT_STREQ(L"-Infinity", Mul(mten, inf).value());
  EXPECT_STREQ(L"Infinity", Mul(minf, mten).value());
  EXPECT_STREQ(L"Infinity", Mul(mten, minf).value());

  EXPECT_STREQ(L"NaN", Mul(nan, nan).value());
  EXPECT_STREQ(L"NaN", Mul(nan, mnan).value());
  EXPECT_STREQ(L"NaN", Mul(nan, ten).value());

  EXPECT_STREQ(L"-NaN", Mul(mnan, nan).value());
  EXPECT_STREQ(L"-NaN", Mul(mnan, mnan).value());
  EXPECT_STREQ(L"-NaN", Mul(mnan, ten).value());

  EXPECT_STREQ(L"NaN", Mul(ten, nan).value());
  EXPECT_STREQ(L"-NaN", Mul(ten, mnan).value());
}

TEST_F(Decimal128Test, OpSub) {
  EXPECT_STREQ(L"-0.0", Sub("-0.0", "0.0").value());
  EXPECT_STREQ(L"0.0", Sub("0.0", "0.0").value());
  EXPECT_STREQ(L"0.23", Sub("1.3", "1.07").value());
  EXPECT_STREQ(L"0.00", Sub("1.3", "1.30").value());
  EXPECT_STREQ(L"-0.77", Sub("1.3", "2.07").value());
}

TEST_F(Decimal128Test, OpSubSpecialValues) {
  auto const inf = Infinity();
  auto const minf = Infinity(1);
  auto const nan = QuietNaN();
  auto const mnan = QuietNaN(1);
  Decimal128 ten(10);

  EXPECT_STREQ(L"NaN", Sub(inf, inf).value());
  EXPECT_STREQ(L"Infinity", Sub(inf, minf).value());
  EXPECT_STREQ(L"-Infinity", Sub(minf, inf).value());
  EXPECT_STREQ(L"NaN", Sub(minf, minf).value());

  EXPECT_STREQ(L"Infinity", Sub(inf, ten).value());
  EXPECT_STREQ(L"-Infinity", Sub(ten, inf).value());
  EXPECT_STREQ(L"-Infinity", Sub(minf, ten).value());
  EXPECT_STREQ(L"Infinity", Sub(ten, minf).value());

  EXPECT_STREQ(L"NaN", Sub(nan, nan).value());
  EXPECT_STREQ(L"NaN", Sub(nan, mnan).value());
  EXPECT_STREQ(L"NaN", Sub(nan, ten).value());

  EXPECT_STREQ(L"-NaN", Sub(mnan, nan).value());
  EXPECT_STREQ(L"-NaN", Sub(mnan, mnan).value());
  EXPECT_STREQ(L"-NaN", Sub(mnan, ten).value());

  EXPECT_STREQ(L"NaN", Sub(ten, nan).value());
  EXPECT_STREQ(L"-NaN", Sub(ten, mnan).value());
}

TEST_F(Decimal128Test, Remainder) {
  EXPECT_STREQ(L"2.1", Remainder("2.1", "3").value());
  EXPECT_STREQ(L"1", Remainder("10", "3").value());
  EXPECT_STREQ(L"-1", Remainder("-10", "3").value());
  EXPECT_STREQ(L"0.2", Remainder("10.2", "1").value());
  EXPECT_STREQ(L"0.1", Remainder("10", "0.3").value());
  EXPECT_STREQ(L"1.0", Remainder("3.6", "1.3").value());
}

TEST_F(Decimal128Test, ToFloat64) {
  EXPECT_EQ(0.0, float64(Zero()));
  EXPECT_EQ(-0.0, float64(Zero(1)));

  EXPECT_EQ(1.0, float64(Decimal128(1)));
  EXPECT_EQ(-1.0, float64(Decimal128(-1)));

  EXPECT_EQ(123.45, float64(Decimal128(123.45)));
  EXPECT_EQ(-123.45, float64(Decimal128(-123.45)));

  Float64UInt64 inf64;
  inf64.u64_ = uint64(0x7FF0) << 48;

  EXPECT_EQ(inf64.f64_, float64(Infinity()));
  EXPECT_TRUE(IsFloat64QuietNaN(float64(QuietNaN()), 0));

  EXPECT_EQ(-inf64.f64_, float64(Infinity(1)));
  EXPECT_TRUE(IsFloat64QuietNaN(float64(QuietNaN(1)), 1));
}

TEST_F(Decimal128Test, ToString) {
  // Normal
  EXPECT_STREQ(L"0", Decimal128(0).ToString().value());
  EXPECT_STREQ(L"-0", ToString(0xA2080000, 0, 0, 0).value());

  EXPECT_STREQ(L"1", Decimal128(1).ToString().value());
  EXPECT_STREQ(L"-1", Decimal128(-1).ToString().value());
  EXPECT_STREQ(L"123", Decimal128(123).ToString().value());
  EXPECT_STREQ(L"-123", Decimal128(-123).ToString().value());

  EXPECT_STREQ(L"1.5", Decimal128(1.5).ToString().value());
  EXPECT_STREQ(L"-1.5", Decimal128(-1.5).ToString().value());

  EXPECT_STREQ(L"-50", Decimal128(-50).ToString().value());

  EXPECT_STREQ(L"0.5000", ToString(0x22070000, 0, 0, 0x1400).value());
  EXPECT_STREQ(L"0E-19", ToString(0x22034000, 0, 0, 0).value());
}

TEST_F(Decimal128Test, ToStringLarge) {
  EXPECT_STREQ(
    L"1.000000000000000000000000000000000E+6144",
    ToString(0x47FFC000, 0, 0, 0).value());
  EXPECT_STREQ(
    L"1.00000000000000000000000000000000E+6143",
    ToString(0x43FFC800, 0, 0, 0).value());
  EXPECT_STREQ(L"1.00000000000E+6122", ToString(0x43FFC000, 0,  0x20, 0).value());
  EXPECT_STREQ(L"1.00E+6113", ToString(0x43FFC000, 0, 0, 0x80).value());
  EXPECT_STREQ(L"1.0E+6112", ToString(0x43FFC000, 0, 0, 0x10).value());
  EXPECT_STREQ(L"1E+6111", ToString(0x43FFC000, 0, 0, 1).value());
}

TEST_F(Decimal128Test, ToStringSpecial) {
  EXPECT_STREQ(L"Infinity", Infinity(0).ToString().value());
  EXPECT_STREQ(L"-Infinity", Infinity(1).ToString().value());
  EXPECT_STREQ(L"NaN", QuietNaN(0).ToString().value());
  EXPECT_STREQ(L"-NaN", QuietNaN(1).ToString().value());
  EXPECT_STREQ(L"sNaN", SignalingNaN(0).ToString().value());
  EXPECT_STREQ(L"-sNaN", SignalingNaN(1).ToString().value());
}

TEST_F(Decimal128Test, ToStringSubnormal) {
  EXPECT_STREQ(L"1.0E-6175", ToString(0, 0, 0, 0x10).value());
  EXPECT_STREQ(L"1E-6175", ToString(0x00004000, 0, 0, 1).value());
  EXPECT_STREQ(L"1E-6176", ToString(0, 0, 0, 1).value());
  EXPECT_STREQ(
    L"1.00000000000000000000000000000000E-6144",
    ToString(0x00000800, 0, 0, 0).value());
  EXPECT_STREQ(
    L"9.99999999999999999999999999999999E-6144",
    // exp=0, significand=9{33}
    ToString(0x00000FF3, 0xFCFF3FCF, 0xF3FCFF3F, 0xCFF3FCFF).value());
}

TEST_F(Decimal128Test, Truncate) {
  EXPECT_STREQ(L"25", Truncate("5", "0.200").value());
  EXPECT_STREQ(L"2", Truncate("5", "2").value());
  EXPECT_STREQ(L"0", Truncate("2", "3").value());
  EXPECT_STREQ(L"3", Truncate("10", "3").value());
  EXPECT_STREQ(L"3", Truncate("1", "0.3").value());
  EXPECT_STREQ(L"10", Truncate("1", "0.1").value());
  EXPECT_STREQ(L"5000", DivInt("5.00", "1E-3").value());
}

TEST_F(Decimal128Test, Questionable) {
  EXPECT_STRNE(L"4444444444444444444444444444444444", Add("4444444444444444444444444444444444", "0.49").value()) << "dqadd014";
  EXPECT_STRNE(L"4444444444444444444444444444444444", Add("4444444444444444444444444444444444", "0.499").value()) << "dqadd015";
  EXPECT_STRNE(L"4444444444444444444444444444444444", Add("4444444444444444444444444444444444", "0.4999").value()) << "dqadd016";
  EXPECT_STRNE(L"4444444444444444444444444444444444", Add("4444444444444444444444444444444444", "0.5000").value()) << "dqadd017";
  EXPECT_STRNE(L"1231234567890123456784560123456789", Add("1231234567890123456784560123456789", "0.49").value()) << "dqadd225";
  EXPECT_STRNE(L"1231234567890123456784560123456789", Add("1231234567890123456784560123456789", "0.499999").value()) << "dqadd226";
  EXPECT_STRNE(L"1231234567890123456784560123456789", Add("1231234567890123456784560123456789", "0.499999999").value()) << "dqadd227";
  EXPECT_STRNE(L"1231234567890123456784560123456788", Add("1231234567890123456784560123456788", "0.499999999").value()) << "dqadd240";
  EXPECT_STRNE(L"1231234567890123456784560123456788", Add("1231234567890123456784560123456788", "0.5").value()) << "dqadd241";
  EXPECT_STRNE(L"9876543219876543216543210123456789", Add("9876543219876543216543210123456789", "0.49").value()) << "dqadd6565";
  EXPECT_STRNE(L"9876543219876543216543210123456789", Add("9876543219876543216543210123456789", "0.499999").value()) << "dqadd6566";
  EXPECT_STRNE(L"9876543219876543216543210123456789", Add("9876543219876543216543210123456789", "0.499999999").value()) << "dqadd6567";
  EXPECT_STRNE(L"9876543219876543216543210123456788", Add("9876543219876543216543210123456788", "0.499999999").value()) << "dqadd7540";
  EXPECT_STRNE(L"9876543219876543216543210123456788", Add("9876543219876543216543210123456788", "0.5").value()) << "dqadd7541";
  EXPECT_STRNE(L"1.000000000000000000000000000000000E+34", Add("1E34", "-0.5").value()) << "dqadd71300";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.49").value()) << "dqadd71350";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.499").value()) << "dqadd71351";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.4999").value()) << "dqadd71352";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.49999").value()) << "dqadd71353";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.499999").value()) << "dqadd71354";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.4999999").value()) << "dqadd71355";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.49999999").value()) << "dqadd71356";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.499999999").value()) << "dqadd71357";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.4999999999").value()) << "dqadd71358";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.49999999999").value()) << "dqadd71359";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.499999999999").value()) << "dqadd71360";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.4999999999999").value()) << "dqadd71361";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.49999999999999").value()) << "dqadd71362";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.499999999999999").value()) << "dqadd71363";
  EXPECT_STRNE(L"9999999999999999999999999999999999", Add("9999999999999999999999999999999999", "0.4999999999999999").value()) << "dqadd71364";
  EXPECT_STRNE(L"12398765432112345678945678.12345678", Add("12398765432112345678945678", "0.123456785").value()) << "dqadd75040";
  EXPECT_STRNE(L"12398765432112345678945678.12345678", Add("12398765432112345678945678", "0.1234567850").value()) << "dqadd75041";
  EXPECT_STRNE(L"12398765432112345678945678.12345678", Add("12398765432112345678945678", "0.1234567850000000").value()) << "dqadd75049";
  EXPECT_STRNE(L"12398765432112345678945678.00000000", Add("12398765432112345678945678", "0.0000000050000000").value()) << "dqadd75057";
  EXPECT_STRNE(L"2E-6176", Div("2.5E-6172", "1e+4").value()) << "dqdiv1810";
  EXPECT_STRNE(L"2E-6176", Div("1.50E-6172", "1e+4").value()) << "dqdiv1813";
  EXPECT_STRNE(L"0E-6176", Div("5E-6172", "1e+5").value()) << "dqdiv1820";
  EXPECT_STRNE(L"-0E-6176", Div("-5E-6172", "1e+5").value()) << "dqdiv1826";
  EXPECT_STRNE(L"2E-6176", Mul("2.5E-6172", "1e-4").value()) << "dqmul810";
  EXPECT_STRNE(L"2E-6176", Mul("1.50E-6172", "1e-4").value()) << "dqmul813";
  EXPECT_STRNE(L"0E-6176", Mul("5E-6172", "1e-5").value()) << "dqmul820";
  EXPECT_STRNE(L"-0E-6176", Mul("-5E-6172", "1e-5").value()) << "dqmul826";
  EXPECT_STRNE(L"0.09999999999999999999999999999999999", Mul("1", "0.09999999999999999999999999999999999").value()) << "dqmul907";
  EXPECT_STRNE(L"1.000000000000000000000000000000000E-6143", Mul("9.999999999999999999999999999999999E-6143", "0.09999999999999999999999999999999999").value()) << "dqmul908";
  EXPECT_STREQ(L"-9.999999999999999999999999999999999E+37", Sub("7000", "10000e+34").value()) << "dqsub062";
  EXPECT_STREQ(L"-9.999999999999999999999999999999993E+37", Sub("70000", "10000e+34").value()) << "dqsub063";
  EXPECT_STREQ(L"-9.999999999999999999999999999999930E+37", Sub("700000", "10000e+34").value()) << "dqsub064";
  EXPECT_STREQ(L"9.999999999999999999999999999999999E+37", Sub("10000e+34", "7000").value()) << "dqsub067";
  EXPECT_STREQ(L"9.999999999999999999999999999999993E+37", Sub("10000e+34", "70000").value()) << "dqsub068";
  EXPECT_STREQ(L"9.999999999999999999999999999999930E+37", Sub("10000e+34", "700000").value()) << "dqsub069";
  EXPECT_STREQ(L"0.9999999999999999999999999999999999", Sub("1", "0.0000000000000000000000000000000001").value()) << "dqsub364";
  EXPECT_STREQ(L"300.0000000000000000000000000000000", Sub("333.0000000000000000000000000123456", "33.00000000000000000000000001234565").value()) << "dqsub921";
  EXPECT_STREQ(L"99.99999999999999999999999999999995", Sub("133.0000000000000000000000000123456", "33.00000000000000000000000001234565").value()) << "dqsub922";
  EXPECT_STREQ(L"99.99999999999999999999999999999996", Sub("133.0000000000000000000000000123456", "33.00000000000000000000000001234564").value()) << "dqsub923";
  EXPECT_STREQ(L"90.00000000000000000000000000000000", Sub("133.0000000000000000000000000123456", "43.00000000000000000000000001234560").value()) << "dqsub925";
  EXPECT_STREQ(L"89.99999999999999999999999999999999", Sub("133.0000000000000000000000000123456", "43.00000000000000000000000001234561").value()) << "dqsub926";
  EXPECT_STREQ(L"89.99999999999999999999999999999994", Sub("133.0000000000000000000000000123456", "43.00000000000000000000000001234566").value()) << "dqsub927";
  EXPECT_STREQ(L"9.99999999999999999999999999999994", Sub("101.0000000000000000000000000123456", "91.00000000000000000000000001234566").value()) << "dqsub928";
  EXPECT_STREQ(L"1.99999999999999999999999999999994", Sub("101.0000000000000000000000000123456", "99.00000000000000000000000001234566").value()) << "dqsub929";
  EXPECT_STRNE(L"1.000000000000000000000000000000000", Sub("0.4444444444444444444444444444444444", "-0.5555555555555555555555555555555561").value()) << "dqsub472";
  EXPECT_STREQ(L"1231234555555555555555555567456788", Sub("1231234555555555555555555567456788", "0.5").value()) << "dqsub541";
  EXPECT_STREQ(L"-1231234555555555555555555567456788", Sub("0.5", "1231234555555555555555555567456788").value()) << "dqsub641";
}

} // CommonTest
