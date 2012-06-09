#include "precomp.h"
// @(#)$Id$
//
// Evita Lexer Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Compiler/Syntax/Keyword.h"
#include "Compiler/Syntax/Lexer.h"
#include "Compiler/Syntax/Operator.h"
#include "Compiler/Syntax/RankSpec.h"
#include "Compiler/Syntax/TypeArgsLexer.h"
#include "Compiler/Syntax/TypeArgList.h"
#include "Common.h"

#include "./LexerChecker.h"

using namespace Compiler;

class TypeArgsLexerTest : public Compiler::Testing::LexerTest {
};

#define MyLexer TypeArgsLexer

// foo;
TEST_F(TypeArgsLexerTest, NameOnly) {
  RUN_LEXER(foo_, Op__SemiColon);
  RUN_CHECKER(foo_, Op__SemiColon);
}

// foo<bar>;
TEST_F(TypeArgsLexerTest, TypeArgs1) {
  RUN_LEXER(foo_, Op__Lt, bar_, Op__Gt, Op__SemiColon);

  Type* tyargv[] = {
    &UnresolvedType::New(bar_),
  };
  Type::Array tyargs(tyargv, lengthof(tyargv));

  RUN_CHECKER(
    foo_,
    new TypeArgList(tyargs),
    Op__SemiColon);
}

// foo<bar, baz>
TEST_F(TypeArgsLexerTest, TypeArgs2) {
  RUN_LEXER(foo_, Op__Lt, bar_, Op__Comma, baz_, Op__Gt, Op__SemiColon);

  Type* tyargv[] = {
    &UnresolvedType::New(bar_),
    &UnresolvedType::New(baz_),
  };
  Type::Array tyargs(tyargv, lengthof(tyargv));

  RUN_CHECKER(
    foo_,
    new TypeArgList(tyargs),
    Op__SemiColon);
}

// foo<bar[]>;
TEST_F(TypeArgsLexerTest, TypeArgs3) {
  RUN_LEXER(foo_, Op__Lt, bar_, new RankSpec(1), Op__Gt, Op__SemiColon);

  const Type* tyargv[] = {
    &ArrayType::Intern(UnresolvedType::New(bar_), 1),
  };
  Type::Array tyargs(tyargv, lengthof(tyargv));

  RUN_CHECKER(
    foo_,
    new TypeArgList(tyargs),
    Op__SemiColon);
}
