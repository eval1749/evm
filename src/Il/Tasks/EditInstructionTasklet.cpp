#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - EditInstructionTasklet
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./EditInstructionTasklet.h"

#include "../Ir/Instructions.h"

namespace Il {
namespace Tasks {

// ctor
EditInstructionTasklet::EditInstructionTasklet(
  const String& name,
  Session& session,
  const Module& module)
  : EditTasklet(name, session), module_(module) {}

EditInstructionTasklet::~EditInstructionTasklet() {
  ASSERT(m_oWorkList.IsEmpty());
}

// [A]
void EditInstructionTasklet::Add(Instruction& inst) {
  if (!inst.IsInList()) {
    DEBUG_FORMAT("%s.Add(%s)", name(), inst);
    m_oWorkList.Push(&inst);
  }
}

void EditInstructionTasklet::Add(Output& output) {
  if (auto const r1 = output.DynamicCast<SsaOutput>()) {
    Add(r1);
  }
}

void EditInstructionTasklet::Add(SsaOutput& rx) {
  DEBUG_FORMAT("%s.Add(%s)", name(), rx);

  auto count = 0;
  foreach (Register::EnumUser, users, &rx) {
    auto& user = *users.GetI();
    DEBUG_FORMAT("add %s", user);
    Add(user);
    ++count;
  }

  if (!count) {
    DEBUG_FORMAT("no use sites for %s", rx);
  }
}

// [M]
void EditInstructionTasklet::MakeEmpty() {
  m_oWorkList.MakeEmpty();
} // MakeEmpty

// [S]
void EditInstructionTasklet::Start() {
  DEBUG_FORMAT("%s", name());

  while (!m_oWorkList.IsEmpty()) {
      auto const pI = m_oWorkList.Pop();
      DEBUG_FORMAT("Edit %s", pI);
      if (pI->GetBBlock() == nullptr) {
          DEBUG_FORMAT("%s is removed.", pI);
      } else {
          EditInstruction(pI);
      } // if
  } // while
} // Start

void EditInstructionTasklet::SwapOperands(
  Instruction& inst,
  int const iX,
  int const iY) {
  ASSERT(iX != iY);

  DEBUG_FORMAT("Swap operand[%d] and operand[%d] of %s",
      iX, iY, inst);

  auto const pBoxX = inst.GetOperandBox(iX);
  auto const pBoxY = inst.GetOperandBox(iY);
  auto const pSx = pBoxX->GetOperand();
  auto const pSy = pBoxY->GetOperand();
  pBoxX->SetOperand(pSy);
  pBoxY->SetOperand(pSx);
}

} // Tasks
} // Il
