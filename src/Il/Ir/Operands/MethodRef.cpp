#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- MethodRef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MethodRef.h"

#include "../Namespace.h"

namespace Il {
namespace Ir {

// ctor
MethodRef::MethodRef(
    MethodGroup* const pMethodGroup,
    int const iModifiers,
    FunctionType* const pFunctionType,
    const SourceInfo* const pSourceInfo) :
        Base(pSourceInfo),
        modifiers_(iModifiers),
        m_pFunctionType(pFunctionType),
        m_pMethodGroup(pMethodGroup) {
    ASSERT(nullptr != m_pFunctionType);
    ASSERT(nullptr != m_pMethodGroup);
} // MethodRef

// [G]
Class* MethodRef::GetClass() const {
    return m_pMethodGroup->GetClass();
} // GetClass

Name* MethodRef::name() const {
    return m_pMethodGroup->name();
} // GetName

Namespace* MethodRef::GetNamespace() const {
    return m_pMethodGroup->GetNamespace();
} // GetNamespace

} // Ir
} // Il
