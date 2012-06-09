#include "precomp.h"
// @(#)$Id$
//
// Evita String Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCommonTest.h"

#include "Common/Collections.h"

namespace CommonTest {

using Common::Collections::ArrayList_;
using Common::Collections::BinaryTree_;

struct Value {
  int max_;
  int min_;
  public: Value() : max_(0), min_(0) {}
  public: explicit Value(int x) : max_(x), min_(x) {}
  public: Value(int min, int max) : max_(max), min_(min) {}
  
  public: bool operator==(int x) const { 
    return x >= min_ && x < max_; 
  }

  public: bool operator ==(const Value& r) const {
    return max_ == r.max_ && min_ == r.min_;
  }

  public: bool operator <(int x) const { return min_ < x; }
  public: bool operator <(const Value& r) const { return min_ < r.min_; }
};

::std::ostream& operator <<(::std::ostream& os, const Value& value) {
  return os << String::Format("Value(%d,%d)", value.min_, value.max_);
}

class BinaryTreeTest : public AbstractCommonTest {
  protected: typedef BinaryTree_<int, Value> BinaryTree;
  private: BinaryTree tree_;

  protected: BinaryTree& tree() { return tree_; }

  protected: void SetUp() {
    AbstractCommonTest::SetUp();
    for (int i = 0; i < 100; i += 10) {
      if (i != 50) {
        Value value(i, i + 10);
        tree_.Insert(value);
      }
    }
  }

  protected: bool Find(int const key) {
    foreach (BinaryTree::Enum, en, tree()) {
      if (en.Get() == key) {
        return true;
      }
    }
    return false;
  }
};

TEST_F(BinaryTreeTest, Delete) {
  EXPECT_TRUE(Find(15));
  tree().Delete(15);
  EXPECT_EQ(Value(), tree().Find(15));
  EXPECT_FALSE(Find(15));
}

TEST_F(BinaryTreeTest, EmptyTree) {
  BinaryTree empty_tree;
  EXPECT_EQ(Value(), empty_tree.Find(15));
}

TEST_F(BinaryTreeTest, Enum) {
  ArrayList_<Value> list;
  foreach (BinaryTree::Enum, en, tree()) {
    list.Add(en.Get());
  }

  ASSERT_EQ(9, list.Count());
  ASSERT_EQ(Value(0, 10), list.Get(0));
  ASSERT_EQ(Value(10, 20), list.Get(1));
  ASSERT_EQ(Value(20, 30), list.Get(2));
}

TEST_F(BinaryTreeTest, Find) {
  EXPECT_EQ(Value(10, 20), tree().Find(15));
  EXPECT_EQ(Value(), tree().Find(55));
}


TEST_F(BinaryTreeTest, Insert) {
  tree().Insert(Value(50, 60));
  EXPECT_EQ(Value(50, 60), tree().Find(55));
}

} // CommonTest

namespace Common {

inline String Stringify(const CommonTest::Value& value) {
  return String::Format("Value(%d,%d)", value.min_, value.max_);
}

} // Common
