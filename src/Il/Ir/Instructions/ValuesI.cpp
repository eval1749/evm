#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ValuesI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

ValuesI::ValuesI() : Base(Ty_Void, Void) {}

ValuesI::ValuesI(const Values& out) :
    Base(ValuesType::Intern(), out) {}

ValuesI::ValuesI(const Type& outy, const Values& out) : Base(outy, out) {}

ValuesI::ValuesI(const Values& out, const Operand& src0)
    : Base(ValuesType::Intern(src0.GetTy()), out) {
  ASSERT(!src0.Is<Values>());
  AppendOperand(src0);
}

ValuesI::ValuesI(
    const Type& outy,
    const Values& out,
    const Operand& src0)
    : Base(outy, out) {
  ASSERT(!src0.Is<Values>());
  AppendOperand(src0);
}

ValuesI::ValuesI(const Values& out, const Operand& src0, const Operand& src1)
    : Base(ValuesType::Intern(src0.GetTy(), src1.GetTy()), out) {
  ASSERT(!src0.Is<Values>());
  ASSERT(!src1.Is<Values>());
  AppendOperand(src0);
  AppendOperand(src1);
}

ValuesI::ValuesI(const Values& out, const ValuesI& vals_inst)
    : Base(vals_inst.output_type(), out) {
  for (auto& operand: vals_inst.operands()) {
    AppendOperand(operand);
  }
}

// [G]
const ValuesType& ValuesI::GetValuesTy() const {
  return *output_type().StaticCast<ValuesType>();
} // GetValuesTy

// [U]
void ValuesI::UpdateOutputType() {
  ValuesTypeBuilder builder(CountOperands());
  for (auto& operand: operands())
    builder.Append(operand.type());
  set_output_type(builder.GetValuesType());
}

} // Ir
} // Il
