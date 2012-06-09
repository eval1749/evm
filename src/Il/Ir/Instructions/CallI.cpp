#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CallI.h"

#include "./FunctionOperandBox.h"

#include "../Name.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

CallI::CallI() : Base(Ty_Void, Void) {}

CallI::CallI(
  const Type& outy,
  const Output& output,
  const Operand& callee,
  const Values& args)
  : Base(outy, output),
    m_pFrameReg(nullptr) {
  if (auto const fn = callee.DynamicCast<Function>()) {
    AppendOperand(new FunctionOperandBox(fn));
  } else {
    AppendOperand(callee);
  }

  AppendOperand(args);
}

const ValuesType& CallI::args_type() const {
  return *GetVy()->GetDefI()->output_type().StaticCast<ValuesType>();
}

/// <summary>
///   Returns true if function is Voidy.
/// </summary>
bool CallI::IsUseless() const {
  return output() == *Useless;
}

} // Ir
} // Il
