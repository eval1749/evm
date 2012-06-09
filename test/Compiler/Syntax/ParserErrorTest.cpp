#include "precomp.h"
// @(#)$Id$
//
// Evita Lexer Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractParserTest.h"

#include "Compiler/Syntax/Parser.h"

namespace CompilerTest {

using namespace Compiler;

class ParserErrorTest : public AbstractParserTest {
  protected: ErrorList ParseWithError(const String& source) {
    parser().Parse(source.value(), source.length());
    parser().Finish();
    return GetErrors();
  }

  protected: ErrorList ParseStmtWithError(const String& source) {
    auto whole = String::Format(
        "class Foo {\nvoid Main() {\n%s\n}\n}\n",
        source);
    return ParseWithError(whole);
  }
};

#define RUN_PARSER_FOR_STATEMENT(stmts) \
    RUN_PARSER("class Foo {\nvoid Main() {\n" stmts "\n}\n}\n")

TEST_F(ParserErrorTest, Parse_Class_Redefined_Class) {
  auto actual = ParseWithError("class A {} class A {}");
  auto expected = ErrorItem("Parse_Class_Redefined_Class", 0, 18, "A");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Class_Redefined_Different) {
  auto actual = ParseWithError("namespace A {} class A {}");
  auto expected = ErrorItem("Parse_Class_Redefined_Different", 0, 22, "A");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Expr_NewArray_Invalid) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  char[] chars = new char[];\n"
    "}");
  auto expected = ErrorItem(
    "Parse_Expr_NewArray_Invalid", 1, 28,
    "Array creation expression requires array size or initializer.");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Field_Const_NoValue) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  const int x;\n"
    "}");
  auto expected = ErrorItem("Parse_Field_Const_NoValue", 1, 13, "x");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Field_Modifier_Invalid) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  virtual int x;\n"
    "}");
  auto expected = ErrorItem("Parse_Field_Modifier_Invalid", 1, 3,
                            "Keyword(virtual)");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Field_Value_Void) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  void x;\n"
    "}");
  auto expected = ErrorItem("Parse_Field_Value_Void", 1, 8, "x");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Member_Duplicate) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public bool this[int index] { get; get; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Indexer_Member_Duplicate", 1, 38,
                            "Item", "get");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Member_Invalid) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public bool this[int index] { foo; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Indexer_Member_Invalid", 1, 33, "foo");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Member_Missing_Body) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public bool this[int index] { set; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Indexer_Member_Missing_Body", 1, 33,
                            "set");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Member_None) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public bool this[int index] {} }\n"
    "}");
  auto expected = ErrorItem("Parse_Indexer_Member_None", 1, 15, "Item");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Modifier_Invalid) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public static bool this[int index] {} }\n"
    "}");
  auto expected = ErrorItem("Parse_Indexer_Modifier_Invalid", 1, 10,
                            "Keyword(static)");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Param_Duplicate) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public int this[int x, int x] { get { return 1; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Indexer_Param_Duplicate", 1, 30, "x");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Param_Missing) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public int this[] { get { return 1; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Indexer_Param_Missing", 1, 18, "[]");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Param_Reserved) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public int this[int value] { get { return value; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Indexer_Param_Reserved", 1, 23, "value");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Param_Void) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public int this[void x] { get { return 1; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Indexer_Param_Void", 1, 24, "x");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Indexer_Value_Void) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public void this[int x] { get { return 1; } }\n"
    "}");
  auto expected = ErrorItem(
      "Parse_Indexer_Value_Void", 1, 15,
      "Item");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Method_Param_Duplicate) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public void Bar(int x, int x) {}\n"
    "}");
  auto expected = ErrorItem("Parse_Method_Param_Duplicate", 1, 30, "x");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Method_Param_Void) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public void Bar(void x) {}\n"
    "}");
  auto expected = ErrorItem("Parse_Method_Param_Void", 1, 24, "x");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Operator_ParenNotClosed) {
  auto actual = ParseWithError(
      "namespace Foo {\n"
      "  class Bar {}\n");
  EXPECT_TRUE(parser().NeedMoreFeed());
  auto expected = ErrorItem("Parse_Operator_ParenNotClosed", 0, 15, "{");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Property_Member_Duplicate) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public int State { get; get; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Property_Member_Duplicate", 1, 27,
                            "State", "get");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Property_Member_Invalid) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public int State { foo; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Property_Member_Invalid", 1, 22, "foo");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Property_Member_Missing_Body) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public int State { set; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Property_Member_Missing_Body", 1, 22,
                            "set");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Property_Member_None) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public int State {} }\n"
    "}");
  auto expected = ErrorItem("Parse_Property_Member_None", 1, 14, "State");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Property_Value_Void) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public void State { get { return 1; } }\n"
    "}");
  auto expected = ErrorItem("Parse_Property_Value_Void", 1, 15, "State");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Stmt_Return_Expr) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public void Bar() { return 1; }\n"
    "}");
  auto expected = ErrorItem("Parse_Expect", 1, 30, "1", ";");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Stmt_Return_Void) {
  auto actual = ParseWithError(
    "class Foo {\n"
    "  public int Bar() { return; }\n"
    "}");
  auto expected = ErrorItem("Parse_Stmt_Return_Void", 1, 28, ";");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Stmt_Using_Void) {
  auto actual = ParseStmtWithError("using (void x = 1) {}");
  auto expected = ErrorItem("Parse_Stmt_Using_Void", 2, 13, "x");
  EXPECT_EQ(expected, actual);
}

TEST_F(ParserErrorTest, Parse_Unexpected_Eof) {
  auto actual = ParseWithError(
      "namespace\n"
      "Foo\n");
  EXPECT_TRUE(parser().NeedMoreFeed());
  auto expected = ErrorItem("Parse_Unexpected_Eof", 0, 1, "Namespace");
  EXPECT_EQ(expected, actual);
}

} // CompilerTest
