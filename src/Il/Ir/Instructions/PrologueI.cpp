#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./PrologueI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

PrologueI::PrologueI() : Base(Ty_Void, Void) {}

PrologueI::PrologueI(const Values& out) :
    Base(ValuesType::Intern(), out) {}

} // Ir
} // Il
