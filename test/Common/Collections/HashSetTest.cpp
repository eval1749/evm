#include "precomp.h"
// @(#)$Id$
//
// Evita String Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Common.h"
#include "Common/Collections.h"

using namespace Common;
using namespace Common::Collections;

class HashSetTest : public ::testing::Test {
  protected: typedef HashSet_<String> HashSet;

  protected: HashSet set_;

  protected: HashSetTest() {
    set_.Add("1");
    set_.Add("2");
    set_.Add("3");
  }
};

TEST_F(HashSetTest, ctor_Collection) {
  decltype(set_) set(set_);
  EXPECT_EQ(3, set.Count());
  EXPECT_TRUE(set.Contains("1"));
  EXPECT_TRUE(set.Contains("2"));
  EXPECT_TRUE(set.Contains("3"));
}

TEST_F(HashSetTest, Add) {
  EXPECT_EQ(3, set_.Count());
  EXPECT_TRUE(set_.Contains("1"));
  EXPECT_TRUE(set_.Contains("2"));
  EXPECT_TRUE(set_.Contains("3"));
}

TEST_F(HashSetTest, AddAll) {
  decltype(set_) set;
  set.AddAll(set_);
  EXPECT_EQ(3, set.Count());
  EXPECT_TRUE(set.Contains("1"));
  EXPECT_TRUE(set.Contains("2"));
  EXPECT_TRUE(set.Contains("3"));
}

TEST_F(HashSetTest, Clear) {
  EXPECT_EQ(3, set_.Count());
  set_.Clear();
  EXPECT_EQ(0, set_.Count());
  EXPECT_TRUE(set_.IsEmpty());
}

TEST_F(HashSetTest, Enum) {
  int flagv[256];
  ::ZeroMemory(flagv, sizeof(flagv));

  auto n = 0;
  foreach (HashSet::Enum, oEnum, set_) {
    auto const entry = oEnum.Get();
    flagv[*entry.value()]++;
    n++;
  }
  EXPECT_EQ(3, n);
  EXPECT_EQ(1, flagv['1']);
  EXPECT_EQ(1, flagv['2']);
  EXPECT_EQ(1, flagv['3']);
}

TEST_F(HashSetTest, Remove) {
  EXPECT_TRUE(set_.Remove("1"));
  EXPECT_FALSE(set_.Contains("1"));
  EXPECT_EQ(2, set_.Count());

  EXPECT_FALSE(set_.Remove("not_exist"));
  EXPECT_EQ(2, set_.Count());
}
