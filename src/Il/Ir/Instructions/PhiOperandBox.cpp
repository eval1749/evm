#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL - Phi Operand Box
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./PhiOperandBox.h"

#include "../BBlock.h"

namespace Il {
namespace Ir {

PhiOperandBox::PhiOperandBox(
    BBlock* const pBB,
    Operand* const pSx) :
        m_pBB(pBB) {
    this->init(pSx);
} // PhiOperandBox

} // Ir
} // Il
