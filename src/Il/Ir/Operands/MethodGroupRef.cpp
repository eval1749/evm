#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Function
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MethodGroupRef.h"

namespace Il {
namespace Ir {

// ctor
MethodGroupRef::MethodGroupRef(
    Class* const pClass,
    const Name& name) :
        m_pClass(pClass),
        m_pName(pName) {
    ASSERT(nullptr != m_pClass);
    ASSERT(nullptr != m_pName);
} // MethodGroupRef

// [E]
bool MethodGroupRef::Equals(const Operand& another) const {
    auto const that = another.DynamicCast<MethodGroupRef>();
    return nullptr != that
        && this->m_pClass == that->m_pClass
        && this->m_pName == that->m_pName;
} // MethodGroupRef::Equal

} // Ir
} // Il
