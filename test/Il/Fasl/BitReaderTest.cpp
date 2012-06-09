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
#include "Il/Fasl/BitReader.h"

namespace IlTest {

using namespace Il::Fasl;
using Common::Io::MemoryStream;

class BitReaderTest : public ::testing::Test {
};

TEST_F(BitReaderTest, Basic1) {
  BitReader reader;

  uint8 sample[] = { 0x35 }; // 0011 0101
  reader.Feed(sample, sizeof(sample));

  EXPECT_EQ(0, reader.Read(1));
  EXPECT_EQ(0, reader.Read(1));
  EXPECT_EQ(1, reader.Read(1));
  EXPECT_EQ(1, reader.Read(1));
  EXPECT_EQ(0, reader.Read(1));
  EXPECT_EQ(1, reader.Read(1));
  EXPECT_EQ(0, reader.Read(1));
  EXPECT_EQ(1, reader.Read(1));

  EXPECT_EQ(-1, reader.Read(1)); // check EOF
}

TEST_F(BitReaderTest, Basic2) {
  BitReader reader;

  uint8 sample[] = { 0x35, 0x97 }; // 0011 0101 1001 0111
  reader.Feed(sample, sizeof(sample));

  EXPECT_EQ(3, reader.Read(4));
  EXPECT_EQ(5, reader.Read(4));
  EXPECT_EQ(9, reader.Read(4));
  EXPECT_EQ(7, reader.Read(4));

  EXPECT_EQ(-1, reader.Read(1)); // check EOF
}

TEST_F(BitReaderTest, Crossing) {
  BitReader reader;

  uint8 sample[] = { 0x35, 0x97 }; // 0011 0101 1001 0111
  reader.Feed(sample, sizeof(sample));

  EXPECT_EQ(3, reader.Read(4));
  EXPECT_EQ(0x59, reader.Read(8));
  EXPECT_EQ(7, reader.Read(4));

  EXPECT_EQ(-1, reader.Read(1)); // check EOF
}

}  // IlTest
