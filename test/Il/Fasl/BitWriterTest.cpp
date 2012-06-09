#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Common/Io.h"
#include "Il.h"
#include "Il/Fasl/BitWriter.h"

namespace IlTest {

using namespace Il::Fasl;
using Common::Io::MemoryStream;

class BitWriterTest : public ::testing::Test {
};

TEST_F(BitWriterTest, Basic1) {
  MemoryStream mem;
  BitWriter writer(mem);
  writer.Write1(1);
  ASSERT_EQ(1, writer.total_bits());
  writer.Flush();

  uint8 buf[1];
  ASSERT_EQ(1, mem.Read(buf, 1));
  EXPECT_EQ(0x80, buf[0]);
  EXPECT_EQ(0, mem.Read(buf, 1));
}

TEST_F(BitWriterTest, Basci2) {
  MemoryStream mem;
  BitWriter writer(mem);
  writer.Write1(1);     // 0001
  writer.Write5(21);    // 10101
  ASSERT_EQ(6, writer.total_bits());
  writer.Flush();

  uint8 buf[1];
  ASSERT_EQ(1, mem.Read(buf, 1));
  EXPECT_EQ(0xD4, buf[0]); // 1101 0100
  EXPECT_EQ(0, mem.Read(buf, 1)); // Check EOF
}

TEST_F(BitWriterTest, Basci3) {
  MemoryStream mem;
  BitWriter writer(mem);
  writer.Write1(1);
  writer.Write5(30);
  writer.Write6(60);
  writer.Write7(120);
  writer.Write8(250);
  ASSERT_EQ(27, writer.total_bits());
  writer.Flush();

  uint8 buf[4]; // 4 = ceil(27, 8)
  ASSERT_EQ(4, mem.Read(buf, 4));
  EXPECT_EQ(0xFB, buf[0]);
  EXPECT_EQ(0xCF, buf[1]);
  EXPECT_EQ(0x1F, buf[2]);
  EXPECT_EQ(0x40, buf[3]);
  EXPECT_EQ(0, mem.Read(buf, 1)); // Check EOF
}

TEST_F(BitWriterTest, Flush) {
  MemoryStream mem;
  BitWriter writer(mem, 10);

  for (int i = 0; i < 100; i++) {
    writer.Write5(42);
  }

  ASSERT_EQ(5 * 100, writer.total_bits());
  writer.Flush();

  int const num_bytes = (500 + 7) / 8;
  uint8 buf[num_bytes];
  EXPECT_EQ(num_bytes, mem.Read(buf, num_bytes));
  EXPECT_EQ(0, mem.Read(buf, 1)); // Check EOF
}

}  // IlTest
