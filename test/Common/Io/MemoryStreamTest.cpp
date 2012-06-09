#include "precomp.h"
// @(#)$Id$
//
// Evita - MemoryStreamTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Common.h"
#include "Common/Io.h"

namespace CommonTest {

using namespace Common;
using namespace Common::Io;

class MemoryStreamTest : public ::testing::Test {
};

TEST_F(MemoryStreamTest, Basic) {
  MemoryStream mem;

  char const sz[] = "abcd";
  ASSERT_EQ(sizeof(sz), mem.Write(sz, sizeof(sz)));

  char buf[sizeof(sz)];
  ASSERT_EQ(sizeof(sz), mem.Read(buf, sizeof(buf)));
  EXPECT_STREQ(sz, buf);
  EXPECT_EQ(0, mem.Read(buf, sizeof(buf)));
  EXPECT_EQ(0, mem.Read(buf, sizeof(buf)));
}

TEST_F(MemoryStreamTest, Grow) {
  MemoryStream mem(10);

  auto const M = 5;
  auto const N = 100;
  
  for (int i = 0; i < N; i++) {
    char sz[M];
    ::FillMemory(sz, sizeof(sz), static_cast<uint8>(i + 'A'));
    ASSERT_EQ(sizeof(sz), mem.Write(sz, sizeof(sz)));
  }

  char buf[M * N];
  ::FillMemory(buf, sizeof(buf), '*');
  ASSERT_EQ(sizeof(buf), mem.Read(buf, sizeof(buf)));
  auto p = buf;
  for (int i = 0; i < N; i++) {
    char sz[M];
    ::FillMemory(sz, sizeof(sz), static_cast<uint8>(i + 'A'));
    for (int j = 0; j < 5; j++) {
      EXPECT_EQ(sz[j], *p);
      p++;
    }
  }
  EXPECT_EQ(0, mem.Read(buf, 1));
}

}  // CommonTest
