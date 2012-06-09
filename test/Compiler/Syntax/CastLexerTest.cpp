#include "precomp.h"
// @(#)$Id$
//
// Evita Lexer Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "Compiler/Syntax/CastOperator.h"
#include "Compiler/Syntax/Keyword.h"
#include "Compiler/Syntax/Lexer.h"
#include "Compiler/Syntax/Operator.h"
#include "Compiler/Syntax/RankSpec.h"
#include "Compiler/Syntax/CastLexer.h"
#include "Common.h"

#include "./LexerChecker.h"

using namespace Compiler;

class CastLexerTest : public Compiler::Testing::LexerTest {
  protected: CastOperator* cast_foo_;

  protected: CastLexerTest()
    : cast_foo_(new CastOperator(UnresolvedType::New(foo_))) {
  }
};

class MyCastLexer : public CastLexer {
  private: TypeNameResolverStub resolver_;

  public: MyCastLexer(TokenProcessor* const processor)
    : CastLexer(processor, &resolver_) {
  }
};

#define MyLexer MyCastLexer

// (foo) bar : CastOperator(foo) Name(bar)
TEST_F(CastLexerTest, Cast) {
  RUN_LEXER(
    Op__OpenParen,
    foo_,
    Op__CloseParen,
    bar_,
    Op__SemiColon);

  RUN_CHECKER(cast_foo_, bar_, Op__SemiColon);
}

// (foo)(-bar) is cast expression
TEST_F(CastLexerTest, Cast2) {
  RUN_LEXER(
    Op__OpenParen,
    foo_,
    Op__CloseParen,
    Op__OpenParen,
    Op__Sub,
    bar_,
    Op__CloseParen,
    Op__SemiColon);

  RUN_CHECKER(
    cast_foo_,
    Op__OpenParen,
    Op__Sub,
    bar_,
    Op__CloseParen,
    Op__SemiColon);
}

// (foo) + bar : '(' Name(foo) ')' '+' Name(bar)
TEST_F(CastLexerTest, NotCast) {
  RUN_LEXER(
    Op__OpenParen,
    foo_,
    Op__CloseParen,
    Op__Add,
    bar_,
    Op__SemiColon);

  RUN_CHECKER(
    Op__OpenParen,
    foo_,
    Op__CloseParen,
    Op__Add,
    bar_,
    Op__SemiColon);
}

// (foo) is bar : '(' Name(foo) ')' is Name(bar)
TEST_F(CastLexerTest, NotCast2) {
  RUN_LEXER(
    Op__OpenParen,
    foo_,
    Op__CloseParen,
    K_is,
    bar_,
    Op__SemiColon);

  RUN_CHECKER(
    Op__OpenParen,
    foo_,
    Op__CloseParen,
    K_is,
    bar_,
    Op__SemiColon);
}

// (foo)-bar is not cast expression
TEST_F(CastLexerTest, NotCast3) {
  RUN_LEXER(
    Op__OpenParen,
    foo_,
    Op__CloseParen,
    Op__Sub,
    bar_,
    Op__SemiColon);

  RUN_CHECKER(
    Op__OpenParen,
    foo_,
    Op__CloseParen,
    Op__Sub,
    bar_,
    Op__SemiColon);
}
