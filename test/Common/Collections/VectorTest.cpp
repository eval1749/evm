#include "precomp.h"
// @(#)$Id$
//
// Evita Stack Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCommonTest.h"

#include "Common/Collections/Vector_.h"

namespace CommonTest {

using namespace Common::Collections;

class VectorTest : public AbstractCommonTest {
};

TEST_F(VectorTest, Ctor) {
  Vector_<int> v(3);
  EXPECT_EQ(3, v.size());

  auto i = 0;
  for (auto it = v.begin(); it != v.end(); ++it) {
    *it = i;
    EXPECT_EQ(i, v[i]);
    ++i;
  }
  EXPECT_EQ(v.size(), i);

  Vector_<int> u(v);
  auto j = 0;
  auto vit = const_cast<const Vector_<int>&>(v).begin();
  for (auto it = u.begin(); it != u.end(); ++it) {
    EXPECT_EQ(*vit, *it);
    EXPECT_EQ(*it, j);
    ++j;
    ++vit;
  }
}

TEST_F(VectorTest, CopyTo) {
  Vector_<int> v(3);
  for (auto i = 0; i < v.size(); i++) {
    v[i] = i;
  }
  Vector_<int> u(5);
  v.CopyTo(u, 2);
  EXPECT_EQ(0, u[2]);
  EXPECT_EQ(1, u[3]);
  EXPECT_EQ(2, u[4]);
}

TEST_F(VectorTest, ReverseIterator) {
  Vector_<int> v(3);
  for (auto i = 0; i < v.size(); i++) {
    v[i] = i;
  }
  auto it = const_cast<const Vector_<int>&>(v).rbegin();
  EXPECT_EQ(2, *it);
  ++it;
  EXPECT_EQ(1, *it);
  ++it;
  EXPECT_EQ(0, *it);
  ++it;
  EXPECT_EQ(v.rend(), it);
}

TEST_F(VectorTest, Subvector) {
  Vector_<int> v(5);
  for (auto i = 0; i < v.size(); i++) {
    v[i] = i;
  }

  auto u = Vector_<int>(v.begin() + 1, v.begin() + 3);
  EXPECT_EQ(2, u.size());
  EXPECT_EQ(1, u[0]);
  EXPECT_EQ(2, u[1]);
}

} // CommonTest
