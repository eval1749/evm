#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler Test - TokenBox
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TokenBox.h"

#include "../../Il/IlTest.h"

namespace CompilerTest {

::std::ostream& operator<<(::std::ostream& os, const TokenBox& box) {
  return os << box.ToString();
}

TokenBox::~TokenBox() {
  DEBUG_FORMAT("[%s] %s %s", token_->ref_count(), token_->GetKind(), *token_);
}

bool TokenBox::operator==(const TokenBox& r) const {
  if (auto const a = token_->DynamicCast<CharToken>()) {
    if (auto const b = r.token_->DynamicCast<CharToken>()) {
      return a->code() == b->code();
    }
    return false;
  }

  if (auto const a = token_->DynamicCast<NameRefToken>()) {
    if (auto const b = r.token_->DynamicCast<NameRefToken>()) {
      return a->name_ref() == b->name_ref();
    }
    return false;
  }

  if (auto const a = token_->DynamicCast<Float32Token>()) {
    if (auto const b = r.token_->DynamicCast<Float32Token>()) {
      return a->value() == b->value();
    }
    return false;
  }

  if (auto const a = token_->DynamicCast<Float64Token>()) {
    if (auto const b = r.token_->DynamicCast<Float64Token>()) {
      return a->value() == b->value();
    }
    return false;
  }

  if (auto const a = token_->DynamicCast<IntToken>()) {
    if (auto const b = r.token_->DynamicCast<IntToken>()) {
      return a->literal_type() == b->literal_type()
          && a->value() == b->value();
    }
    return false;
  }

  if (auto const a = token_->DynamicCast<KeywordToken>()) {
    if (auto const b = r.token_->DynamicCast<KeywordToken>()) {
      return a->value() == b->value();
    }
    return false;
  }

  if (auto const a = token_->DynamicCast<NameToken>()) {
    if (auto const b = r.token_->DynamicCast<NameToken>()) {
      return a->name() == b->name();
    }
    return false;
  }

  if (auto const a = token_->DynamicCast<OperatorToken>()) {
    if (auto const b = r.token_->DynamicCast<OperatorToken>()) {
      return a->value() == b->value();
    }
    return false;
  }

  if (auto const a = token_->DynamicCast<RankSpecToken>()) {
    if (auto const b = r.token_->DynamicCast<RankSpecToken>()) {
      return a->rank() == b->rank();
    }
    return false;
  }

  if (auto const a = token_->DynamicCast<RawNameToken>()) {
    if (auto const b = r.token_->DynamicCast<RawNameToken>()) {
      return a->name() == b->name();
    }
  }

  if (auto const a = token_->DynamicCast<StringToken>()) {
    if (auto const b = r.token_->DynamicCast<StringToken>()) {
      return a->value() == b->value();
    }
  }

  return false;
}

String TokenBox::ToString() const {
  return token_->ToString();
}

} // CompilerTest
