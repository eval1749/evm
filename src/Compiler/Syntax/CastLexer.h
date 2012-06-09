// @(#)$Id$
//
// Evita Compiler - CastLexer.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_CastLexer_h)
#define INCLUDE_evc_CastLexer_h

#include "./CastOperator.h"
#include "./Keyword.h"
#include "./TokenHandler.h"
#include "./TypeNameResolver.h"

namespace Compiler {

class CastLexer : public TokenHandler {
  private: enum State {
    State_Unary,
    State_Unary_Name,
    State_Unary_Paren,
    State_Unary_Paren_Name,
    State_Unary_Paren_Name_Close,
    State_Unary_Paren_Type,
  }; // State

  private: State state_;
  private: NameRef* name_ref_;
  private: Type* m_pType;
  private: TypeNameResolver* const m_pTypeNameResolver;

  // ctor
  public: explicit CastLexer(
    TokenProcessor* const pProcessor,
    TypeNameResolver* const pTypeNameResolver)
    : TokenHandler(pProcessor),
      state_(State_Unary),
      name_ref_(nullptr),
      m_pType(nullptr),
      m_pTypeNameResolver(pTypeNameResolver) {
  }

  // [F]
  private: void FinishProcess() {
    GotToken();
    Reset();
  } // FinishProcess

  private: void FinishProcess(const Token& look_ahead) {
    SaveToken(look_ahead);
    FinishProcess();
  }

  // [I]
  // "~", "!", "(", identifier, literal, keyword - ("as", "is")
  private: static bool IsStotoken(const Token& token) {
    if (token == *Op__LogNot) {
        return true;
    } // if

    if (token == *Op__Not) {
        return true;
    } // if

    if (token == *Op__OpenParen) {
        return true;
    } // if

    if (token.Is<NameRef>()) {
        return true;
    } // if

    if (token == *K_as || token == *K_is) {
        return false;
    } // if

    if (token.Is<Keyword>()) {
        return true;
    } // if

    return false;
  } // IsStotoken

  // [O]
  protected: void OnChange() {
    Reset();
  } // OnChange

  // [P]
  protected: virtual void ProcessAux(const Token& token) override {
    switch (state_) {
    case State_Unary:
        if (token == *Op__OpenParen) {
            SaveToken(token);
            state_ = State_Unary_Paren;
            return;
        } // if

        GotToken(token);
        return;

    ////////////////////////////////////////////////////////////
    //
    // Cast ::= "(" type ")" Stotoken
    // Stotoken ::= "!", "(", "~", identifier, literal,
    //      keyword other than "as" and "is".
    //
    case State_Unary_Paren:
        SaveToken(token);

        if (token == *Op__OpenParen) {
            GotToken(token);
            state_ = State_Unary;
            return;
        } // if

        if (auto const pType = token.DynamicCast<Type>()) {
            m_pType = pType;
            state_ = State_Unary_Paren_Type;
            return;
        } // if

        if (auto const pNameRef = token.DynamicCast<NameRef>()) {
            name_ref_ = pNameRef;
            state_ = State_Unary_Paren_Name;
            return;
        } // if

        FinishProcess(token);
        return;

    case State_Unary_Paren_Name:
        ASSERT(name_ref_ != nullptr);

        if (token == *Op__CloseParen) {
            SaveToken(token);
            state_ = State_Unary_Paren_Name_Close;
            return;
        } // if

        FinishProcess(token);
        return;

    case State_Unary_Paren_Name_Close:
        ASSERT(name_ref_ != nullptr);

        if (IsStotoken(token)) {
            ClearQueue();
            auto& type = m_pTypeNameResolver->ResolveTypeName(
                name_ref_);
            SaveToken(*new CastOperator(type));
        } // if

        FinishProcess(token);
        return;

    case State_Unary_Paren_Type:
        ASSERT(m_pType != nullptr);

        if (token == *Op__CloseParen) {
            ClearQueue();
            GotToken(*new CastOperator(*m_pType));
            FinishProcess();
            return;
        } // if

        // Got "(type something". This will cause syntax error.
        FinishProcess(token);
        return;

    default:
        CAN_NOT_HAPPEN();
    } // swtich state_
  } // ProcessAux

  // [R]
  private: void Reset() {
    state_ = State_Unary;
    name_ref_ = nullptr;
    m_pType = nullptr;
  } // Reset

  DISALLOW_COPY_AND_ASSIGN(CastLexer);
}; // CastLexer

} // Compiler

#endif // !defined(INCLUDE_evc_CastLexer_h)
