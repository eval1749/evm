#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Functor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86Functor.h"

namespace Il {
namespace Cg {

using namespace Il::Ir;

void X86Functor::Process(X86Instruction* p) {
  CgFunctor::Process(p);
}

void X86Functor::Process(CgInstruction* p) {
  if (auto const x86inst = p->DynamicCast<X86Instruction>()) {
    x86inst->Apply(this);
  } else {
    CgFunctor::Process(p);
  }
}

void X86Functor::Process(Instruction* p) {
  CgFunctor::Process(p);
}

void X86Functor::Process(Operand* p) {
  if (auto const x86operand = p->DynamicCast<X86Operand>()) {
    x86operand->Apply(this);
  } else {
    CgFunctor::Process(p);
  }
}

#define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
  DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format)

#define DEFINE_TARGET_INSTRUCTION( \
    mp_name, mp_arity, mp_family, mp_format) \
  void X86Functor::Process(mp_name ##I* p) { \
    ASSERT(p != nullptr); \
    Process(static_cast<mp_family ## Instruction*>(p)); \
  }

#define FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
  void X86Functor::Process(mp_Type* const p) { \
    ASSERT(p != nullptr); \
    CgFunctor::Process(p); \
  }

#include "../../../Il/FunctorMethods.inc"

#define CG_FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
  void X86Functor::Process(mp_Type* const p) { \
    ASSERT(p != nullptr); \
    CgFunctor::Process(p); \
  }

#include "../../../Cg/CgFunctorMethods.inc"

#define X86_FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
  void X86Functor::Process(mp_Type* const p) { \
    ASSERT(p != nullptr); \
    Process(static_cast<mp_Base*>(p)); \
  }

#include "./X86FunctorMethods.inc"

} // Ir
} // Il
