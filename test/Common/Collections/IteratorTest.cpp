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

namespace CommonTest {

using namespace Common;
using namespace Common::Collections;

class IteratorTest : public ::testing::Test {
};

class Item : public DoubleLinkedItem_<Item> {
  private: int value_;
  public: Item(int value) : value_(value) {}
  public: int value() { return value_; }
  public: bool operator==(const Item& another) const {
    return value_ == another.value_;
  }
  public: bool operator!=(const Item& another) const {
    return !operator==(another);
  }
  DISALLOW_COPY_AND_ASSIGN(Item);
};

TEST_F(IteratorTest, ForEachReverse) {
  DoubleLinkedList_<Item> list;
  Item item1(1);
  Item item2(2);
  Item item3(3);
  list.Append(&item1);
  list.Append(&item2);
  list.Append(&item3);
  auto it = list.rbegin();
  for (const auto& item: ReverseRange(list)) {
    EXPECT_EQ(*it, item);
    ++it;
  }
}

}  // CommonTest
