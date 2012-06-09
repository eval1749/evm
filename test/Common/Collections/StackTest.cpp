#include "precomp.h"
// @(#)$Id$
//
// Evita Stack Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Common.h"
#include "Common/Collections.h"

using namespace Common;
using namespace Common::Collections;

class StackTest : public ::testing::Test {
  protected: typedef Stack_<String> Stack;
  protected: Stack stack_;

  protected: virtual void SetUp() override {
    stack_.Push("a");
    stack_.Push("b");
    stack_.Push("c");
  }
};

TEST_F(StackTest, Clear) {
  stack_.Clear();
  EXPECT_EQ(0, stack_.Count());
  EXPECT_TRUE(stack_.IsEmpty());
}

TEST_F(StackTest, Enum) {
  Stack::Enum oEnum(stack_);
  const char* const samples[] = { "c", "b", "a" };
  for (auto p = samples; p < samples + lengthof(samples); p++) {
    EXPECT_TRUE(!oEnum.AtEnd());
    unless (oEnum.AtEnd()) break;
    auto elem = oEnum.Get();
    EXPECT_EQ(String(*p), elem);
    oEnum.Next();
  }
}

TEST_F(StackTest, Count) {
  EXPECT_EQ(3, stack_.Count());
}

TEST_F(StackTest, GetTop) {
  EXPECT_EQ(String("c"), stack_.GetTop());
}

TEST_F(StackTest, IsEmpty) {
  EXPECT_FALSE(stack_.IsEmpty());
}

TEST_F(StackTest, Pop) {
  EXPECT_EQ(String("c"), stack_.Pop());
  EXPECT_EQ(String("b"), stack_.Pop());
  EXPECT_EQ(String("a"), stack_.Pop());
}
