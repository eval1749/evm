#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - ResolveAliasPass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ResolveAliasPass.h"

namespace Compiler {
// ctor
ResolveAliasPass::ResolveAliasPass(CompileSession* const pCompileSession)
    : Base(L"ResolveAliasPass", pCompileSession) {
}

// [P]
void ResolveAliasPass::Process(AliasDef* const pAliasDef) {
    ASSERT(pAliasDef != nullptr);
    ASSERT(m_pNameMap == nullptr);

    auto const pTargetNameRef = pAliasDef->GetTargetNameRef();

    auto const pOperand = this->Lookup(pTargetNameRef);
    if (pOperand == nullptr) {
        this->AddError(
            pAliasDef->GetSourceInfo(),
            CompileError_Resolve_NotFound,
            pTargetNameRef);
        return;
    } // if

    if (auto const pClass= pOperand->DynamicCast<Class>()) {
        pAliasDef->SetTarget(pClass);
        return;
    } // if

    if (auto const pNamespace = pOperand->DynamicCast<Namespace>()) {
        pAliasDef->SetTarget(pNamespace);
        return;
    } // if

    this->AddError(
        pAliasDef->GetSourceInfo(),
        CompileError_Resolve_ExpectClassOrNamespaceDef,
        pTargetNameRef,
        pOperand);
} // Process

// [S]
void ResolveAliasPass::Start() {
    auto const pSession = this->GetCompileSession();
    pSession->Apply(this);
} // Start

} // Compiler
