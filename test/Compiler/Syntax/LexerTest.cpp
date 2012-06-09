#include "precomp.h"
// @(#)$Id$
//
// Evita Lexer Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractLexerTest.h"
#include "Common.h"

namespace CompilerTest {

using namespace Compiler;

class LexerTest : public AbstractLexerTest {
  private: Lexer lexer_;

  protected: LexerTest()
    : ALLOW_THIS_IN_INITIALIZER_LIST(
        lexer_(session(), "unittest", *this)) {}

  // [R]
  protected: void Run(const String& text) {
    lexer_.Feed(text.value(), text.length());
    lexer_.Finish();
  }
};

// TODO(yosi) 2011-12-30 NYI: Lexer errors
// TODO(yosi) 2011-12-30 NYI: Float

// Note: source should end with newline.
// Lexer returns null at end of string.
#define NORMAL_CASE(source, box, ...) { \
  Run(source); \
  ASSERT_TRUE(!session().HasError()); \
  TokenBox expected[] = { box, __VA_ARGS__ }; \
  for (auto i = 0; i < ARRAYSIZE(expected); i++) { \
    EXPECT_EQ(expected[i], GetToken()); \
    ASSERT(expected[i].token().ref_count() == 1);\
  } \
  EXPECT_FALSE(HasToken()); \
}

#define ERROR_CASE(source, error, ...) { \
  Run(source); \
  ASSERT_TRUE(session().HasError()); \
  auto actual_tree = GetErrors(); \
  auto expected_tree = MakeErrorTree(error, __VA_ARGS__); \
  EXPECT_EQ(expected_tree, actual_tree); \
}

TEST_F(LexerTest, CommentBlock) {
  NORMAL_CASE("/* comment */ foo", NewToken("foo"));
}

TEST_F(LexerTest, CommentLine) {
  NORMAL_CASE("/* comment */\nfoo", NewToken("foo"));
}

TEST_F(LexerTest, Error_Bad_BackSlash) {
  ERROR_CASE(
    "'\\z'",
    ErrorItem("Lexer_Bad_BackSlash", 0, 3, "z"));
}

TEST_F(LexerTest, Error_Bad_RankSpec) {
  ERROR_CASE(
    "[,2]",
    ErrorItem("Lexer_Bad_RankSpec", 0, 3, "2"));
}

TEST_F(LexerTest, Error_Bad_Real) {
  ERROR_CASE(
    "1ex",
    ErrorItem("Lexer_Bad_Real", 0, 3, "x"));
}

TEST_F(LexerTest, Error_Expect_Digit) {
  ERROR_CASE(
  // 123
    "9.a",
    ErrorItem("Lexer_Expect_Digit", 0, 3, "a"));
}

TEST_F(LexerTest, Error_Expect_IdStart) {
  ERROR_CASE(
  // 12
    "@1",
    ErrorItem("Lexer_Expect_IdStart", 0, 2, "1"));
}

TEST_F(LexerTest, Error_InvalidChar) {
  ERROR_CASE(
  // 12
    " \x1B",
    ErrorItem("Lexer_Invalid_Char", 0, 2, "\x1B"));
}

TEST_F(LexerTest, Error_Unclosed_BlockComment) {
  ERROR_CASE(
  // 1234567
    " /* foo",
    ErrorItem("Lexer_Unclosed_BlockComment", 0, 7, 0, 2));
}

TEST_F(LexerTest, Error_Unclosed_DoubleQuote) {
  ERROR_CASE(
  // 1 2345
    " \"foo",
    ErrorItem("Lexer_Unclosed_DoubleQuote", 0, 5, 0, 2));
}

TEST_F(LexerTest, Error_Unclosed_RankSpec) {
  ERROR_CASE(
  // 1234
    " [,,",
    ErrorItem("Lexer_Unclosed_RankSpec", 0, 4, 0, 2));
}

TEST_F(LexerTest, Error_Unclosed_SingleQuote) {
  ERROR_CASE(
  // 123
    " 'f",
    ErrorItem("Lexer_Unclosed_SingleQuote", 0, 3, 0, 2));
}

TEST_F(LexerTest, Error_Unexpected_Eof) {
  ERROR_CASE(
  // 1234
    " 1e+",
    ErrorItem("Lexer_Unexpected_Eof", 0, 4, 0, 2));
}

TEST_F(LexerTest, Error_Unexpected_Newline) {
  ERROR_CASE(
  // 1 23456
    " \"foo\n"
    " bar\"",
    ErrorItem("Lexer_Unexpected_Newline", 1, 0, 0, 2));
}

TEST_F(LexerTest, LiteralChar) {
  NORMAL_CASE("'x'", NewChar('x'));
}

TEST_F(LexerTest, LiteralCharBsN) {
  NORMAL_CASE("'\n'", NewChar('\n'));
}

TEST_F(LexerTest, LiteralCharBsU) {
  NORMAL_CASE("'\\u1234'", NewChar(0x1234));
}

TEST_F(LexerTest, LiteralCharBsHex) {
  NORMAL_CASE("'\x69'", NewChar('\x69'));
}

TEST_F(LexerTest, LiteralFalse) {
  NORMAL_CASE("false", NewToken(*K_false));
}

TEST_F(LexerTest, LiteralFloat32) {
  NORMAL_CASE("1.5f", NewToken(1.5f));
}

TEST_F(LexerTest, LiteralFloat32e) {
  NORMAL_CASE("1.6e10f", NewToken(1.6e10f));
}

TEST_F(LexerTest, LiteralFloat32em) {
  NORMAL_CASE("1.6e-8f", NewToken(1.6e-8f));
}

TEST_F(LexerTest, LiteralFloat64) {
  NORMAL_CASE("1.5", NewToken(1.5));
}

TEST_F(LexerTest, LiteralFloat64e) {
  NORMAL_CASE("1.6e10", NewToken(1.6e10));
}

TEST_F(LexerTest, LiteralFloat64em) {
  NORMAL_CASE("1.6e-8", NewToken(1.6e-8));
}

TEST_F(LexerTest, LiteralFloat64d) {
  NORMAL_CASE("1.5d", NewToken(1.5));
}

TEST_F(LexerTest, LiteralFloat64ed) {
  NORMAL_CASE("1.6e10d", NewToken(1.6e10));
}

TEST_F(LexerTest, LiteralFloat64emd) {
  NORMAL_CASE("1.6e-8d", NewToken(1.6e-8));
}

TEST_F(LexerTest, LiteralInt32) {
  NORMAL_CASE("123", NewInt(*Ty_Int32, 123));
}

TEST_F(LexerTest, LiteralInt32Hex) {
  NORMAL_CASE("0xBadBeef", NewInt(*Ty_Int32, 0xBadBeef));
}

TEST_F(LexerTest, LiteralInt64) {
  NORMAL_CASE("123l", NewInt(*Ty_Int64, 123));
}

TEST_F(LexerTest, LiteralInt64gHex) {
  NORMAL_CASE("0x123l", NewInt(*Ty_Int64, 0x123));
}

TEST_F(LexerTest, LiteralString) {
  NORMAL_CASE("\"foo\"", NewString("foo"));
}

TEST_F(LexerTest, LiteralBs) {
  NORMAL_CASE(
   "\x22" "\\a"  "\\b"  "\\f"  "\\r"  "\\t"  "\\v"  "\\'"  "\\\""
   "\\\\"  "\x22",
   NewString("\x07\x08\x0C\x0D\x09\x0B'\"\\"));
}

TEST_F(LexerTest, LiteralStringRaw) {
  NORMAL_CASE("@\"f\\oo\"", NewString("f\\oo"));
}

TEST_F(LexerTest, LiteralTrue) {
  NORMAL_CASE("true", NewToken(*K_true));
}

TEST_F(LexerTest, LiteralUInt32) {
  NORMAL_CASE("123u", NewInt(*Ty_UInt32, 123));
}

TEST_F(LexerTest, LiteralUInt32Hex) {
  NORMAL_CASE("0xBadBeefu", NewInt(*Ty_UInt32, 0xBadBeef));
}

TEST_F(LexerTest, LiteralUInt64) {
  NORMAL_CASE("123lu", NewInt(*Ty_UInt64, 123));
  NORMAL_CASE("123ul", NewInt(*Ty_UInt64, 123));
}

TEST_F(LexerTest, LiteralUInt64gHex) {
  NORMAL_CASE("0x123lu", NewInt(*Ty_UInt64, 0x123));
  NORMAL_CASE("0x123ul", NewInt(*Ty_UInt64, 0x123));
}

TEST_F(LexerTest, Name) {
  NORMAL_CASE("foo class @class",
    NewToken("foo"),
    NewToken(*K_class),
    NewToken("class"));
}

TEST_F(LexerTest, OperatorAdd) {
  NORMAL_CASE("+ ++ +=",
    NewToken(*Op__Add),
    NewToken(*Op__Add1),
    NewToken(*Op__AddEq));
}

TEST_F(LexerTest, OperatorAnd) {
  NORMAL_CASE("& && &=",
    NewToken(*Op__LogAnd),
    NewToken(*Op__CondAnd),
    NewToken(*Op__AndEq));
}

TEST_F(LexerTest, OperatorDiv) {
  NORMAL_CASE("/ /=",
    NewToken(*Op__Div),
    NewToken(*Op__DivEq));
}

TEST_F(LexerTest, OperatorEq) {
  NORMAL_CASE("= == !=",
    NewToken(*Op__Assign),
    NewToken(*Op__Eq),
    NewToken(*Op__Ne));
}

TEST_F(LexerTest, OperatorGt) {
  NORMAL_CASE("> >= >> >>=",
    NewToken(*Op__Gt),
    NewToken(*Op__Ge),
    NewToken(*Op__Shr),
    NewToken(*Op__ShrEq));
}

TEST_F(LexerTest, OperatorLt) {
  NORMAL_CASE("< <= << <<=",
    NewToken(*Op__Lt),
    NewToken(*Op__Le),
    NewToken(*Op__Shl),
    NewToken(*Op__ShlEq));
}

TEST_F(LexerTest, OperatorMul) {
  NORMAL_CASE("* *=",
    NewToken(*Op__Mul),
    NewToken(*Op__MulEq));
}

TEST_F(LexerTest, OperatorOr) {
  NORMAL_CASE("| || |=",
    NewToken(*Op__LogIor),
    NewToken(*Op__CondOr),
    NewToken(*Op__LogIorEq));
}

TEST_F(LexerTest, OperatorSub) {
  NORMAL_CASE("- -- -=",
    NewToken(*Op__Sub),
    NewToken(*Op__Sub1),
    NewToken(*Op__SubEq));
}

TEST_F(LexerTest, RankSpec) {
  NORMAL_CASE(
    "[ [] [,] [,,]",
    NewToken(*Op__OpenBracket),
    NewRank(1),
    NewRank(2),
    NewRank(3));
}

} // CompilerTest
