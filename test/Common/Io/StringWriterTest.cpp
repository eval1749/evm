#include "precomp.h"
// @(#)$Id$
//
// Evita - StringWriterTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../CommonTest.h"

#include "Common/Io.h"

namespace CommonTest {

using namespace Common;
using namespace Common::Io;

class StringWriterTest : public ::testing::Test {
};

TEST_F(StringWriterTest, WriteChar) {
  StringWriter buf;
  buf.Write('a');
  buf.Write(static_cast<char16>(0x3032));
  char16 const expected[] = { 'a', 0x3032, 0 };
  EXPECT_STREQ(expected, buf.ToString().value());
}

TEST_F(StringWriterTest, WriteFormatC) {
  StringWriter buf;
  buf.Write("%c", 'a');
  EXPECT_STREQ(L"a", buf.ToString().value());
}

TEST_F(StringWriterTest, WriteFormatD) {
  StringWriter buf;
  buf.Write("%d %4d %-4d %04d %+d", 12, 34, 56, 78, 90);
  EXPECT_STREQ(L"12 34     56 0078 +90", buf.ToString().value());
}

TEST_F(StringWriterTest, WriteFormatD2) {
  StringWriter buf;
  buf.Write("%d %4d %-4d %04d %+d", -12, -34, -56, -78, -90);
  EXPECT_STREQ(L"-12 -34   -56 0-78 -90", buf.ToString().value());
}

TEST_F(StringWriterTest, WriteFormatR) {
  StringWriter buf;
  buf.Write("%r %R", 12345, 12345);
  EXPECT_STREQ(L"9ix 9IX", buf.ToString().value());
}

TEST_F(StringWriterTest, WriteFormatS) {
  StringWriter buf;
  buf.Write("%s %5s %-5s", "foo", "bar", "baz");
  EXPECT_STREQ(L"foo bar     baz", buf.ToString().value());
}

TEST_F(StringWriterTest, WriteFormatU) {
  StringWriter buf;
  buf.Write("%u %d", -1234, static_cast<uint32>(-1234));
  // Note: first one is static_cast<int64>(-1234)
  EXPECT_STREQ(L"18446744073709550382 4294966062", buf.ToString().value());
}

TEST_F(StringWriterTest, WriteFormatX) {
  StringWriter buf;
  buf.Write("%x %X", 0xBEEF, 0xFEED);
  EXPECT_STREQ(L"beef FEED", buf.ToString().value());
}

TEST_F(StringWriterTest, WriteString) {
  StringWriter buf;
  buf.Write("abc");
  buf.Write(L"XYZ");
  EXPECT_STREQ(L"abcXYZ", buf.ToString().value());
}

}  // CommonTest
