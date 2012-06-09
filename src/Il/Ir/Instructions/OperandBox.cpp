#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - OperandBox
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./OperandBox.h"

#include "../Operands/Operand.h"
#include "../Operands/Register.h"

namespace Il {
namespace Ir {

// ctor
OperandBox::OperandBox(const Operand* p) { 
  init(p); 
} // OperandBox

// [R]
void OperandBox::Replace(const Operand& src) {
  m_pOperand->Unrealize(this);
  m_pOperand = &const_cast<Operand&>(src);
  m_pOperand->Realize(this);
}

// [G]
Register* OperandBox::GetRx() const {
  return GetOperand()->DynamicCast<Register>();
}

// [S]
void OperandBox::SetOperand(const Operand& operand) {
  m_pOperand = &const_cast<Operand&>(operand);
}

} // Ir
} // Il
