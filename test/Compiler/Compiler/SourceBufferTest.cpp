#include "precomp.h"
// @(#)$Id$
//
// Evita Lexer Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCompilerTest.h"

#include "Compiler/SourceBuffer.h"

namespace CompilerTest {

using namespace Compiler;

class SourceBufferTest : public ::testing::Test {
};

TEST_F(SourceBufferTest, CrLf) {
  const char16 text[] = L"line1\r\nline2\r\nline3\r\n";
  SourceBuffer buffer;
  buffer.Add(text, ::lstrlen(text));
  EXPECT_EQ(String("line2"), buffer.GetLine(1));
}

TEST_F(SourceBufferTest, Cr) {
  const char16 text[] = L"line1\rMore\nline2\r\nline3\r\n";
  SourceBuffer buffer;
  buffer.Add(text, ::lstrlen(text));
  EXPECT_EQ(String("line2"), buffer.GetLine(1));
}

TEST_F(SourceBufferTest, Lf) {
  const char16 text[] = L"line1\nline2\nline3\n";
  SourceBuffer buffer;
  buffer.Add(text, ::lstrlen(text));
  EXPECT_EQ(String("line2"), buffer.GetLine(1));
}

} // CompilerTest
