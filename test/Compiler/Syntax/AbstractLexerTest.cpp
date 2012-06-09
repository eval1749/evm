#include "precomp.h"
// @(#)$Id$
//
// Evita Lexer Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractLexerTest.h"

namespace CompilerTest {

AbstractLexerTest::~AbstractLexerTest() {
  DEBUG_PRINTF("%p\n", this);
  while (!tokens_.IsEmpty()) {
    auto& token = *tokens_.Take();
    ASSERT(token.ref_count() == 1);
    token.Release();
  }
}

  // [P]
void AbstractLexerTest::ProcessToken(const Token& token) {
  tokens_.Give(&token);
  token.AddRef();
}

// [N]
const NameRef& AbstractLexerTest::NewArrayNameRef(
    const String& name,
    int rank) {
  Vector_<const NameRef::Item*> items(1);
  items[0] = &NewSimpleName(name);
  Vector_<int> ranks(1);
  ranks[0] = rank;
  return *new NameRef(items, ranks);
}

const NameRef& AbstractLexerTest::NewArrayNameRef(
    const String& name,
    int rank1,
    int rank2) {
  Vector_<const NameRef::Item*> items(1);
  items[0] = &NewSimpleName(name);
  Vector_<int> ranks(2);
  ranks[0] = rank1;
  ranks[1] = rank2;
  return *new NameRef(items, ranks);
}

TokenBox AbstractLexerTest::NewChar(char16 const code) {
  return TokenBox(*new CharToken(SourceInfo("unittest", 0, 0), code));
}

const NameRef::CompositeName& AbstractLexerTest::NewCompositeName(
    const String& name,
    const String& arg1) {
  Vector_<const NameRef*> args(1);
  args[0] = new NameRef(Name::Intern(arg1), SourceInfo("unittest", 1, 1));
  return *new NameRef::CompositeName(
    Name::Intern(name),
    args,
    SourceInfo("unittest", 1, 3));
}

const NameRef::CompositeName& AbstractLexerTest::NewCompositeName(
    const String& name,
    const String& arg1,
    const String& arg2) {
  Vector_<const NameRef*> args(2);
  args[0] = new NameRef(Name::Intern(arg1), SourceInfo("unittest", 1, 1));
  args[1] = new NameRef(Name::Intern(arg2), SourceInfo("unittest", 1, 2));
  return *new NameRef::CompositeName(
    Name::Intern(name),
    args,
    SourceInfo("unittest", 1, 3));
}

const NameRef::CompositeName& AbstractLexerTest::NewCompositeName(
    const String& name,
    const NameRef::Item& item) {
  Vector_<const NameRef::Item*> items(1);
  items[0] = &item;
  return NewCompositeName(name, *new NameRef(items));
}

const NameRef::CompositeName& AbstractLexerTest::NewCompositeName(
    const String& name,
    const NameRef& item) {
  Vector_<const NameRef*> args(1);
  args[0] = &item;
  return *new NameRef::CompositeName(
    Name::Intern(name),
    args,
    SourceInfo("unittest", 1, 3));
}

TokenBox AbstractLexerTest::NewInt(const Type& ty, int64 const i64) {
  return TokenBox(*new IntToken(SourceInfo("unittest", 0, 0), ty, i64));
}

TokenBox AbstractLexerTest::NewRank(int const rank) {
  ASSERT(rank >= 1);
  return TokenBox(*new RankSpecToken(SourceInfo("unittest", 0, 0), rank));
}

const NameRef::SimpleName& AbstractLexerTest::NewSimpleName(
    const String& str) {
  return *new NameRef::SimpleName(
      Name::Intern(str),
      SourceInfo("unittest", 0, 0));
}

TokenBox AbstractLexerTest::NewString(const String& str) {
  return TokenBox(*new StringToken(SourceInfo("unittest", 0, 0), str));
}

TokenBox AbstractLexerTest::NewToken(const String& str1) {
  return TokenBox(
      *new NameToken(SourceInfo("unittest", 0, 0), Name::Intern(str1)));
}

TokenBox AbstractLexerTest::NewToken(
    const String& str1,
    const String& str2) {
  Vector_<const NameRef::Item*> names(2);
  names[0] = &NewSimpleName(str1);
  names[1] = &NewSimpleName(str2);
  return TokenBox(*new NameRefToken(*new NameRef(names)));
}

TokenBox AbstractLexerTest::NewToken(
    const String& str1,
    const String& str2,
    const String& str3) {
  Vector_<const NameRef::Item*> names(3);
  names[0] = &NewSimpleName(str1);
  names[1] = &NewSimpleName(str2);
  names[2] = &NewSimpleName(str3);
  return TokenBox(*new NameRefToken(*new NameRef(names)));
}

TokenBox AbstractLexerTest::NewToken(float32 const f32) {
  return TokenBox(*new Float32Token(SourceInfo("unittest", 0, 0), f32));
}

TokenBox AbstractLexerTest::NewToken(float64 const f64) {
  return TokenBox(*new Float64Token(SourceInfo("unittest", 0, 0), f64));
}

TokenBox AbstractLexerTest::NewToken(const Keyword& keyword) {
  return TokenBox(*new KeywordToken(SourceInfo("unittest", 0, 0), keyword));
}

TokenBox AbstractLexerTest::NewToken(const NameRef& name_ref) {
  return TokenBox(*new NameRefToken(name_ref));
}

TokenBox AbstractLexerTest::NewToken(const NameRef::Item& item) {
  Vector_<const NameRef::Item*> items(1);
  items[0] = &item;
  return TokenBox(*new NameRefToken(*new NameRef(items)));
}

TokenBox AbstractLexerTest::NewToken(
    const NameRef::Item& item1,
    const NameRef::Item& item2) {
  Vector_<const NameRef::Item*> items(2);
  items[0] = &item1;
  items[1] = &item2;
  return TokenBox(*new NameRefToken(*new NameRef(items)));
}

TokenBox AbstractLexerTest::NewToken(const Operator& op) {
  return TokenBox(*new OperatorToken(SourceInfo("unittest", 0, 0), op));
}

} // CompilerTest
