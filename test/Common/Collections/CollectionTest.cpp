#include "precomp.h"
// @(#)$Id$
//
// Evita String Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCommonTest.h"
#include "Common/Collections.h"

namespace Common {
namespace Collections {
int ComputeHashCode(int x) { return x; }
} // Collections
} // Common

namespace CommonTest {

using namespace Common;
using namespace Common::Collections;

class CollectionTest : public AbstractCommonTest {
  protected: CollectionV_<int> const c1_;
  protected: CollectionV_<int> const c2_;
  protected: CollectionV_<int> const c3_;
  protected: CollectionV_<int> const c4_;

  protected: CollectionTest()
    : c1_('A', 'B', 'C'),
      c2_('A', 'B', 'C'),
      c3_('X', 'Y') {}

  DISALLOW_COPY_AND_ASSIGN(CollectionTest);
};

TEST_F(CollectionTest, Ctor) {
  int const v[] = { 'A', 'B', 'C' };
  Collection_<int> c1(v, ARRAYSIZE(v));
  EXPECT_EQ(ARRAYSIZE(v), c1.Count());
}

TEST_F(CollectionTest, CtorArray) {
  int const v[] = { 'A', 'B', 'C' };
  Array_<int> arr(v, ARRAYSIZE(v));
  Collection_<int> c1(arr);
  EXPECT_EQ(ARRAYSIZE(v), c1.Count());
}

TEST_F(CollectionTest, CtorArrayList) {
  int const v[] = { 'A', 'B', 'C' };
  Array_<int> arr(v, ARRAYSIZE(v));
  ArrayList_<int> list(arr);
  Collection_<int> c1(list);
  EXPECT_EQ(ARRAYSIZE(v), c1.Count());
}

TEST_F(CollectionTest, CtorHashMap) {
  HashMap_<int, int> map;
  map.Add(1, 'A');
  map.Add(2, 'B');
  map.Add(3, 'C');

  Collection_<int> c1(map);
  EXPECT_EQ(map.Count(), c1.Count());
}

TEST_F(CollectionTest, CtorHashSet) {
  int const v[] = { 'A', 'B', 'C' };
  Collection_<int> c1(v, ARRAYSIZE(v));
  EXPECT_EQ(ARRAYSIZE(v), c1.Count());

  HashSet_<int> set(c1);
  EXPECT_EQ(ARRAYSIZE(v), set.Count());

  Collection_<int> c12(set);
  EXPECT_EQ(ARRAYSIZE(v), c12.Count());
}

TEST_F(CollectionTest, Enum) {
  int const v[] = { 'A', 'B', 'C' };
  Collection_<int> c1(v, ARRAYSIZE(v));
  auto count = 0;
  foreach (Collection_<int>::Enum, elems, c1) {
    ++count;
  }
  EXPECT_EQ(ARRAYSIZE(v), count);
}

TEST_F(CollectionTest, IsEmpty) {
  EXPECT_FALSE(c1_.IsEmpty());
  EXPECT_FALSE(c2_.IsEmpty());
  EXPECT_FALSE(c3_.IsEmpty());
  Collection_<int> empty;
  EXPECT_TRUE(empty.IsEmpty());
}

TEST_F(CollectionTest, OperatorEq) {
  EXPECT_TRUE(c1_ == c1_);
  EXPECT_TRUE(c1_ == c2_);
  EXPECT_FALSE(c1_ == c3_);
  EXPECT_FALSE(c1_ == c4_);
}

TEST_F(CollectionTest, OperatorGt) {
  EXPECT_FALSE(c1_ > c1_);
  EXPECT_FALSE(c1_ > c2_);
  EXPECT_FALSE(c1_ > c3_);
}

TEST_F(CollectionTest, OperatorGe) {
  EXPECT_TRUE(c1_ >= c1_);
  EXPECT_TRUE(c1_ >= c2_);
  EXPECT_FALSE(c1_ >= c3_);
}

TEST_F(CollectionTest, OperatorLt) {
  EXPECT_FALSE(c1_ < c1_);
  EXPECT_FALSE(c1_ < c2_);
  EXPECT_TRUE(c1_ < c3_);
}

TEST_F(CollectionTest, OperatorLe) {
  EXPECT_TRUE(c1_ <= c1_);
  EXPECT_TRUE(c1_ <= c2_);
  EXPECT_TRUE(c1_ <= c3_);
}

TEST_F(CollectionTest, OperatorNe) {
  EXPECT_FALSE(c1_ != c1_);
  EXPECT_FALSE(c1_ != c2_);
  EXPECT_TRUE(c1_ != c3_);
}

TEST_F(CollectionTest, ToString) {
  EXPECT_EQ(String("{65, 66, 67}"), c1_.ToString());
}

}  // CommonTest
