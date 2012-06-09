// @(#)$Id$
//
// Evita Compiler - TokenHandler.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Compiler_Syntax_TokenHandler_h)
#define INCLUDE_Evita_Compiler_Syntax_TokenHandler_h

#include "./TokenProcessor.h"

namespace Compiler {

class TokenHandler : public TokenProcessor {
  public: enum Flag {
    Disabled,
    Enabled,
  };

  private: int enabled_count_;
  private: TokenProcessor& processor_;
  private: Queue_<const Token*> qeueu_;

  // ctor
  protected: explicit TokenHandler(TokenProcessor& processor)
    : enabled_count_(1),
      processor_(processor) {
  }

  public: virtual ~TokenHandler() {}

  // [C]
  protected: void ClearQueue() {
    qeueu_.Clear();
  }

  // [D]
  public: void Disable() {
    ASSERT(enabled_count_ > 0);
    enabled_count_--;
    if (enabled_count_ == 0) {
        OnChange();
    }
  }

  // [E]
  public: void Enable() {
    enabled_count_++;
    if (enabled_count_ == 1) {
        OnChange();
    }
  }

  // [G]
  protected: void GotToken() {
    // TODO(yosi) 2012-02-10 We should stop feeding token once compilation
    // failed.
    while (!qeueu_.IsEmpty()) {
      auto& token = *qeueu_.Take();
      processor_.ProcessToken(token);
    }
  }

  protected: void GotToken(const Token& token) {
    processor_.ProcessToken(token);
    GotToken();
  }

  // [I]
  public: bool IsEnabled() const {
    return enabled_count_ > 0;
  }

  // [O]
  protected: virtual void OnChange() {}

  // [P]
  public: virtual void ProcessToken(const Token& token) override {
    if (IsEnabled()) {
      ProcessAux(token);
    } else {
      processor_.ProcessToken(token);
    }
  }

  protected: virtual void ProcessAux(const Token&) = 0;

  // [S]
  protected: void SaveToken(const Token& token) {
    qeueu_.Give(&token);
  }

  DISALLOW_COPY_AND_ASSIGN(TokenHandler);
};

} // Compiler

#endif // !defined(INCLUDE_Evita_Compiler_Syntax_TokenHandler_h)
