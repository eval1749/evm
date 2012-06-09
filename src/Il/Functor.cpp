#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Functor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Ir.h"

namespace Il {
namespace Ir {

void Functor::Process(IrObject*) {}
void Functor::Process(Instruction* p) { Process(static_cast<IrObject*>(p)); }
void Functor::Process(Operand* p) { Process(static_cast<IrObject*>(p)); }

#define FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
  void Functor::Process(mp_Type* p) { \
    Process(static_cast<mp_Base*>(p)); \
  }

#include "./FunctorMethods.inc"

} // Ir
} // Il
