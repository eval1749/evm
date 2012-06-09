#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL - Switch Operand Box
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./SwitchOperandBox.h"

#include "../BBlock.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
// Note: We take Operand instead of Literal for FASL.
SwitchOperandBox::SwitchOperandBox(
    BBlock* const pBB,
    Operand* const pLx) :
        m_pLabel(&pBB->label()) {
    this->init(pLx);
} // SwitchOperandBox

// [G]
BBlock* SwitchOperandBox::GetBB() const {
    return this->GetLabel()->GetBB();
} // GetBB

// [S]
void SwitchOperandBox::SetBB(BBlock* const pBB) {
    ASSERT(pBB != nullptr);
    this->m_pLabel = &pBB->label();
} // SetBB

} // Ir
} // Il
