#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Token Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCompilerTest.h"

#include <ostream>

#include "Compiler/Syntax/Token.h"

namespace CompilerTest {

using namespace Compiler;

class TokenTest : public AbstractCompilerTest {
  protected: SourceInfo source_info_;

  protected: TokenTest()
    : source_info_("unittest", 123, 456) {}

  protected: MemoryZone& zone() {
    return session().memory_zone();
  }
};

TEST_F(TokenTest, CharToken) {
  ScopedRefCount_<CharToken> token_a(new CharToken(source_info_, 'a'));
  EXPECT_FALSE(token_a->Is(*K_class));
  EXPECT_FALSE(token_a->Is(*Op__Add));
  EXPECT_FALSE(token_a->Is(*Q_var));
  EXPECT_EQ(*Ty_Char, token_a->literal_type());
  EXPECT_EQ(String("'a'"), token_a->ToString());
  auto& lit = token_a->ToLiteral(zone());
  EXPECT_EQ(*Ty_Char, lit.type());
  EXPECT_EQ(
      static_cast<char16>('a'), 
      static_cast<CharLiteral&>(lit).value());
}

TEST_F(TokenTest, QualifiedNameToken) {
  ScopedRefCount_<NameToken> token_n(
      new NameToken(source_info_, Name::Intern("n")));

  ScopedRefCount_<NameToken> token_a(
      new NameToken(source_info_, Name::Intern("a")));

  ScopedRefCount_<QualifiedNameToken> token_n_a(
      new QualifiedNameToken(
          CollectionV_<const NameToken*>(token_n.Get(), token_a.Get())));

  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(String("n.a"), token_n_a->ToString());

  auto& name_ref = token_n_a->ToNameRef();
  NameRef::Enum items(name_ref);

  ASSERT_FALSE(items.AtEnd());
  EXPECT_EQ(Name::Intern("n"), items->name());

  items.Next();
  ASSERT_FALSE(items.AtEnd());
  EXPECT_EQ(Name::Intern("a"), items->name());

  items.Next();
  EXPECT_TRUE(items.AtEnd());
}

TEST_F(TokenTest, Float32Token) {
  ScopedRefCount_<Float32Token> token_n(new Float32Token(source_info_, 0.5f));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(*Ty_Float32, token_n->literal_type());
  EXPECT_EQ(String("Float32(3F000000)"), token_n->ToString());
  auto& lit = token_n->ToLiteral(zone());
  EXPECT_EQ(*Ty_Float32, lit.type());
  EXPECT_EQ(0.5f, static_cast<Float32Literal&>(lit).value());
}

TEST_F(TokenTest, Float64Token) {
  ScopedRefCount_<Float64Token> token_n(new Float64Token(source_info_, 0.5));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(*Ty_Float64, token_n->literal_type());
  EXPECT_EQ(String("Float64(3FE0000000000000)"), token_n->ToString());
  auto& lit = token_n->ToLiteral(zone());
  EXPECT_EQ(*Ty_Float64, lit.type());
  EXPECT_EQ(0.5, static_cast<Float64Literal&>(lit).value());
}

TEST_F(TokenTest, IntToken) {
  ScopedRefCount_<IntToken> token_n(
      new IntToken(source_info_, *Ty_Int32, 1234));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(*Ty_Int32, token_n->literal_type());
  EXPECT_EQ(String("1234"), token_n->ToString());
  auto& lit = token_n->ToLiteral(zone());
  EXPECT_EQ(*Ty_Int32, lit.type());
  EXPECT_EQ(1234, static_cast<Int32Literal&>(lit).value());
}

TEST_F(TokenTest, Int8Token) {
  ScopedRefCount_<IntToken> token_n(
    new IntToken(source_info_, *Ty_Int8, 42));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(*Ty_Int8, token_n->literal_type());
  EXPECT_EQ(String("Common.Int8(42)"), token_n->ToString());
  auto& lit = token_n->ToLiteral(zone());
  EXPECT_EQ(*Ty_Int8, lit.type());
  EXPECT_EQ(42, lit.GetInt32());
}

TEST_F(TokenTest, KeywordToken) {
  ScopedRefCount_<KeywordToken> token_n(
      new KeywordToken(source_info_, *K_namespace));
  EXPECT_TRUE(token_n->Is(*K_namespace));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(String("Keyword(namespace)"), token_n->ToString());
}

TEST_F(TokenTest, NameToken) {
  auto& foo = Name::Intern("foo");
  ScopedRefCount_<NameToken> token_n(
      new NameToken(source_info_, foo));
  EXPECT_TRUE(token_n->Is(foo));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(String("foo"), token_n->ToString());
}

TEST_F(TokenTest, OperatorToken) {
  ScopedRefCount_<OperatorToken> token_n(
      new OperatorToken(source_info_, *Op__Sub));
  EXPECT_TRUE(token_n->Is(*Op__Sub));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(String("operator -"), token_n->ToString());
}

TEST_F(TokenTest, RankSpecToken) {
  ScopedRefCount_<RankSpecToken> token_n(
      new RankSpecToken(source_info_, 2));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(2, token_n->rank());
  EXPECT_EQ(String("[,]"), token_n->ToString());
}

TEST_F(TokenTest, RawNameToken) {
  ScopedRefCount_<RawNameToken> token_n(
      new RawNameToken(source_info_, Name::Intern("var")));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(String("@var"), token_n->ToString());
}

TEST_F(TokenTest, StringToken) {
  ScopedRefCount_<StringToken> token_n(
      new StringToken(source_info_, "abc"));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(String("\"abc\""), token_n->ToString());
}

TEST_F(TokenTest, TypeToken) {
  ScopedRefCount_<TypeToken> token_n(
      new TypeToken(source_info_, *Ty_Float32));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(*Ty_Float32, token_n->type());
  EXPECT_EQ(String("Common.Float32"), token_n->ToString());
}

TEST_F(TokenTest, TypeListToken) {
  CollectionV_<const Type*> types(Ty_Int8, Ty_Int16, Ty_Int32);
  ScopedRefCount_<TypeListToken> token_n(
      new TypeListToken(source_info_, types));
  EXPECT_FALSE(token_n->Is(*K_class));
  EXPECT_FALSE(token_n->Is(*Op__Add));
  EXPECT_FALSE(token_n->Is(*Q_var));
  EXPECT_EQ(
    String("<Common.Int8,Common.Int16,Common.Int32>"),
    token_n->ToString());
  Type::Collection::Enum expected_enum(types);
  TypeListToken::Enum actual_enum(*token_n);
  while (!actual_enum.AtEnd() && !expected_enum.AtEnd()) {
    EXPECT_EQ(expected_enum.Get(), actual_enum.Get());
    actual_enum.Next();
    expected_enum.Next();
  }
  EXPECT_TRUE(actual_enum.AtEnd());
  EXPECT_TRUE(expected_enum.AtEnd());
}

} // CompilerTest
