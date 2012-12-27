#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Functor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./VisitFunctor.h"

#include "./CompilationUnit.h"
#include "./CompileSession.h"

#include "./Ir.h"

namespace Compiler {

void VisitFunctor::Process(AliasDef* const) {}
void VisitFunctor::Process(CastOperator* const) {}

void VisitFunctor::Process(ClassDef* const pClassDef) {
    ASSERT(pClassDef != nullptr);

    foreach (ClassDef::EnumMember, oEnum, *pClassDef) {
        auto const pNameDef = oEnum.Get();
        pNameDef->Apply(this);
    } // for
} // Process ClassDef

void VisitFunctor::Process(CompilationUnit* const pCompilationUnit) {
    ASSERT(pCompilationUnit != nullptr);
    pCompilationUnit->namespace_body().Apply(this);
} // CompilationUnit

void VisitFunctor::Process(CompileSession* const pCompileSession) {
    ASSERT(pCompileSession != nullptr);
    for (auto& unit: pCompileSession->compilation_units())
      unit.Apply(this);
} // Process CompileSession

void VisitFunctor::Process(FieldDef* const) {}
void VisitFunctor::Process(Function* const) {}
void VisitFunctor::Process(Keyword* const) {}
void VisitFunctor::Process(LocalVarDef* const) {}

void VisitFunctor::Process(MethodDef* const pMethodDef) {
    ASSERT(pMethodDef != nullptr);

    foreach (MethodDef::EnumParam, oEnum, *pMethodDef) {
        oEnum.Get()->Apply(this);
    } // for param

    foreach (MethodDef::EnumVar, oEnum, *pMethodDef) {
        oEnum.Get()->Apply(this);
    } // for var
} // Process MethodDef

void VisitFunctor::Process(MethodGroupDef* const pMethodGroupDef) {
    foreach (MethodGroupDef::EnumMethodDef, oEnum, *pMethodGroupDef) {
        oEnum.Get()->Apply(this);
    } // for
} // Process MethodGroupDef

void VisitFunctor::Process(MethodParamDef* const) {}
void VisitFunctor::Process(NameDef* const) {}
void VisitFunctor::Process(ClassOrNamespaceDef* const) {}

void VisitFunctor::Process(NamespaceBody* const pNamespaceBody) {
    ASSERT(pNamespaceBody != nullptr);

    foreach (NamespaceBody::EnumUsingNamespace, oEnum, *pNamespaceBody) {
        oEnum.Get()->Apply(this);
    } // for

    foreach (NamespaceBody::EnumMember, oEnum, *pNamespaceBody) {
        oEnum.Get()->Apply(this);
    } // for
} // Process NamespaceBody

void VisitFunctor::Process(NamespaceDef* const) {}
void VisitFunctor::Process(NamespaceMember* const) {}
void VisitFunctor::Process(Operator* const) {}
void VisitFunctor::Process(RankSpec* const) {}
void VisitFunctor::Process(Type* const) {}
void VisitFunctor::Process(TypeArgList* const) {}
void VisitFunctor::Process(UsingNamespace* const) {}
void VisitFunctor::Process(VarDef* const) {}

} // Compiler
