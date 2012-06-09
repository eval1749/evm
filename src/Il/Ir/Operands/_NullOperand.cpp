#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - NullOperand
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NullOperand.h"

#include "./Type.h"
#include "./TypeVar.h"

namespace Il {
namespace Ir {

HashMap_<PtrKey_<Type>, NullOperand*>* NullOperand::s_pHashMap;

// ctor
NullOperand::NullOperand(Type* const pType) :
    m_pType(pType) {
    ASSERT(m_pType != nullptr);
} // NullOperand

// [G]
Type* NullOperand::GetTy() const {
    return m_pType->GetBoundType();
} // GetTy

// [I]
NullOperand* NullOperand::Intern(Type* const pType) {
    // TODO 2011-05-15 eval1749@ Lock NullOperand::Intern
    if (s_pHashMap == nullptr) {
        s_pHashMap = new HashMap_<PtrKey_<Type>, NullOperand*>();
    } // if

    if (auto const pSx = s_pHashMap->Get(pType)) {
        return pSx;
    } // if

    auto const pNull = new NullOperand(pType);
    s_pHashMap->Add(pType, pNull);
    return pNull;
} // Intern

} // Ir
} // Il
