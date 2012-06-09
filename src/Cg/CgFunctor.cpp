#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Functor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CgFunctor.h"

#include "./CgDefs.h"

namespace Il {
namespace Cg {

using namespace Il::Ir;

void CgFunctor::Process(CgInstruction* p) {
  Functor::Process(p);
}

void CgFunctor::Process(Instruction* p) {
  if (auto const q = p->DynamicCast<CgInstruction>()) {
    q->Apply(this);
  } else {
    Functor::Process(p);
  }
}

void CgFunctor::Process(Operand* p) {
  if (auto const q = p->DynamicCast<CgImmediate>()) {
    q->Apply(this);
  } else if (auto const q = p->DynamicCast<CgOutput>()) {
    q->Apply(this);
  } else if (auto const q = p->DynamicCast<CgPseudoOutput>()) {
    q->Apply(this);
  } else {
    Functor::Process(p);
  }
}

#define CG_FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
  void CgFunctor::Process(mp_Type* const p) { \
    ASSERT(p != nullptr); \
    Process(static_cast<mp_Base*>(p)); \
  }

#define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_desc) \
  CG_FUNCTOR_METHOD_2_(mp_name ## I, mp_family ## Instruction)

#define FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
  void CgFunctor::Process(mp_Type* p) { Functor::Process(p); }

#include "../Il/FunctorMethods.inc"

#include "./CgFunctorMethods.inc"

} // Cg
} // Il
