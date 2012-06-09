#include "precomp.h"
// @(#)$Id$
//
// Evita Lexer Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractLexerTest.h"

#include "Compiler/Syntax/Keyword.h"
#include "Compiler/Syntax/Lexer.h"
#include "Compiler/Syntax/Operator.h"
#include "Compiler/Syntax/NameLexer.h"
#include "Common.h"

namespace CompilerTest {

using namespace Compiler;

class NameLexerTest : public AbstractLexerTest {
  private: NameLexer lexer_;

  protected: NameLexerTest()
    : ALLOW_THIS_IN_INITIALIZER_LIST(lexer_(*this)) {}

  protected: void Feed(TokenBox box) {
    lexer_.ProcessToken(box.token());
  }
};

#define FEED(box, ...) { \
  TokenBox v[] = { box, __VA_ARGS__, NewToken(*Op__SemiColon) }; \
  for (auto i = 0; i < ARRAYSIZE(v); i++) { \
    Feed(v[i]); \
  } \
}

#define VERIFY(box, ...) { \
  TokenBox expected[] = { box, __VA_ARGS__, NewToken(*Op__SemiColon) }; \
  for (auto i = 0; i < ARRAYSIZE(expected); i++) { \
    EXPECT_EQ(expected[i], GetToken()); \
    ASSERT(expected[i].token().ref_count() == 1);\
  } \
  EXPECT_FALSE(HasToken()); \
}

TEST_F(NameLexerTest, Array) { // foo[]
  FEED(NewToken("foo"), NewRank(1));
  VERIFY(NewToken(NewArrayNameRef("foo", 1)));
}

TEST_F(NameLexerTest, Array2) { // foo[][,]
  FEED(NewToken("foo"), NewRank(1), NewRank(2));
  VERIFY(NewToken(NewArrayNameRef("foo", 1, 2)));
}

TEST_F(NameLexerTest, CompositeName) { // foo<A>
  FEED(NewToken("foo"), NewToken(*Op__Lt), NewToken("A"), NewToken(*Op__Gt));
  VERIFY(NewToken(NewCompositeName("foo", "A")));
}

TEST_F(NameLexerTest, CompositeNameArray) { // foo<A>[]
  FEED(
    NewToken("foo"),
        NewToken(*Op__Lt), NewToken("A"), NewToken(*Op__Gt),
    NewRank(1));
  Vector_<const NameRef::Item*> items(1);
  items[0] = &NewCompositeName("foo", "A");
  Vector_<int> ranks(1);
  ranks[0] = 1;
  VERIFY(NewToken(*new NameRef(items, ranks)));
}

TEST_F(NameLexerTest, CompositeNameArrayArg) { // foo<A>
  FEED(
    NewToken("foo"),
    NewToken(*Op__Lt),
        NewToken("A"), NewRank(1),
    NewToken(*Op__Gt));
  Vector_<const NameRef::Item*> items(1);
  items[0] = &NewCompositeName("foo", NewArrayNameRef("A", 1));
  VERIFY(NewToken(*new NameRef(items)));
}

TEST_F(NameLexerTest, CompositeNameDot) { // foo.bar<A, B>
  FEED(
    NewToken("foo"),
    NewToken(*Op__Dot),
    NewToken("bar"), 
    NewToken(*Op__Lt),
        NewToken("A"),
        NewToken(*Op__Comma),
        NewToken("B"),
    NewToken(*Op__Gt));
  VERIFY(NewToken(NewSimpleName("foo"), NewCompositeName("bar", "A", "B")));
}

TEST_F(NameLexerTest, CompositeNameNest) { // foo<bar<A>>
  FEED(
    NewToken("foo"),
    NewToken(*Op__Lt),
        NewToken("bar"),
        NewToken(*Op__Lt),
            NewToken("A"),
    NewToken(*Op__Shr));
  VERIFY(NewToken(NewCompositeName("foo", NewCompositeName("bar", "A"))));
}

TEST_F(NameLexerTest, CompositeNameNest2) { // foo<bar<baz<A>>>
  FEED(
    NewToken("foo"),
    NewToken(*Op__Lt),
        NewToken("bar"),
        NewToken(*Op__Lt),
            NewToken("baz"),
            NewToken(*Op__Lt),
                NewToken("A"),
        NewToken(*Op__Shr),
    NewToken(*Op__Gt));
  VERIFY(
    NewToken(
        NewCompositeName("foo",
            NewCompositeName("bar",
                NewCompositeName("baz", "A")))));
}

TEST_F(NameLexerTest, CompositeNameNest3) { // foo<bar<baz<quux<A>>>>
  FEED(
    NewToken("foo"),
    NewToken(*Op__Lt),
        NewToken("bar"),
        NewToken(*Op__Lt),
            NewToken("baz"),
            NewToken(*Op__Lt),
                NewToken("quux"),
                NewToken(*Op__Lt),
                    NewToken("A"),
            NewToken(*Op__Shr),
    NewToken(*Op__Shr));
  VERIFY(
    NewToken(
        NewCompositeName("foo",
            NewCompositeName("bar",
                NewCompositeName("baz",
                    NewCompositeName("quux", "A"))))));
}

TEST_F(NameLexerTest, Name) {
  FEED(NewToken("foo"));
  VERIFY(NewToken("foo"));
}

TEST_F(NameLexerTest, NameDotName) {
  FEED(NewToken("foo"), NewToken(*Op__Dot), NewToken("bar"));
  VERIFY(NewToken("foo", "bar"))
}

TEST_F(NameLexerTest, NameDotNameDotName) { // foo.bar.baz
  FEED(
    NewToken("foo"),
    NewToken(*Op__Dot),
    NewToken("bar"),
    NewToken(*Op__Dot),
    NewToken("baz"));
  VERIFY(NewToken("foo", "bar", "baz"));
}

TEST_F(NameLexerTest, NameDotNameRank) { // foo.bar[,]
  FEED(
    NewToken("foo"),
    NewToken(*Op__Dot),
    NewToken("bar"),
    NewRank(2));

  Vector_<const NameRef::Item*> items(2);
  items[0] = &NewSimpleName("foo");
  items[1] = &NewSimpleName("bar");

  Vector_<int> ranks(1);
  ranks[0] = 2;

  VERIFY(NewToken(*new NameRef(items, ranks)));
}

TEST_F(NameLexerTest, NameAndCompositeName) { // T Action<T>
  FEED(
    NewToken("T"),
    NewToken("Action"),
    NewToken(*Op__Lt),
    NewToken("T"),
    NewToken(*Op__Gt));

  VERIFY(NewToken("T"), NewToken(NewCompositeName("foo", "T")));
}

TEST_F(NameLexerTest, NotCompositeName) { // foo < A
  FEED(NewToken("foo"), NewToken(*Op__Lt), NewToken("A"));
  VERIFY(NewToken("foo"), NewToken(*Op__Lt), NewToken("A"));
}

} // CompilerTest
