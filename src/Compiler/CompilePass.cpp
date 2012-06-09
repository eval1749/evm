#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - CompilePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CompilePass.h"

#include "./CompileSession.h"
#include "./VisitFunctor.h"

#include "./Ir/MethodDef.h"

#include "./Passes/FinalizeClassPass.h"
#include "./Passes/FinalizeEnumPass.h"
#include "./Passes/FinalizeMethodPass.h"
#include "./Passes/FinalizeUsingNamespacePass.h"
#include "./Passes/ResolveClassPass.h"
#include "./Passes/ResolveMethodPass.h"

#include "../Il/Tasks/VerifyTask.h"

namespace Compiler {

namespace {
using Il::Tasks::VerifyTask;

class Verifier : public VisitFunctor {
    private: CompileSession* const m_pSession;

    public: Verifier(CompileSession* const pSession) :
        m_pSession(pSession) {}

    private: void Process(MethodDef* const pMethodDef) {
        auto& module = pMethodDef->module();
        VerifyTask oVerifyTask(*m_pSession,  module);
        oVerifyTask.Start();
    }

    DISALLOW_COPY_AND_ASSIGN(Verifier);
};

static Array_<CompilePass::NewFunction>* s_Passes;

}

// ctor
CompilePass::CompilePass(
    const String& name,
    CompileSession* const session)
    : Task(name, *session),
      compile_session_(*session),
      writer_(nullptr) {
  DEBUG_PRINTF("%p\n", this);
}

CompilePass::~CompilePass() {
  DEBUG_PRINTF("%p\n", this);
}

Collection_<CompilePass::NewFunction> LibExport CompilePass::GetPasses() {
  if (s_Passes == nullptr) {
    NewFunction passv[] = {
      FinalizeUsingNamespacePass::Create,
      ResolveClassPass::Create,
      FinalizeClassPass::Create,
      ResolveMethodPass::Create,
      FinalizeMethodPass::Create,
      FinalizeEnumPass::Create,
    };
    s_Passes = new Array_<NewFunction>(passv, lengthof(passv));
  }
  return Collection_<NewFunction>(*s_Passes);
}

// [V]
bool CompilePass::Verify() {
  auto& session = compile_session();
  if (!session.HasError()) {
    Verifier oVerifier(&session);
    session.Apply(&oVerifier);
  }

  return !session.HasError();
}

}
