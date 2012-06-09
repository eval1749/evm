#include "precomp.h"
// @(#)$Id$
//
// Util Test for Decimal128
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCommonTest.h"

#include "Common/Numerics/Decimal128Impl.h"
#include "Common/Numerics/DecimalTables.h"

namespace Common {
namespace Numerics {
::std::ostream& operator<<(::std::ostream& os, const Decimal128Impl& d) {
  return os << d.ToString().value();
}
} // Numerics
} // Common

namespace CommonTest {

using namespace Common::Numerics;

class Decimal128ImplTest : public AbstractCommonTest {
  protected: class EncodedData : public Decimal128Impl::EncodedData {
    public: EncodedData() { ::ZeroMemory(words_, sizeof(words_)); }
  };
};

TEST_F(Decimal128ImplTest, PropertiesAllZero) {
  Decimal128Impl all_zero;
  EXPECT_EQ(0, all_zero.value().coefficent_msd());
  EXPECT_EQ(0, all_zero.value().combination_field());
  EXPECT_EQ(0, all_zero.value().dpd_at(0));
  EXPECT_EQ(0, all_zero.value().exponent_biased());
  EXPECT_EQ(0, all_zero.value().exponent_continuation());
  EXPECT_EQ(0, all_zero.value().exponent_msb());
  EXPECT_EQ(0, all_zero.value().sign());
}

TEST_F(Decimal128ImplTest, PropertiesExponent) {
  EncodedData x;
  auto const kExponent = (1 << 12) + 3;
  x.set_exponent(kExponent);
  EXPECT_EQ(kExponent, Decimal128Impl(x).exponent());
  EXPECT_EQ(2083, x.exponent_continuation());
  EXPECT_EQ(2, x.exponent_msb());
}

TEST_F(Decimal128ImplTest, PropertiesIeeeClass) {
  EXPECT_EQ(Class_Infinity, Decimal128Impl::Infinity(0).ieee_class());
  EXPECT_EQ(Class_Infinity, Decimal128Impl::Infinity(1).ieee_class());
  EXPECT_EQ(Class_QuietNaN, Decimal128Impl::QuietNaN(0).ieee_class());
  EXPECT_EQ(Class_QuietNaN, Decimal128Impl::QuietNaN(1).ieee_class());
  EXPECT_EQ(Class_SignalingNaN, Decimal128Impl::SignalingNaN(0).ieee_class());
  EXPECT_EQ(Class_SignalingNaN, Decimal128Impl::SignalingNaN(1).ieee_class());
  EXPECT_EQ(Class_Zero, Decimal128Impl::Zero(0).ieee_class());
  EXPECT_EQ(Class_Zero, Decimal128Impl::Zero(1).ieee_class());
  EXPECT_EQ(Class_Normal, Decimal128Impl(1).ieee_class());
}

TEST_F(Decimal128ImplTest, Epsilon) {
  EncodedData epsilon_data;
  epsilon_data.set_exponent(Decimal128Impl::kEmin);
  epsilon_data.set_dpd_at(0, 1);
  Decimal128Impl epsilon(epsilon_data);
  EXPECT_STREQ(
    L"1E-6143",
    epsilon.ToString().value());
  EXPECT_STREQ(
    L"1.000000000000000000000000000000000",
    (epsilon + Decimal128Impl(1)).ToString().value());
}

TEST_F(Decimal128ImplTest, LeastPositiveNormalized) {
  EncodedData data;
  data.set_exponent(-Decimal128Impl::kEbias + 1);
  data.set_dpd_at(10, kPackedBcd3ToDpd[0x100]);

  Decimal128Impl least_positive_normalized(data);
  EXPECT_EQ(0, least_positive_normalized.value().words_[0]);
  EXPECT_EQ(0, least_positive_normalized.value().words_[1]);
  EXPECT_EQ(0, least_positive_normalized.value().words_[2]);
  EXPECT_EQ(0x4800, least_positive_normalized.value().words_[3]);

  EXPECT_EQ(Class_Normal, least_positive_normalized.ieee_class());
  EXPECT_STREQ(
    L"1.00000000000000000000000000000000E-6143",
    least_positive_normalized.ToString().value());
  EXPECT_EQ(
    least_positive_normalized,
    Decimal128Impl::FromString("1.00000000000000000000000000000000E-6143"));

  EXPECT_STREQ(
    L"1.000000000000000000000000000000000",
    (least_positive_normalized + Decimal128Impl(1)).ToString().value());
}

TEST_F(Decimal128ImplTest, PropertiesMinusOne) {
  Decimal128Impl minus_one(-1);
  EXPECT_EQ(0, minus_one.value().coefficent_msd());
  EXPECT_EQ(8, minus_one.value().combination_field());
  EXPECT_EQ(1, minus_one.value().dpd_at(0));
  EXPECT_EQ(6176, minus_one.value().exponent_biased());
  EXPECT_EQ(2080, minus_one.value().exponent_continuation());
  EXPECT_EQ(1, minus_one.value().exponent_msb());
  EXPECT_EQ(1, minus_one.value().sign());
}

TEST_F(Decimal128ImplTest, PropertiesOne) {
  Decimal128Impl one(1);
  EXPECT_EQ(0, one.value().coefficent_msd());
  EXPECT_EQ(8, one.value().combination_field());
  EXPECT_EQ(1, one.value().dpd_at(0));
  EXPECT_EQ(6176, one.value().exponent_biased());
  EXPECT_EQ(2080, one.value().exponent_continuation());
  EXPECT_EQ(1, one.value().exponent_msb());
  EXPECT_EQ(0, one.value().sign());
}

TEST_F(Decimal128ImplTest, PropertiesSet) {
  EncodedData x;
  x.set_combination_field(1, 4);
  for (auto i = 1; i < 11; i++) {
    x.set_dpd_at(i, i);
  }

  for (auto i = 1; i < 11; i++) {
    EXPECT_EQ(i, x.dpd_at(i));
  }

  EXPECT_EQ(4, x.coefficent_msd());
  EXPECT_EQ(1, x.exponent_msb());
}

TEST_F(Decimal128ImplTest, Subnormal) {
  EncodedData data;
  data.set_exponent(-Decimal128Impl::kEbias);
  data.set_dpd_at(0, 1);
  Decimal128Impl subnormal(data);
  EXPECT_EQ(Class_Subnormal, subnormal.ieee_class());
  EXPECT_STREQ(L"1E-6176", subnormal.ToString().value());
  EXPECT_EQ(subnormal, Decimal128Impl::FromString("1E-6176"));
}

TEST_F(Decimal128ImplTest, PropertiesZero) {
  Decimal128Impl zero(0);
  EXPECT_EQ(0, zero.value().coefficent_msd());
  EXPECT_EQ(8, zero.value().combination_field());
  EXPECT_EQ(0, zero.value().dpd_at(0));
  EXPECT_EQ(6176, zero.value().exponent_biased());
  EXPECT_EQ(2080, zero.value().exponent_continuation());
  EXPECT_EQ(1, zero.value().exponent_msb());
  EXPECT_EQ(0, zero.value().sign());
}

} // CommonTest
