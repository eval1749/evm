// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Compiler_Syntax_AbstractAbstractLexerTest_h)
#define INCLUDE_test_Compiler_Syntax_AbstractAbstractLexerTest_h

#include "../AbstractCompilerTest.h"
#include "../DomNode.h"
#include "./TokenBox.h"
#include "Compiler/Syntax/TokenProcessor.h"

namespace CompilerTest {

using namespace Compiler;

class AbstractLexerTest :
    public AbstractCompilerTest,
    public TokenProcessor {

  protected: Queue_<const Token*> tokens_;

  public: virtual ~AbstractLexerTest();

  // [G]
  protected: TokenBox GetToken() {
    return TokenBox(*tokens_.Take());
  }

  // [H]
  protected: bool HasToken() const { return !tokens_.IsEmpty(); }

  // [P]
  private: void ProcessToken(const Token&) override;

  // [N]
  protected: const NameRef& NewArrayNameRef(const String&, int);
  protected: const NameRef& NewArrayNameRef(const String&, int, int);
  protected: TokenBox NewChar(char16);
  protected: const NameRef::CompositeName& NewCompositeName(
      const String&,
      const String&);
  protected: const NameRef::CompositeName& NewCompositeName(
      const String&,
      const String&,
      const String&);
  protected: const NameRef::CompositeName& NewCompositeName(
      const String&,
      const NameRef::Item&);
  protected: const NameRef::CompositeName& NewCompositeName(
      const String&,
      const NameRef&);
  protected: TokenBox NewInt(const Type&, int64);
  protected: TokenBox NewRank(int);
  protected: const NameRef::SimpleName& NewSimpleName(const String&);
  protected: TokenBox NewString(const String&);
  protected: TokenBox NewToken(const String&);
  protected: TokenBox NewToken(const String&, const String&);
  protected: TokenBox NewToken(const String&, const String&, const String&);
  protected: TokenBox NewToken(float32);
  protected: TokenBox NewToken(float64);
  protected: TokenBox NewToken(const Keyword&);
  protected: TokenBox NewToken(const NameRef&);
  protected: TokenBox NewToken(const NameRef::Item&);
  protected: TokenBox NewToken(const NameRef::Item&, const NameRef::Item&);
  protected: TokenBox NewToken(const Operator&);
};

} // CompilerTest

#endif // !defined(INCLUDE_test_Compiler_Syntax_AbstractAbstractLexerTest_h)
