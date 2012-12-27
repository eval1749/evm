#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Parsre - NameLexer
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NameLexer.h"

#include "./Operator.h"

namespace Compiler {

namespace {

typedef HashSet_<Operator*> TokenSet;
static TokenSet* s_stop_token_set;

// [I]
// 7.6.4.2 Grammar ambiguities (p.163)
static void InitStopTokenSet() {
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
}

static bool IsStopToken(const Token& token) {
  auto const p = token.DynamicCast<Operator>();
  return p && s_stop_token_set->Contains(p);
}

} // namespace

NameLexer::Builder::Builder(const NameToken& name_token)
    : close_token_(nullptr), lookahead_token_(nullptr) {
  Add(name_token);
}

NameLexer::Builder::~Builder() {
  DEBUG_PRINTF("%p\n", this);

  ReleaseTokens();

  for (auto const builder: builders_)
    delete builder;
}

void NameLexer::Builder::Add(Builder& builder) {
  builders_.Add(&builder);
}

void NameLexer::Builder::Add(int rank) {
  ASSERT(rank >= 1);
  ReleaseTokens();
  ranks_.Add(rank);
}

void NameLexer::Builder::Add(const NameToken& name_token) {
  ReleaseTokens();
  items_.Add(
    new NameRef::SimpleName(
        name_token.name(),
        name_token.source_info()));
}

const NameRef& NameLexer::Builder::MakeNameRef() {
  return *new NameRef(items_.ToVector(), ranks_.ToVector());
}

const Token& NameLexer::Builder::MakeToken() {
  if (ranks_.IsEmpty()
        && items_.Count() == 1
        && items_[0]->Is<NameRef::SimpleName>()) {
    return *new NameToken(items_[0]->source_info(), items_[0]->name());
  }

  return *new NameRefToken(MakeNameRef());
}

void NameLexer::Builder::Process(TokenProcessor& processor) {
  {
    ScopedRefCount_<const Token> token(MakeToken());
    processor.ProcessToken(*token);
  }

  if (lookahead_token_) {
    processor.ProcessToken(*lookahead_token_);
  }

  for (auto const builder: builders_)
    builder->Process(processor);

  if (close_token_) {
    ASSERT(lookahead_token_ && lookahead_token_->Is(*Op__Lt));
    processor.ProcessToken(*close_token_);
  }

  ReleaseTokens();
}

void NameLexer::Builder::ReleaseTokens() {
  if (lookahead_token_) {
    lookahead_token_->Release();
    lookahead_token_ = nullptr;
  }

  if (close_token_) {
    close_token_->Release();
    close_token_ = nullptr;
  }
}

void NameLexer::Builder::SetComma(const Token& token) {
  ASSERT(token.Is(*Op__Comma));
  ReleaseTokens();
  lookahead_token_ = &token;
  token.AddRef();
}

void NameLexer::Builder::SetComposite() {
  ASSERT(!items_.IsEmpty());
  ASSERT(!close_token_
            || close_token_->Is(*Op__Gt)
            || close_token_->Is(*Op__Shr));
  ASSERT(lookahead_token_ && lookahead_token_->Is(*Op__Lt));

  ReleaseTokens();

  ArrayList_<const NameRef*> args(builders_.Count());
  for (auto const builder: builders_) {
    args.Add(&builder->MakeNameRef());
    delete builder;
  }

  builders_.Clear();

  auto& last = *items_[items_.Count() - 1];

  items_[items_.Count() - 1] = new NameRef::CompositeName(
    last.name(),
    args.ToVector(),
    last.source_info());
}

void NameLexer::Builder::SetDot(const Token& token) {
  ASSERT(token.Is(*Op__Dot));
  ReleaseTokens();
  lookahead_token_ = &token;
  token.AddRef();
}

void NameLexer::Builder::SetGt(const Token& token) {
  ASSERT(token.Is(*Op__Gt));
  ASSERT(!close_token_);
  ASSERT(lookahead_token_ && lookahead_token_->Is(*Op__Lt));
  close_token_ = &token;
  token.AddRef();
}

void NameLexer::Builder::SetGtGt(const Token &token) {
  ASSERT(token.Is(*Op__Shr));
  ASSERT(!close_token_);
  ASSERT(lookahead_token_ && lookahead_token_->Is(*Op__Lt));
  close_token_ = &token;
  token.AddRef();
}

void NameLexer::Builder::StartComposite(const Token& token) {
  ASSERT(!lookahead_token_);
  ASSERT(token.Is(*Op__Lt));
  lookahead_token_ = &token;
  token.AddRef();
}

// ctor
NameLexer::NameLexer(TokenProcessor& processor)
    : builder_(nullptr),
      mode_(Mode_Type),
      processor_(processor),
      state_(State_Start) {
  if (s_stop_token_set == nullptr) {
    InitStopTokenSet();
  }
}

NameLexer::~NameLexer() {
  delete builder_;
  while (!builder_stack_.IsEmpty()) {
    delete builder_stack_.Pop();
  }
}

// [E]
void NameLexer::EndArgument() {
  ASSERT(!!builder_);
  ASSERT(!builder_stack_.IsEmpty());
  builder_stack_.GetTop()->Add(*builder_);
  builder_ = nullptr;
}

void NameLexer::EndTwoComposites() {
  ASSERT(!!builder_);
  ASSERT(builder_stack_.Count() >= 1);

  builder_->SetComposite();
  EndArgument();

  builder_= builder_stack_.Pop();
  builder_->SetComposite();
}

// [F]
void NameLexer::FinishBuild() {
  if (builder_) {
    builder_stack_.Push(builder_);
    builder_ = nullptr;
  }

  for (auto i = builder_stack_.Count() - 1; i >= 0; --i) {
    builder_stack_[i]->Process(processor_);
  }

  while (!builder_stack_.IsEmpty()) {
    delete builder_stack_.Pop();
  }

  state_ = State_Start;
}

void NameLexer::FinishBuild(const Token& token) {
  DEBUG_FORMAT("token=%s", token);
  FinishBuild();
  processor_.ProcessToken(token);
}

// [H]
void NameLexer::HandleGt(const Token& token) {
  ASSERT(token.Is(*Op__Gt));
  ASSERT(!builder_);
  ASSERT(!builder_stack_.IsEmpty());
  builder_ = builder_stack_.Pop();
  builder_->SetGt(token);
  state_ = State_Gt;
}

void NameLexer::HandleGtGt(const Token& token) {
  ASSERT(token.Is(*Op__Shr));
  ASSERT(!builder_);
  ASSERT(builder_stack_.Count() >= 2);
  builder_ = builder_stack_.Pop();
  builder_stack_[0]->SetGtGt(token);
  state_ = State_Gt_Gt;
}

// [P]
void NameLexer::PopMode() {
  mode_ = mode_stack_.Pop();
}

void NameLexer::ProcessToken(const Token& token) {
  DEBUG_FORMAT("[%d] state=%d token=%s",
    builder_stack_.Count(), state_, token);
  switch (state_) {
    case State_Gt:
      ASSERT(!!builder_);
      ASSERT(!token.Is(*Op__Gt)); // ">>" must be in State_Gt_Gt

      if (token.Is(*Op__Comma)) {
        builder_->SetComposite();
        if (builder_stack_.IsEmpty()) {
          FinishBuild(token);
          return;
        }

        builder_->SetComma(token);
        EndArgument();
        state_ = State_Name_Lt;
        return;
      }

      if (token.Is(*Op__Dot)) {
        builder_->SetComposite();
        builder_->SetDot(token);
        state_ = State_Name_Dot;
        return;
      }

      if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
        builder_->SetComposite();
        builder_->Add(rank_token->rank());
        state_ = State_Gt_Rank;
        return;
      }

      if (builder_stack_.IsEmpty()) {
        if (mode_ == Mode_Type || IsStopToken(token)) {
          builder_->SetComposite();
        }
      }

      FinishBuild(token);
      return;

    case State_Gt_Gt:
      ASSERT(!!builder_);
      ASSERT(builder_stack_.Count() >= 1);

      if (token.Is(*Op__Comma)) {
        EndTwoComposites();

        if (builder_stack_.IsEmpty()) {
          FinishBuild(token);
          return;
        }

        builder_->SetComma(token);
        EndArgument();
        state_ = State_Name_Lt;
        return;
      }

      if (token.Is(*Op__Dot)) {
        EndTwoComposites();
        builder_->SetDot(token);
        state_ = State_Name_Dot;
        return;
      }

      if (token.Is(*Op__Gt)) {  // A < B < C < T >> >
        if (builder_stack_.Count() >= 2) { // A < B<C<T>> >
          EndTwoComposites();
          EndArgument();
          HandleGt(token);
          return;
        }
        FinishBuild(token);
        return;
      }

      if (token.Is(*Op__Shr)) { // A < B < C < D < T >> >>
        if (builder_stack_.Count() >= 3) { // A< B < C<D<T>> >>
          EndTwoComposites();
          EndArgument();
          HandleGtGt(token);
          return;
        }
        FinishBuild(token);
        return;
      }

      if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
        EndTwoComposites();
        builder_->Add(rank_token->rank());
        state_ = State_Gt_Rank;
        return;
      }

      if (builder_stack_.Count() == 1) {
        if (mode_ == Mode_Type || IsStopToken(token)) {
          EndTwoComposites();
        }
      }

      FinishBuild(token);
      return;

    case State_Gt_Rank:
      if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
        builder_->SetComposite();
        builder_->Add(rank_token->rank());
        state_ = State_Gt_Rank;
        return;
      }

      FinishBuild(token);
      return;

    case State_Name:
      ASSERT(!!builder_);
      ASSERT(builder_stack_.IsEmpty());

      if (token.Is(*Op__Dot)) {
        builder_->SetDot(token);
        state_ = State_Name_Dot;
        return;
      }

      if (token.Is(*Op__Lt)) {
        StartComposite(token);
        state_ = State_Name_Lt;
        return;
      }

      if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
        builder_->Add(rank_token->rank());
        state_ = State_Name_Rank;
        return;
      }

      if (auto const name = token.DynamicCast<NameToken>()) {
        builder_->Process(processor_);
        delete builder_;
        builder_ = new Builder(*name);
        return;
      }

      FinishBuild(token);
      return;

    case State_Name_Dot:
      ASSERT(!!builder_);

      if (auto const name = token.DynamicCast<NameToken>()) {
        builder_->Add(*name);
        state_ = State_Name_Dot_Name;
        return;
      }

      FinishBuild(token);
      return;

    case State_Name_Dot_Name:
      ASSERT(!!builder_);

      if (token.Is(*Op__Dot)) {
        builder_->SetDot(token);
        state_ = State_Name_Dot_Name_Dot;
        return;
      }

      if (token.Is(*Op__Lt)) {
        StartComposite(token);
        state_ = State_Name_Lt;
        return;
      }

      if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
        builder_->Add(rank_token->rank());
        state_ = State_Name_Rank;
        return;
      }

      FinishBuild(token);
      return;

    case State_Name_Dot_Name_Dot:
      ASSERT(!!builder_);

      if (auto const name = token.DynamicCast<NameToken>()) {
        builder_->Add(*name);
        state_ = State_Name_Dot_Name;
        return;
      }

      FinishBuild(token);
      return;

    case State_Name_Lt:
      ASSERT(!builder_);

      if (auto const name = token.DynamicCast<NameToken>()) {
        builder_ = new Builder(*name);
        state_ = State_Name_Lt_Name;
        return;
      }

      if (token.Is(*Op__Gt)) { // A<>
        HandleGt(token);
        FinishBuild();
        return;
      }

      if (token.Is(*Op__Shr)) {
        if (builder_stack_.Count() >= 2) { // A<B<>>
          HandleGtGt(token);
          FinishBuild();
          return;
        }
      }

      FinishBuild(token);
      return;

    case State_Name_Lt_Name:
      ASSERT(!!builder_);

      if (token.Is(*Op__Comma)) {
        builder_->SetComma(token);
        EndArgument();
        state_ = State_Name_Lt;
        return;
      }

      if (token.Is(*Op__Dot)) {
        builder_->SetDot(token);
        state_ = State_Name_Lt_Name_Dot;
        return;
      }

      if (token.Is(*Op__Gt)) {
        EndArgument();
        HandleGt(token);
        return;
      }

      if (token.Is(*Op__Lt)) {
        StartComposite(token);
        state_ = State_Name_Lt;
        return;
      }

      if (token.Is(*Op__Shr)) {
        if (builder_stack_.Count() >= 2) { // A<B<T>>
          EndArgument();
          HandleGtGt(token);
          return;
        }
      }

      if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
        builder_->Add(rank_token->rank());
        state_ = State_Name_Lt_Name_Rank;
        return;
      }

      FinishBuild(token);
      return;

    case State_Name_Lt_Name_Dot:
      ASSERT(!!builder_);

      if (auto const name = token.DynamicCast<NameToken>()) {
        builder_->Add(*name);
        state_ = State_Name_Lt_Name;
        return;
      }

      FinishBuild(token);
      return;

    case State_Name_Lt_Name_Rank:
      ASSERT(!!builder_);

      if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
        builder_->Add(rank_token->rank());
        return;
      }

      if (token.Is(*Op__Comma)) {
        builder_->SetComma(token);
        EndArgument();
        state_ = State_Name_Lt;
        return;
      }

      if (token.Is(*Op__Gt)) {
        EndArgument();
        HandleGt(token);
        return;
      }

      if (token.Is(*Op__Shr)) {
        if (builder_stack_.Count() >= 2) { // A<B<T[]>>
          EndArgument();
          HandleGtGt(token);
          return;
        }
      }

      FinishBuild(token);
      return;

    case State_Name_Rank:
      ASSERT(!!builder_);
      if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
        builder_->Add(rank_token->rank());
        return;
      }

      FinishBuild(token);
      return;

    case State_Start:
      ASSERT(!builder_);

      if (auto const name = token.DynamicCast<NameToken>()) {
        builder_ = new Builder(*name);
        state_ = State_Name;
        return;
      }

      processor_.ProcessToken(token);
      return;

    default:
      CAN_NOT_HAPPEN();
  }
}

// [S]
void NameLexer::SetExpressionMode() {
  mode_stack_.Push(mode_);
  mode_ = Mode_Expression;
}

void NameLexer::StartComposite(const Token& token) {
  ASSERT(!!builder_);
  ASSERT(token.Is(*Op__Lt));
  builder_->StartComposite(token);
  builder_stack_.Push(builder_);
  builder_ = nullptr;
}

} // Compiler
