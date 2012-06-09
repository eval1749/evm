// @(#)$Id$
//
// Evita Compiler - TypeArgsLexer.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Compiler_Syntax_TypeArgsLexer_h)
#define INCLUDE_evm_Compiler_Syntax_TypeArgsLexer_h

#include "./TokenProcessor.h"

namespace Compiler {

class TypeArgsLexer : public TokenProcessor {
  private: enum State {
    State_Disabled,

    State_Enabled,
    State_Enabled_Name,
    State_Enabled_Name_Lt,
    State_Enabled_Name_Lt_Name,
    State_Enabled_Name_Lt_Name_Gt,
    State_Enabled_Name_Lt_Type,
  }; // State

  private: bool ambiguities_;
  private: int lt_count_;
  private: Queue_<const Token*> queue_;
  private: TokenProcessor* processor_;
  private: State state_;

  // ctor
  public: TypeArgsLexer(TokenProcessor* const);

  // [D]
  public: void Disable() {
    state_ = State_Disabled;
  }

  // [E]
  public: void EnableWithAmbiguities() {
    ASSERT(state_ == State_Disabled);
    state_ = State_Enabled;
    ambiguities_ = false;
  }

  public: void EnableWithoutAmbiguities() {
    ASSERT(state_ == State_Disabled);
    state_ = State_Enabled;
    ambiguities_ = true;
  }

  // [P]
  public: virtual void ProcessToken(const Token&) override;

  // [S]
  private: void SendToken(const Token& token);
  private: void SendAllTokens();

  DISALLOW_COPY_AND_ASSIGN(TypeArgsLexer);
}; // TypeArgsLexer

} // Compiler

#endif // !defined(INCLUDE_evm_Compiler_Syntax_TypeArgsLexer_h)
