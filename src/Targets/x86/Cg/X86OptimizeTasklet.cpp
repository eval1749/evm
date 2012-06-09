#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Targets - X86 Asm
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86OptimizeTasklet.h"

#include "./X86Defs.h"

namespace Il {
namespace Cg {

using namespace Il::Tasks;

// ctor
X86OptimizeTasklet::X86OptimizeTasklet(
    Session& session,
    const Module& module)
    : X86NormalizeTasklet(session, module) {}

// [P]
//  STATIC_CAST Int32 %r1 <= %r2
//  ADD Int32 %r3 <= %r1 int1
//  LEA T* %r4 <= %r3 int2
//  =>
//  LEA T* %r3 <= %r2 int1+int2
void X86OptimizeTasklet::Process(x86LeaI* pI) {
  ASSERT(pI != nullptr);

  if (auto const pCastI = pI->GetRx()->GetDefI()
          ->DynamicCast<StaticCastI>()) {
    pI->GetOperandBox(0)->Replace(*pCastI->GetSx());
    Add(pI);
    Add(pCastI);
    return;
  }

  auto const pLy = pI->GetLy();
  if (!pLy) return;

  auto const pAddI = pI->GetRx()->GetDefI()->DynamicCast<AddI>();
  if (!pAddI) return;

  auto const pInt = pAddI->GetSx()->DynamicCast<Literal>();
  if (!pInt) return;

  auto const pToIntI = pAddI->GetRx()->GetDefI()->DynamicCast<StaticCastI>();
  if (!pToIntI) return;

  pI->GetOperandBox(0)->Replace(*pToIntI->GetSx());

  pI->GetOperandBox(1)->Replace(
      module().NewLiteral(*Ty_Int32, pLy->GetInt32() + pInt->GetInt32()));

  Add(pI);
  Add(pAddI);
  Add(pToIntI);
}

} // Cg
} // Il
