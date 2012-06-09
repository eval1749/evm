#include "precomp.h"
// @(#)$Id$
//
// Evita Queue Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Common.h"
#include "Common/Collections.h"

using namespace Common;
using namespace Common::Collections;

class QueueTest : public ::testing::Test {
  protected: typedef Queue_<String> Queue;
  protected: Queue queue_;

  protected: virtual void SetUp() override {
    queue_.Give("a");
    queue_.Give("b");
    queue_.Give("c");
  }
};

TEST_F(QueueTest, Clear) {
  queue_.Clear();
  EXPECT_EQ(0, queue_.Count());
  EXPECT_TRUE(queue_.IsEmpty());
}

TEST_F(QueueTest, Enum) {
  Queue::Enum oEnum(queue_);
  String const samples[] = { "a", "b", "c" };
  for (auto p = samples; p < samples + lengthof(samples); p++) {
    EXPECT_TRUE(!oEnum.AtEnd());
    unless (oEnum.AtEnd()) break;
    auto const elem = oEnum.Get();
    EXPECT_EQ(*p, elem);
    oEnum.Next();
  }
}

TEST_F(QueueTest, Count) {
  EXPECT_EQ(3, queue_.Count());
}

TEST_F(QueueTest, IsEmpty) {
  EXPECT_FALSE(queue_.IsEmpty());
}

TEST_F(QueueTest, Take) {
  EXPECT_EQ(String("a"), queue_.Take());
}
