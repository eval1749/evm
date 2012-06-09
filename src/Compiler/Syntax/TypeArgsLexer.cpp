#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - TypeArgsLexer.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeArgsLexer.h"

#include "./Operator.h"
#include "./RankSpec.h"
#include "./TypeArgList.h"

#include "../../Common/Collections/HashSet_.h."
#include "../../Common/Collections/PtrKey_.h."

namespace Compiler {

using namespace Common::Collections;

// new-expresion:
//  'new' type ('[' integer (',' integer)* ']')*
//
// cast-expression:
//  '(' type ')' unary-expression
//
// statement-expression:
//  invocation-expression
//  object-creation-expression
//  assignment
//  post-increment-expression
//  post-decrement-expression
//  pre-increment-expression
//  pre-decrement-expression
//
//  T<A,B> name is
//      type name
//  instead of
//      name '<' name ',' name '>' name
//  Because of relational expression isn't statement-expression.
//
//  'for' '(' variable-declaration? ';' bool-expression? ';' expression? ')'
//  'for' '(' variable-declaration ':' expression ')'
//

namespace {

typedef HashSet_<Operator*> TokenSet;
static TokenSet* s_stop_token_set;

// [I]
// 7.6.4.2 Grammar ambiguities (p.163)
static void InitStotokenSet() {
  s_stop_token_set = new TokenSet();
  s_stop_token_set->Add(Op__OpenParen);        // \(
  s_stop_token_set->Add(Op__CloseParen);       // \)
  s_stop_token_set->Add(Op__CloseBracket);     // \[
  s_stop_token_set->Add(Op__CloseBrace);       // \{
  s_stop_token_set->Add(Op__Colon);            // :
  s_stop_token_set->Add(Op__SemiColon);        // ;
  s_stop_token_set->Add(Op__Comma);            // ,
  s_stop_token_set->Add(Op__Dot);              // .
  s_stop_token_set->Add(Op__Question);         // ?
  s_stop_token_set->Add(Op__Eq);               // ==
  s_stop_token_set->Add(Op__Ne);               // !=
  s_stop_token_set->Add(Op__LogIor);           // |
  s_stop_token_set->Add(Op__LogXor);           // ^
} // InitStotokenSet

static bool IsStopToken(const Token& token) {
  auto const p = token.DynamicCast<Operator>();
  return p && s_stop_token_set->Contains(p);
} // IsStotoken

static bool IsNameToken(const Token& token) {
  return token.Is<Name>() || token.Is<NameRef>();
}

// Construct type arguments token.
class TypeArgsFactory {
  private: Queue_<const Token*> input_;
  private: Queue_<const Token*> output_;
  private: TokenProcessor* const processor_;
  private: const Token* unread_;

  public: TypeArgsFactory(
      Queue_<const Token*>* const input,
      TokenProcessor* const processor)
      : processor_(processor), unread_(nullptr) {
    while (!input->IsEmpty()) {
      auto& token = *input->Take();
      if (token == *Op__Shr) {
        input_.Give(Op__Gt);
        input_.Give(Op__Gt);
      } else {
        input_.Give(&token);
      }
    }
  }

  // [E]
  private: void Emit(const Token& token) {
    processor_->ProcessToken(token);
  }

  // [G]
  private: const Token& GetToken() {
    if (auto const unread = unread_) {
      unread_ = nullptr;
      return *unread;
    }
    return *input_.Take();
  }

  // [H]
  private: const Token& HandleTypeArgs(NameRef* const name_ref) {
    Type::List tylist;
    for (;;) {
      const Type* type = HandleTypeArg();

      // Handle rank specifier.
      //    T rank-spec* (',' | '>')
      for (;;) {
        auto& token = GetToken();
        if (token == *Op__Gt) {
          tylist.Add(type);
          if (name_ref != nullptr) {
            return *new UnresolvedConstructedType(name_ref, tylist);
          }
          return *new TypeArgList(tylist);
        }

        if (token == *Op__Comma) {
          tylist.Add(type);
          break;
        }

        if (auto const rank_spec = token.DynamicCast<RankSpec>()) {
          type = &ArrayType::Intern(*type, rank_spec->GetRank());

        } else {
          CAN_NOT_HAPPEN();
        }
      }
    }
  }

  private: const Type* HandleTypeArg() {
    auto& token = GetToken();
    if (auto const name_ref = token.DynamicCast<NameRef>()) {
      auto& token = GetToken();
      if (token == *Op__Lt) {
        return HandleTypeArgs(name_ref).StaticCast<Type>();
      }

      UngetToken(token);
      return &UnresolvedType::New(name_ref);
    }

    if (auto const type = token.DynamicCast<Type>()) {
      return type;
    }

    CAN_NOT_HAPPEN();
  }

  // [R]
  public: void Run() {
    Emit(GetToken());
    ASSERT(GetToken() == *Op__Lt);
    Emit(HandleTypeArgs(nullptr));
  }

  // [U]
  private: void UngetToken(const Token& token) {
    ASSERT(unread_ == nullptr);
    unread_ = &token;
  }

  DISALLOW_COPY_AND_ASSIGN(TypeArgsFactory);
};

}   // namespace

// ctor
TypeArgsLexer::TypeArgsLexer(TokenProcessor* const processor)
    : ambiguities_(false),
      lt_count_(0),
      processor_(processor),
      state_(State_Enabled) {
  if (s_stop_token_set == nullptr) {
    InitStotokenSet();
  } // if
} // TypeArgsLexer

// [P]
// Enabled:
//  name "<" type ("," type)* ">" Stotoken
//  => name type-argument-list
void TypeArgsLexer::ProcessToken(const Token& token) {
  switch (state_) {
    case State_Disabled:
      SendToken(token);
      break;

    case State_Enabled:
      ASSERT(lt_count_ == 0);
      ASSERT(queue_.IsEmpty());

      if (IsNameToken(token)) {
        queue_.Give(&token);
        state_ = State_Enabled_Name;
      } else {
        SendToken(token);
        state_ = State_Enabled;
      }
      break;

    case State_Enabled_Name:
      ASSERT(lt_count_ == 0);
      ASSERT(queue_.Count() == 1);

      queue_.Give(&token);

      if (token == *Op__Lt) {
        lt_count_ = 1;
        state_ = State_Enabled_Name_Lt;

      } else {
        SendAllTokens();
      }
      break;

    case State_Enabled_Name_Lt:
      ASSERT(lt_count_ >= 1);

      queue_.Give(&token);

      if (IsNameToken(token)) {
        state_ = State_Enabled_Name_Lt_Name;

      } else if (token.Is<Type>()) {
        state_ = State_Enabled_Name_Lt_Type;

      } else {
        SendAllTokens();
      }
      break;

    case State_Enabled_Name_Lt_Name:
      ASSERT(lt_count_ >= 1);
      ASSERT(!queue_.IsEmpty());

      queue_.Give(&token);

      if (token == *Op__Comma) {
        state_ = State_Enabled_Name_Lt;

      } else if (token == *Op__Gt) {
        lt_count_--;

        state_  = lt_count_ == 0
            ? State_Enabled_Name_Lt_Name_Gt
            : State_Enabled_Name_Lt_Type;

      } else if (token == *Op__Lt) {
        lt_count_++;
        state_ = State_Enabled_Name_Lt;

      } else if (token == *Op__Shr && lt_count_ >= 2) {
        lt_count_ -= 2;
        state_  = State_Enabled_Name_Lt_Name_Gt;

      } else if (token.Is<RankSpec>()) {
        // nothing to do

      } else {
        SendAllTokens();
      }
      break;

    case State_Enabled_Name_Lt_Name_Gt:
      ASSERT(lt_count_ == 0);
      ASSERT(!queue_.IsEmpty());

      //  declaration: name '<' ... '>' name
      //        Queue<T> name
      //  type: name '<' ... '>' '[' ','* ']'
      //        Queue<T>[]
      //  unary: name '<' ... '>' StopToken
      //        Queue<T>;
      if (token.Is<RankSpec>() || !ambiguities_ || IsStopToken(token)) {
        TypeArgsFactory factory(&queue_, processor_);
        factory.Run();
        ASSERT(queue_.IsEmpty());
        SendToken(token);
        state_ = State_Disabled;
        return;
      }

      queue_.Give(&token);
      SendAllTokens();
      break;

    case State_Enabled_Name_Lt_Type:
      ASSERT(lt_count_ >= 1);
      ASSERT(!queue_.IsEmpty());

      queue_.Give(&token);

      if (token == *Op__Comma) {
        state_ = State_Enabled_Name_Lt;

      } else if (token == *Op__Gt) {
        lt_count_--;

        state_  = lt_count_ == 0
            ? State_Enabled_Name_Lt_Name_Gt
            : State_Enabled_Name_Lt_Type;

      } else if (token == *Op__Shr && lt_count_ >= 2) {
        lt_count_ -= 2;
        state_  = State_Enabled_Name_Lt_Name_Gt;

      } else if (token.Is<RankSpec>()) {
        // nothing to do

      } else {
        SendAllTokens();
      }
      break;

    default:
      CAN_NOT_HAPPEN();
  } // switch state_
} // ProcessToken

// [S]
void TypeArgsLexer::SendToken(const Token& token) {
  ASSERT(queue_.IsEmpty());
  processor_->ProcessToken(token);
}

void TypeArgsLexer::SendAllTokens() {
  ASSERT(!queue_.IsEmpty());
  while (!queue_.IsEmpty()) {
    auto& token = *queue_.Take();
    processor_->ProcessToken(token);
  }
  state_ = State_Disabled;
}

} // Compiler
