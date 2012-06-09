#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IrObject
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./IrObject.h"

namespace Il {
namespace Ir {

void IrObject::Apply(Functor* const pFunctor) {
    ASSERT(nullptr != pFunctor);
    pFunctor->Process(this);
} // Appy

} // Ir
} // Il
