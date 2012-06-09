#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Operands - Register
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Register.h"
#include "./Variable.h"

namespace Il {
namespace Ir {

// ctor
Register::Register(
    RegClass const eClass,
    const Variable* const pVar) :
        m_eRegClass(eClass),
        m_pPhysical(nullptr),
        m_pSpill(nullptr),
        m_pVariable(pVar) {
} // Register

// [S]
void Register::SetVariable(const Variable& var) {
  ASSERT(m_pVariable == nullptr);
  m_pVariable = &var;
}

} // Ir
} // Il
