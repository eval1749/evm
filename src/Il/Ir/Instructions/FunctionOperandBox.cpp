#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FunctionOperandBox.h"

#include "../Operands/Function.h"

namespace Il {
namespace Ir {

FunctionOperandBox::FunctionOperandBox(Function* const pFunction) :
        OperandBox(pFunction) {}
} // Ir
} // Il
