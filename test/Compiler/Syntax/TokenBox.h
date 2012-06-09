// @(#)$Id$
//
// Evita Compiler Test - TokenBox
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Compiler_Syntax_TokenBox_h)
#define INCLUDE_test_Compiler_Syntax_TokenBox_h

#include <ostream>

#include "Il/Ir/Name.h"
#include "Compiler/Syntax/Keyword.h"
#include "Compiler/Syntax/Lexer.h"
#include "Compiler/Syntax/Operator.h"
#include "Compiler/Syntax/Token.h"

namespace CompilerTest {

using namespace Compiler;

class TokenBox {
  private: ScopedRefCount_<const Token> token_;
  public: TokenBox(const Token& token) : token_(token) {}
  public: TokenBox(const TokenBox& box) : token_(*box.token_) {}
  public: ~TokenBox();
  private: TokenBox& operator=(const TokenBox&);
  public: const Token& token() const { return *token_; }
  public: bool operator==(const TokenBox&) const;
  public: String ToString() const;
};

::std::ostream& operator<<(::std::ostream&, const TokenBox&);

} // CompilerTest

#endif // !defined(INCLUDE_test_Compiler_Syntax_TokenBox_h)
