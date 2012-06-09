#include "precomp.h"
// @(#)$Id$
//
// Util Test for BigIntegerImpl
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCommonTest.h"

#include "Common/Collections/Vector_.h"
#include "Common/Numerics/BigIntegerImpl.h"
#include "Common/Numerics/Float64Impl.h"

typedef Common::Numerics::BigIntegerImpl::Unit Unit;

namespace Common {
namespace Collections {

using namespace Common::Numerics;

std::ostream& operator <<(
    std::ostream& os,
    const Vector_<Unit> v) {
  os << '[';
  auto comma = "";
  for (auto it = v.begin(); it != v.end(); ++it) {
    os << comma;
    comma = ", ";
    os << std::hex << std::showbase << *it;
  }
  os << ']';
  return os;
}

bool operator ==(const Vector_<Unit>& v, const BigIntegerImpl& b) {
  return v == b.units();
}

} // Collections
} // Common

namespace Common {
namespace Numerics {
std::ostream& operator <<(std::ostream& os, const BigIntegerImpl& b) {
  return os << b.units();
}

std::ostream& operator <<(
    std::ostream& os,
    const Pair_<BigIntegerImpl>& pair) {
  return os << '(' << pair.first() << ' ' << pair.second() << ')';
}
} // Numerics
} // Common

namespace CommonTest {

using namespace Common::Numerics;
typedef Pair_<BigIntegerImpl> Pair;

class BigIntegerImplTest : public AbstractCommonTest {
  protected: typedef BigIntegerImpl BigInt;

  protected: static int32 const kMaxInt32 = 0x7FFFFFFF;
  protected: static int32 const kMinInt32 = -1 << 31;
  protected: static uint32 const kMaxUInt32 = uint32(-1);
  protected: static int64 const kMaxInt64 = (uint64(1) << 63) - 1;
  protected: static int64 const kMinInt64 = int64(-1) << 63;
  protected: static uint64 const kMaxUInt64 = uint64(-1);

  // [F]
  protected: static BigInt FromString(const String& str) {
    enum State {
      State_Digit,
      State_Sign,
      State_Start,
      State_XDigit,
      State_Zero,
    } state = State_Start;
    auto minus = false;
    BigInt accumulator(0);
    for (auto it = str.begin(); it != str.end(); ++it) {
      auto const ch = *it;
      switch (state) {
        case State_Digit:
          if (ch >='0' && ch <= '9') {
            accumulator *= 10;
            accumulator += ch - '0';
            break;
          }
          CAN_NOT_HAPPEN();

        case State_Sign:
          if (ch == '0') {
            state = State_Zero;
            break;
          }
          if (ch == 'X' || ch == 'x') {
            state = State_XDigit;
            break;
          }
          if (ch >= '1' && ch <= '9') {
            state = State_Digit;
            accumulator = ch - '0';
            break;
          }
          CAN_NOT_HAPPEN();

        case State_Start:
          if (ch == '+') {
            state = State_Sign;
            break;
          }
          if (ch == '-') {
            state = State_Sign;
            minus = true;
            break;
          }
          if (ch == '0') {
            state = State_Zero;
            break;
          }
          if (ch >= '1' && ch <= '9') {
            state = State_Digit;
            accumulator = ch - '0';
            break;
          }
          CAN_NOT_HAPPEN();

        case State_XDigit:
          if (ch >='0' && ch <= '9') {
            accumulator *= 10;
            accumulator += ch - '0';
            break;
          }
          if (ch >='A' && ch <= 'F') {
            accumulator *= 16;
            accumulator += ch - 'A' + 10;
            break;
          }
          if (ch >='a' && ch <= 'f') {
            accumulator *= 16;
            accumulator += ch - 'a' + 10;
            break;
          }
          CAN_NOT_HAPPEN();

        case State_Zero:
          if (ch == 'X' || ch == 'x') {
            state = State_XDigit;
            break;
          }

          if (ch >= '1' && ch <= '9') {
            state = State_Digit;
            accumulator = ch - '0';
            break;
          }
          CAN_NOT_HAPPEN();

        default:
          CAN_NOT_HAPPEN();
      }
    }
    return minus ? -BigInt(accumulator) : BigInt(accumulator);
  }

  // [M]
  protected: static BigInt Minus1E22() {
    return BigInt(Vec(0x4DC00000, 0xE61F3645, 0xFFFFFDE1));
  }

  protected: static BigInt Minus1E30() {
    return BigInt(Vec(0xC0000000, 0xB98B1215, 0x60D3632F, 0xFFFFFFF3));
  }

  protected: static float64 MostPositiveFloat64() {
    return Float64Impl(Float64Impl::kMostPositivePattern);
  }

  protected: static BigInt MostPositiveFloat64Int() {
    Vector_<Unit> vec(32 + 1);
    vec.Fill(0, vec.size(), 0);
    vec[30] = 0xFFFFF800u;
    vec[31] = Unit(-1);
    vec[32] = Unit(-0);
    return BigInt(vec);
  }

  // [P]
  protected: static BigInt Plus1E22() {
    return BigInt(Vec(0xB2400000u, 0x19E0C9BA, 0x21E));
  }

  protected: static BigInt Plus1E30() {
    return BigInt(Vec(0x40000000, 0x4674EDEA, 0x9F2C9CD0u, 0x0C));
  }

  // [T]
  protected: String ToString(const BigInt& big) {
    return big.ToString();
  }

  protected: static String ToString(const Vector_<Unit>& units) {
    return BigInt(units).ToString();
  }

  // [V]
  protected: static Vector_<Unit> Vec(int a) {
    Vector_<Unit> v(1);
    v[0] = a;
    return v;
  }

  protected: static Vector_<Unit> Vec(int a, int b) {
    Vector_<Unit> v(2);
    v[0] = a;
    v[1] = b;
    return v;
  }

  protected: static Vector_<Unit> Vec(int a, int b, int c) {
    Vector_<Unit> v(3);
    v[0] = a;
    v[1] = b;
    v[2] = c;
    return v;
  }

  protected: static Vector_<Unit> Vec(int a, int b, int c, int d) {
    Vector_<Unit> v(4);
    v[0] = a;
    v[1] = b;
    v[2] = c;
    v[3] = d;
    return v;
  }

  protected: static Vector_<Unit> Vec(int a, int b, int c, int d, int e) {
    Vector_<Unit> v(5);
    v[0] = a;
    v[1] = b;
    v[2] = c;
    v[3] = d;
    v[4] = e;
    return v;
  }
};

TEST_F(BigIntegerImplTest, Add) {
  EXPECT_EQ(Vec(0), BigInt(0) + 0);
  EXPECT_EQ(Vec(1), BigInt(0) + 1);
  EXPECT_EQ(Vec(1), BigInt(1) + 0);
  EXPECT_EQ(Vec(2), BigInt(1) + 1);
  EXPECT_EQ(Vec(0, 5), BigInt(Vec(0, 2)) + BigInt(Vec(0, 3)));
}

TEST_F(BigIntegerImplTest, BitAnd) {
  EXPECT_EQ(Vec(0), BigInt(0) & 0);
  EXPECT_EQ(Vec(0), BigInt(0) & 1);
  EXPECT_EQ(Vec(1), BigInt(1) & 1);
  EXPECT_EQ(Vec(0x1111), BigInt(0x9999) & 0x5555);
  EXPECT_EQ(Vec(0, 1), BigInt(Vec(0, 1)) & BigInt(Vec(0, 1)));
}

TEST_F(BigIntegerImplTest, BitNot) {
  EXPECT_EQ(Vec(-1), ~BigInt(0));
  EXPECT_EQ(Vec(-2), ~BigInt(1));
  EXPECT_EQ(Vec(0, 1), BigInt(Vec(0, 1)));
  EXPECT_EQ(Vec(0), ~BigInt(-1));
  EXPECT_EQ(Vec(-1, 0), ~BigInt(Vec(0, -1)));
}

TEST_F(BigIntegerImplTest, BitOr) {
  EXPECT_EQ(Vec(0), BigInt(0) | 0);
  EXPECT_EQ(Vec(1), BigInt(0) | 1);
  EXPECT_EQ(Vec(1), BigInt(1) | 1);
  EXPECT_EQ(Vec(0xDDDD), BigInt(0x9999) | 0x5555);
  EXPECT_EQ(Vec(0, 1), BigInt(Vec(0, 1)) | BigInt(Vec(0, 1)));
}

TEST_F(BigIntegerImplTest, BitXor) {
  EXPECT_EQ(Vec(0), BigInt(0) ^ 0);
  EXPECT_EQ(Vec(1), BigInt(0) ^ 1);
  EXPECT_EQ(Vec(0), BigInt(1) ^ 1);
  EXPECT_EQ(Vec(0xCCCC), BigInt(0x9999) ^ 0x5555);
  EXPECT_EQ(Vec(0), BigInt(Vec(0, 1)) ^ BigInt(Vec(0, 1)));
}

TEST_F(BigIntegerImplTest, BitLength) {
  EXPECT_EQ(0, BigInt(0).bit_length());
  EXPECT_EQ(1, BigInt(1).bit_length());
  EXPECT_EQ(0, BigInt(-1).bit_length());
  EXPECT_EQ(16, BigInt((1 << 16) - 1).bit_length());
  EXPECT_EQ(17, BigInt(1 << 16).bit_length());

  EXPECT_EQ(31, BigInt(kMaxInt32).bit_length());
  EXPECT_EQ(31, BigInt(kMinInt32).bit_length());
  EXPECT_EQ(32, BigInt(kMaxUInt32).bit_length());
  EXPECT_EQ(63, BigInt(kMaxInt64).bit_length());
  EXPECT_EQ(63, BigInt(kMinInt64).bit_length());
  EXPECT_EQ(64, BigInt(kMaxUInt64).bit_length());
}

TEST_F(BigIntegerImplTest, Compare) {
  auto const v01 = BigInt(Vec(0, 1));
  auto const v0m1 = BigInt(Vec(0, -1));

  EXPECT_TRUE(BigInt(0).CompareTo(0) == 0);
  EXPECT_TRUE(BigInt(0).CompareTo(1) < 0);
  EXPECT_TRUE(BigInt(1).CompareTo(0) > 0);
  EXPECT_TRUE(BigInt(1).CompareTo(1) == 0);

  EXPECT_TRUE(BigInt(1).CompareTo(2) < 0);
  EXPECT_TRUE(BigInt(2).CompareTo(1) > 0);

  EXPECT_TRUE(BigInt(0).CompareTo(0) == 0);
  EXPECT_TRUE(BigInt(0).CompareTo(-1) > 0);
  EXPECT_TRUE(BigInt(-1).CompareTo(0) < 0);
  EXPECT_TRUE(BigInt(-1).CompareTo(-1) == 0);

  EXPECT_TRUE(BigInt(-1).CompareTo(-2) > 0);
  EXPECT_TRUE(BigInt(-2).CompareTo(-1) < 0);

  EXPECT_TRUE(BigInt(0).CompareTo(v01) < 0);
  EXPECT_TRUE(BigInt(v01).CompareTo(0) > 0);
  EXPECT_TRUE(BigInt(v01).CompareTo(v01) == 0);

  EXPECT_TRUE(BigInt(0).CompareTo(0) == 0);
  EXPECT_TRUE(BigInt(0).CompareTo(v0m1) > 0);
  EXPECT_TRUE(BigInt(v0m1).CompareTo(0) < 0);
  EXPECT_TRUE(BigInt(v0m1).CompareTo(v0m1) == 0);
}

TEST_F(BigIntegerImplTest, CompareOperator) {
  EXPECT_TRUE(BigInt(1) == 1);
  EXPECT_FALSE(BigInt(1) == 0);
  EXPECT_TRUE(BigInt(-1) == -1);
  EXPECT_FALSE(BigInt(-1) == 0);

  EXPECT_FALSE(BigInt(1) != 1);
  EXPECT_TRUE(BigInt(1) != 0);
  EXPECT_FALSE(BigInt(-1) != -1);
  EXPECT_TRUE(BigInt(-1) != 0);

  EXPECT_FALSE(BigInt(1) < 1);
  EXPECT_FALSE(BigInt(1) < 0);
  EXPECT_FALSE(BigInt(-1) < -1);
  EXPECT_TRUE(BigInt(-1) < 0);

  EXPECT_TRUE(BigInt(1) <= 1);
  EXPECT_FALSE(BigInt(1) <= 0);
  EXPECT_TRUE(BigInt(-1) <= -1);
  EXPECT_TRUE(BigInt(-1) <= 0);

  EXPECT_FALSE(BigInt(1) > 1);
  EXPECT_TRUE(BigInt(1) > 0);
  EXPECT_FALSE(BigInt(-1) > -1);
  EXPECT_FALSE(BigInt(-1) > 0);

  EXPECT_TRUE(BigInt(1) >= 1);
  EXPECT_TRUE(BigInt(1) >= 0);
  EXPECT_TRUE(BigInt(-1) >= -1);
  EXPECT_FALSE(BigInt(-1) >= 0);
}

TEST_F(BigIntegerImplTest, Ctor) {
  EXPECT_EQ(Vec(0), BigInt(int32(0)));
  EXPECT_EQ(Vec(1), BigInt(int32(1)));
  EXPECT_EQ(Vec(-1), BigInt(int32(-1)));
  EXPECT_EQ(Vec(-2), BigInt(int32(-2)));
  EXPECT_EQ(Vec(kMaxInt32), BigInt(kMaxInt32));
  EXPECT_EQ(Vec(kMinInt32), BigInt(kMinInt32));

  EXPECT_EQ(Vec(0), BigInt(uint32(0)));
  EXPECT_EQ(Vec(1), BigInt(uint32(1)));
  EXPECT_EQ(Vec(-1, 0), BigInt(uint32(-1)));
  EXPECT_EQ(Vec(-2, 0), BigInt(uint32(-2)));
  EXPECT_EQ(Vec(kMaxInt32), BigInt(uint32(kMaxInt32)));
  EXPECT_EQ(Vec(kMinInt32, 0), BigInt(uint32(kMinInt32)));
  EXPECT_EQ(Vec(kMaxUInt32, 0), BigInt(kMaxUInt32));

  EXPECT_EQ(Vec(0), BigInt(int64(0)));
  EXPECT_EQ(Vec(1), BigInt(int64(1)));
  EXPECT_EQ(Vec(-1), BigInt(int64(-1)));
  EXPECT_EQ(Vec(-2), BigInt(int64(-2)));
  EXPECT_EQ(Vec(-1, kMaxInt32), BigInt(kMaxInt64));
  EXPECT_EQ(Vec(0, kMinInt32), BigInt(kMinInt64));

  EXPECT_EQ(Vec(0), BigInt(uint64(0)));
  EXPECT_EQ(Vec(1), BigInt(uint64(1)));
  EXPECT_EQ(Vec(-1, -1, 0), BigInt(uint64(-1)));
  EXPECT_EQ(Vec(-2, -1, 0), BigInt(uint64(-2)));
  EXPECT_EQ(Vec(-1, kMaxInt32), BigInt(uint64(kMaxInt64)));
  EXPECT_EQ(Vec(0, kMinInt32, 0), BigInt(uint64(kMinInt64)));
  EXPECT_EQ(Vec(-1, -1, 0), BigInt(kMaxUInt64));

  EXPECT_EQ(Vec(kMaxInt32), BigInt(int64(kMaxInt32)));
  EXPECT_EQ(Vec(kMinInt32), BigInt(int64(kMinInt32)));

  EXPECT_EQ(Vec(0), BigInt(Vec(0)));
  EXPECT_EQ(Vec(1), BigInt(Vec(1)));
  EXPECT_EQ(Vec(-1), BigInt(Vec(-1)));
  EXPECT_EQ(Vec(1, 2), BigInt(Vec(1, 2)));
  EXPECT_EQ(Vec(1, 2, 3), BigInt(Vec(1, 2, 3)));
  EXPECT_EQ(Vec(1, 2, 3, 4), BigInt(Vec(1, 2, 3, 4)));

  // non-canonical vector
  EXPECT_EQ(Vec(1, 2), BigInt(Vec(1, 2, 0)));
  EXPECT_EQ(Vec(1, -2), BigInt(Vec(1, -2, -1)));
}

TEST_F(BigIntegerImplTest, CtorFloat) {
  EXPECT_EQ(Vec(10), BigInt(10.0));
  EXPECT_EQ(Vec(-10), BigInt(-10.0));
  EXPECT_EQ(Plus1E22(), BigInt(1E22));
  EXPECT_EQ(Minus1E22(), BigInt(-1E22));
  EXPECT_EQ(Vec(0), BigInt(0.5));
  EXPECT_EQ(MostPositiveFloat64Int(), BigInt(MostPositiveFloat64()));
}

TEST_F(BigIntegerImplTest, FromString) {
  EXPECT_EQ(Vec(0), FromString("0"));
  EXPECT_EQ(Vec(1), FromString("1"));
  EXPECT_EQ(Vec(12), FromString("12"));
  EXPECT_EQ(Vec(123), FromString("123"));
  EXPECT_EQ(Vec(-1), FromString("-1"));
  EXPECT_EQ(Vec(-12), FromString("-12"));
  EXPECT_EQ(Vec(-123), FromString("-123"));
  EXPECT_EQ(Vec(0x499602D2), FromString("1234567890"));
  EXPECT_EQ(
      Vec(0xEB1F0AD2u, 0xAB54A98Cu, 0),
      FromString("12345678901234567890"));
  EXPECT_EQ(Plus1E22(), FromString("10000000000000000000000"));
  EXPECT_EQ(Minus1E22(), FromString("-10000000000000000000000"));
  EXPECT_EQ(
      Plus1E30(),
      FromString("1000000000000000000000000000000"));
  EXPECT_EQ(
      Minus1E30(),
      FromString("-1000000000000000000000000000000"));
}

TEST_F(BigIntegerImplTest, Multiply) {
  EXPECT_EQ(Vec(0xE0000000u), BigInt(-268435456) * 2);
  EXPECT_EQ(Vec(0xF1000000u, 0xEFFFFFF), BigInt(0xFFFFFFFFu) * 0xF000000);
  EXPECT_EQ(Minus1E22(), BigInt(-1) * Plus1E22());
  EXPECT_EQ(Plus1E30(), BigInt(Minus1E30()) * -1);
}

TEST_F(BigIntegerImplTest, Negate) {
  EXPECT_EQ(Vec(0), -BigInt(0));
  EXPECT_EQ(Vec(-1), -BigInt(1));
  EXPECT_EQ(Vec(1), -BigInt(-1));
  EXPECT_EQ(Vec(0, 0x200000), -BigInt(int64(-1) << 53));
  EXPECT_EQ(Vec(0, -1), -BigInt(Vec(0, 1)));
  EXPECT_EQ(Vec(0, 0, -1), -BigInt(Vec(0, 0, 1)));
  EXPECT_EQ(Vec(0, 0, 0, -1), -BigInt(Vec(0, 0, 0, 1)));
  EXPECT_EQ(Minus1E22(), -Plus1E22());
}

TEST_F(BigIntegerImplTest, Remiander) {
  EXPECT_EQ(BigInt(+2), BigInt(+50) % +6);
  EXPECT_EQ(BigInt(-2), BigInt(-50) % +6);
  EXPECT_EQ(BigInt(+2), BigInt(+50) % -6);
  EXPECT_EQ(BigInt(-2), BigInt(-50) % -6);
}

TEST_F(BigIntegerImplTest, ShiftLeft) {
  EXPECT_EQ(Vec(0), BigInt(0) << 100);
  EXPECT_EQ(Vec(1 << 16), BigInt(1) << 16);
  EXPECT_EQ(Vec(0, 1), BigInt(1) << 32);
  EXPECT_EQ(Vec(0, 0, 1), BigInt(1) << 64);
  EXPECT_EQ(Vec(0, 0, 0, 0x10), BigInt(1) << 100);
  EXPECT_EQ(Vec(0, -1), BigInt(-1) << 32);
  EXPECT_EQ(Vec(0, 0, -1), BigInt(-1) << 64);
  EXPECT_EQ(MostPositiveFloat64Int(), BigInt(0x1FFFFFFFFFFFFF) << 971);
}

TEST_F(BigIntegerImplTest, ShiftRight) {
  EXPECT_EQ(Vec(0), BigInt(0) >> 16);
  EXPECT_EQ(Vec(0), BigInt(1) >> 16);
  EXPECT_EQ(Vec(1), BigInt(Vec(0, 1)) >> 32);
  EXPECT_EQ(Vec(0, 1), BigInt(Vec(0, 0, 1)) >> 32);
  EXPECT_EQ(Vec(0x10000), BigInt(Vec(0, 0, 1)) >> 48);
  EXPECT_EQ(Vec(0, 0, 1), BigInt(Vec(0, 0, 0, 1)) >> 32);
  EXPECT_EQ(Vec(0, 0, -1), BigInt(Vec(0, 0, 0, -1)) >> 32);
}

TEST_F(BigIntegerImplTest, ToFloat64) {
  EXPECT_EQ(0.0, float64(BigInt(0)));
  EXPECT_EQ(1.0, float64(BigInt(1)));
  EXPECT_EQ(-1.0, float64(BigInt(-1)));
  EXPECT_EQ(MostPositiveFloat64(), float64(BigInt(0x1FFFFFFFFFFFFF) << 971));
  EXPECT_EQ(
    float64(Float64Impl(Float64Impl::kPositiveInfinityPattern)),
    float64(BigInt(1) << 1024));
}

TEST_F(BigIntegerImplTest, ToString) {
  EXPECT_STREQ(L"0", ToString(0).value());
  EXPECT_STREQ(L"1", ToString(1).value());
  EXPECT_STREQ(L"-1", ToString(Vec(-1)).value());
  EXPECT_STREQ(L"4294967296", ToString(Vec(0, 1)).value());
  EXPECT_STREQ(L"18446744073709551616", ToString(Vec(0, 0, 1)).value());
}

TEST_F(BigIntegerImplTest, Subtract) {
  EXPECT_EQ(BigInt(-536870913), BigInt(-1 << 29) - 1);
  EXPECT_EQ(BigInt(-1152921504606846977), BigInt(int64(-1) << 60) - 1);
  EXPECT_EQ(Minus1E30(), BigInt(0) - BigInt(Plus1E30()));
}

TEST_F(BigIntegerImplTest, Truncate) {
  EXPECT_EQ(Pair(0, 0), BigInt(0).Truncate(10));
  EXPECT_EQ(Pair(0, 1), BigInt(1).Truncate(10));
  EXPECT_EQ(Pair(1, 2), BigInt(12).Truncate(10));
  EXPECT_EQ(Pair(10, 3), BigInt(103).Truncate(10));

  EXPECT_EQ(Pair(0, -1), BigInt(-1).Truncate(10));
  EXPECT_EQ(Pair(-1, -2), BigInt(-12).Truncate(10));
  EXPECT_EQ(Pair(-10, -3), BigInt(-103).Truncate(10));

  EXPECT_EQ(Pair(429496729, 6), BigInt(Vec(0, 1)).Truncate(10));
  EXPECT_EQ(
    Pair(BigInt(Vec(0x99999999, 0x19999999)), 6),
    BigInt(Vec(0, 0, 1)).Truncate(10));

  // Truncate by one unit
  EXPECT_EQ(
    Pair(BigInt(Vec(-1, 0)), BigInt(0)),
    BigInt(Vec(0x80000000u, 0x7FFFFFFF)).Truncate(0x80000000u));

  // D3 Calculate qhat
  // overflow in (u[j+n] * base + u[j+n-1]) / v[n-1]
  EXPECT_EQ(
    Pair(
        BigInt(Vec(0xFFFFFFFE, 0)),
        BigInt(Vec(0x55555556, 0xAAAAAAAAu, 0x2AAAAAAA))),
    BigInt(Vec(0, 0, 0x80000000u, 0x2AAAAAAA))
        .Truncate(BigInt(Vec(0xAAAAAAAB, 0xAAAAAAAA, 0x2AAAAAAA))));

  // D6 Add back for 32 bit base
  //  u = 170141183420855150474555134919112130560
  //  v = 39614081257132168796771975169
  //  truncate(u, v) = 4294967294, 39614081257132168792477007874
  EXPECT_EQ(
    Pair(
        BigInt(Vec(0xFFFFFFFEu, 0)),
        BigInt(Vec(2, -1, 0x7FFFFFFF))),
    BigInt(Vec(0, 0, 0x80000000u, 0x7FFFFFFF)).Truncate(
        BigInt(Vec(1, 0, 0x80000000u, 0))));
  EXPECT_EQ(
    Pair(
        BigInt(Vec(0xFFFFFFFDu, 1)),
        BigInt(Vec(0x80000000u, 0xC0000001u, 0x7FFFFFFF))),
    BigInt(Vec(0, 0, 0x80000000u, -1, 0)).Truncate(
        BigInt(Vec(0x80000000u, 0x40000000u, 0x80000000u, 0))));
}

} // CommonTest
