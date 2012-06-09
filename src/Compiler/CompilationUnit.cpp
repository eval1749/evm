#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- IrIdentifier
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CompilationUnit.h"

#include "../Common/String.h"

#include "./CompileSession.h"
#include "./Functor.h"

#include "./Ir/NamespaceBody.h"
#include "./Ir/NamespaceDef.h"

namespace Compiler {

// ctor
CompilationUnit::CompilationUnit(
    CompileSession& session,
    const String& source_path)
    : ALLOW_THIS_IN_INITIALIZER_LIST(
          global_ns_body_(
              *new NamespaceBody(
                *this,
                nullptr,
                session.global_namespace_def()))),
      session_(session),
      source_path_(source_path) {}

CompilationUnit::~CompilationUnit() {
  DEBUG_PRINTF("%p %ls\n", this, source_path_.value());
}

// [A]
void CompilationUnit::AddSource(const char16* text, size_t text_len) {
 source_.Add(text, text_len);
}

void CompilationUnit::Apply(Functor* const pFunctor) {
  ASSERT(nullptr != pFunctor);
  pFunctor->Process(this);
}

// [F]
void CompilationUnit::FinishSource() {
  source_.Finish();
}

// [G]
String CompilationUnit::GetLine(int i) const {
  return source_.GetLine(i);
}

} // Compiler
