#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - FieldPtrI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FieldPtrI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

FieldPtrI::FieldPtrI() : Base(Ty_Void, Void) {}

FieldPtrI::FieldPtrI(
    const Register& out,
    const Operand& src,
    const Field& field)
    : Base(PointerType::Intern(field.storage_type()), out) {
  AppendOperand(src);
  AppendOperand(field);
}

} // Ir
} // Il
