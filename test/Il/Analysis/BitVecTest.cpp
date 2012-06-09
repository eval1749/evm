#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Il/Analysis/BitVec.h"

namespace IlTest {

using Il::Ir::BitVec;

class BitVecTest : public ::testing::Test {
  protected: static int const N = 71; // = 32 x 2 + 7

  protected: bool CheckBitVec(const BitVec& bitvec, uint32 const a) {
    uint32 v[1] = { a };
    return CheckBitVec(bitvec, v, ARRAYSIZE(v));
  }

  protected: bool CheckBitVec(
      const BitVec& bitvec,
      const uint32* const v, 
      int const vn) {
    auto const BITS = sizeof(int) * 8;
    int count = BITS;
    int k = 0;
    int x = v[0];
    for (int i = 0; i < bitvec.Count(); ++i) {
      if ((x & 1) == 0) {
        if (bitvec.IsOne(i)) {
          return false;
        }
      } else {
        if (bitvec.IsZero(i)) {
          return false;
        }
      }
      x >>= 1;
      --count;
      if (count == 0) {
        x = v[k];
        k = (k + 1) % vn;
        count = BITS;
      }
    }
    return true;
  }

  protected: BitVec& SetBitVec(BitVec& bitvec, uint32 a) {
    uint32 v[1] = { a };
    return SetBitVec(bitvec, v, ARRAYSIZE(v));
  }

  protected: BitVec& SetBitVec(BitVec& bitvec,
                               const uint32* const v, int const vn) {
    auto const BITS = sizeof(int) * 8;
    int count = BITS;
    int k = 0;
    uint32 x = v[0];
    for (int i = 0; i < bitvec.Count(); ++i) {
      if ((x & 1) == 0) {
        bitvec.SetZero(i);
      } else {
        bitvec.SetOne(i);
      }
      x >>= 1;
      --count;
      if (count == 0) {
        x = v[k];
        k = (k + 1) % vn;
        count = BITS;
      }
    }
    return bitvec;
  }
};

TEST_F(BitVecTest, Ctor) {
  BitVec bitvec1(0);
  EXPECT_EQ(0, bitvec1.Count());

  BitVec bitvec2(1);
  EXPECT_EQ(1, bitvec2.Count());
}

TEST_F(BitVecTest, Adjust) {
  BitVec bitvec(1);
  bitvec.Adjust(N);
  EXPECT_EQ(N, bitvec.Count());
}

TEST_F(BitVecTest, AndC2) {
  BitVec bitvec1(N);
  SetBitVec(bitvec1, 0x55AAAA55);

  BitVec bitvec2(N);
  SetBitVec(bitvec2, 0x33CC33CC);

  bitvec1.AndC2(bitvec2);
  EXPECT_TRUE(CheckBitVec(bitvec1, 0x44228811));
}

TEST_F(BitVecTest, Fill) {
  BitVec bitvec1(N);

  bitvec1.FillZero();
  EXPECT_EQ(-1, bitvec1.FindOne());

  bitvec1.FillOne();
  EXPECT_EQ(0, bitvec1.FindOne());
}

TEST_F(BitVecTest, FindLastOne) {
  BitVec bitvec1(N);
  bitvec1.FillZero();
  bitvec1.SetOne(N - 6);
  EXPECT_TRUE(bitvec1.IsOne(N - 6));
}

TEST_F(BitVecTest, Equals) {
  BitVec bitvec1(N);
  bitvec1.FillZero();
  bitvec1.SetOne(N - 6);
  EXPECT_TRUE(bitvec1.Equals(bitvec1));

  BitVec bitvec2(N);
  bitvec2.FillZero();
  EXPECT_FALSE(bitvec1.Equals(bitvec2));

  bitvec2.SetOne(N - 6);
  EXPECT_TRUE(bitvec1.Equals(bitvec2));
}


TEST_F(BitVecTest, Ior) {
  BitVec bitvec1(N);
  SetBitVec(bitvec1, 0x55AAAA55);

  BitVec bitvec2(N);
  SetBitVec(bitvec2, 0x33CC33CC);

  bitvec1.Ior(bitvec2);
  EXPECT_TRUE(CheckBitVec(bitvec1, 0x77EEBBDD));
}

}  // IlTest
