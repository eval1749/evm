#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - UpVarDefI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./UpVarDefI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

UpVarDefI::UpVarDefI() : Base(Ty_Void, Void) {}

UpVarDefI::UpVarDefI(
    const Type& outy,
    PseudoOutput& out,
    const Variable& var)
    : Base(&outy, &out) {
  AppendOperand(&var);
} // UpVarDefI

bool UpVarDefI::IsUseless() const {
    if (this->GetOutput() == Void) {
        return true;
    }

    if (this->GetSx()->StaticCast<Variable>()->m_cUpRefs) {
        return false;
    }

    return Instruction::IsUseless();
} // UpVarDefI::IsUseless

} // Ir
} // Il
