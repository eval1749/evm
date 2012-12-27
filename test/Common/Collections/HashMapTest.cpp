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

class HashMapTest : public ::testing::Test {
  protected: typedef HashMap_<String, String> HashMap;
  protected: HashMap map_;

  protected: virtual void SetUp() override {
    map_.Add("1", "a");
    map_.Add("2", "b");
    map_.Add("3", "c");
  }
};

TEST_F(HashMapTest, Add) {
  EXPECT_EQ(3, map_.Count());
  EXPECT_EQ(String("a"), map_.Get("1"));
  EXPECT_EQ(String("b"), map_.Get("2"));
  EXPECT_EQ(String("c"), map_.Get("3"));
  EXPECT_EQ(String(), map_.Get("not_exist"));
}

TEST_F(HashMapTest, Clear) {
  EXPECT_EQ(3, map_.Count());
  map_.Clear();
  EXPECT_EQ(0, map_.Count());
  EXPECT_TRUE(map_.IsEmpty());
}

TEST_F(HashMapTest, Contains) {
  EXPECT_TRUE(map_.Contains("2"));
  EXPECT_FALSE(map_.Contains("not_exist"));
}

TEST_F(HashMapTest, Enum) {
  int flagv[256];
  ::ZeroMemory(flagv, sizeof(flagv));

  auto n = 0;
  foreach (HashMap::Enum, entries, map_) {
    auto const entry = entries.Get();
    flagv[*entry.GetKey().GetString()]++;
    flagv[*entry.GetValue().GetString()]++;
    n++;
  }
  EXPECT_EQ(3, n);
  EXPECT_EQ(1, flagv['1']);
  EXPECT_EQ(1, flagv['2']);
  EXPECT_EQ(1, flagv['3']);
  EXPECT_EQ(1, flagv['a']);
  EXPECT_EQ(1, flagv['b']);
  EXPECT_EQ(1, flagv['c']);
}

TEST_F(HashMapTest, ForEach) {
  int flagv[256];
  ::ZeroMemory(flagv, sizeof(flagv));

  auto n = 0;
  for (auto const entry: map_) {
    flagv[*entry.GetKey().GetString()]++;
    flagv[*entry.GetValue().GetString()]++;
    ++n;
  }
  EXPECT_EQ(3, n);
  EXPECT_EQ(1, flagv['1']);
  EXPECT_EQ(1, flagv['2']);
  EXPECT_EQ(1, flagv['3']);
  EXPECT_EQ(1, flagv['a']);
  EXPECT_EQ(1, flagv['b']);
  EXPECT_EQ(1, flagv['c']);
}

TEST_F(HashMapTest, Remove) {
  EXPECT_TRUE(map_.Remove("1"));
  EXPECT_EQ(2, map_.Count());
  EXPECT_EQ(String(), map_.Get("1"));
  EXPECT_FALSE(map_.Remove("a"));
  EXPECT_EQ(2, map_.Count());
}
