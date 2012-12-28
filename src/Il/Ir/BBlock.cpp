#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Function
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./BBlock.h"

#include "./CfgEdge.h"
#include "./Module.h"

#include "./Instructions.h"

#include "../Analysis/DataFlowAnalysis.h"

#include "./Operands/Function.h"
#include "./Operands/Label.h"

#include "../../Common/Int32.h"
#include "../../Common/IntPtr.h"
#include "../../Common/StringBuilder.h"

namespace Il {
namespace Ir {

// PhiInstructionRange::Iterator
PhiI& BBlock::PhiInstructionRange::Iterator::operator*() {
  return *iterator_->StaticCast<PhiI>();
}

BBlock::PhiInstructionRange::Iterator BBlock::PhiInstructionRange::end() const {
  auto it = bblock_->instructions().begin();
  auto end = bblock_->instructions().end();
  while (it != end) {
    if (!it->Is<PhiI>())
      break;
    ++it;
  }
  return Iterator(it);
}

// BBlock
BBlock::BBlock(int const name)
  : m_iName(name),
    m_pDfData(nullptr),
    m_pLoopNode(nullptr) {
  m_oLabels.Append(new Label(this));
} // BBlock

BBlock::~BBlock() {
  delete m_pDfData;
}

// [A]
void BBlock::AppendI(const Instruction& inst) {
  Instructions::Append(&const_cast<Instruction&>(inst));
  const_cast<Instruction&>(inst).Realize();
}

void BBlock::AddLabel(Label& label) {
  ASSERT(!label.GetBB());
  label.SetBB(this);
  m_oLabels.Append(&label);
}

// [D]
/// <summary>
///   Returns true if this block dominates specified block.
/// </summary>
bool BBlock::DoesDominate(const BBlock& that) const {
  auto runner = &that;
  for (;;) {
    if (this == runner) {
      return true;
    }

    runner = runner->m_pDomInfo->GetParent();
    if (!runner) {
      return false;
    }
  }
}

// [G]
Instruction* BBlock::GetFirstI() const
  { return static_cast<const Instructions*>(this)->GetFirst(); }

Instruction* BBlock::GetLastI() const
  { return static_cast<const Instructions*>(this)->GetLast(); }

// [I]
void BBlock::InsertAfterI(
    const Instruction& inst,
    const Instruction* const ref_inst) {

  if (ref_inst) {
    if (!inst.source_info().HasInfo()) {
      inst.SetSourceInfo(&ref_inst->source_info());
    }

    Instructions::InsertAfter(
        &const_cast<Instruction&>(inst),
        const_cast<Instruction*>(ref_inst));

  } else {
    Instructions::Prepend(&const_cast<Instruction&>(inst));
  }

  const_cast<Instruction&>(inst).Realize();
}

void BBlock::InsertAfterI(
    const Instruction& inst,
    const Instruction& ref_inst) {
  InsertAfterI(inst, &ref_inst);
}

void BBlock::InsertBeforeI(
  const Instruction& inst, 
  const Instruction* const ref_inst) {
  if (!ref_inst) {
    AppendI(inst);
    return;
  }

  if (!inst.GetSourceInfo()->HasInfo()) {
    inst.SetSourceInfo(ref_inst->GetSourceInfo());
  }

  Instructions::InsertBefore(
    &const_cast<Instruction&>(inst), 
    const_cast<Instruction*>(ref_inst));
  const_cast<Instruction&>(inst).Realize();
}

void BBlock::InsertBeforeI(
    const Instruction& inst,
    const Instruction& ref_inst) {
  InsertBeforeI(inst, &ref_inst);
}

bool BBlock::IsExitBB() const {
  auto const pI = GetLastI();
  return nullptr != pI && pI->Is<ExitI>();
} // IsExitBB

// [M]
void BBlock::MoveBeforeI(
    const Instruction& inst,
    const Instruction* const ref_inst) {
  auto& new_bblock = ref_inst ? *ref_inst->GetBB() : *this;
  auto& old_bblock = *inst.GetBBlock();
  if (auto const jump_inst = inst.DynamicCast<JumpI>()) {
    jump_inst->GetTargetBB()->RedirectEdgeFrom(&new_bblock, &old_bblock);

  } else if (auto const branch_inst = inst.DynamicCast<BranchI>()) {
    branch_inst->GetTrueBB()->RedirectEdgeFrom(&new_bblock, &old_bblock);
    branch_inst->GetFalseBB()->RedirectEdgeFrom(&new_bblock, &old_bblock);

  } else if (auto const term_inst = inst.DynamicCast<TerminateInstruction>()) {
    GetFunction()->GetExitBB()->RedirectEdgeFrom(&new_bblock, &old_bblock);
  }

  old_bblock.Instructions::Delete(&const_cast<Instruction&>(inst));

  if (!ref_inst) {
    new_bblock.Instructions::Append(&const_cast<Instruction&>(inst));
    return;
  }

  new_bblock.Instructions::InsertBefore(
      &const_cast<Instruction&>(inst),
      const_cast<Instruction*>(ref_inst));
}

void BBlock::MoveLabelsTo(BBlock& bblock) {
  while (auto const label = m_oLabels.GetFirst()) {
    RemoveLabel(*label);
    bblock.AddLabel(*label);
  }
}

// [R]
/// <summary>
///   Redirect one edge from pOldBB to this bblock to pNewBB.
/// </summary>
/// <param name="pNewBB">A new start block.</param>
/// <param name="pOldBB">An old start block.</param>
void BBlock::RedirectEdgeFrom(
    BBlock* const pNewBB,
    BBlock* const pOldBB) {
  auto const pBBlock = this;
  pBBlock->ReplaceEdgeFrom(pNewBB, pOldBB);
  pBBlock->ReplacePhiOperands(pNewBB, pOldBB);
} // BBlock::RedirectEdgeFrom

/// <summary>
///   Redirect one edge to pOldBB to this bblock to pNewBB.
/// </summary>
/// <param name="pNewBB">A new destination block.</param>
/// <param name="pOldBB">An old destination block.</param>
void BBlock::RedirectEdgeTo(
  BBlock* const pNewBB,
  BBlock* const pOldBB) {

  auto const pBBlock = this;

  pBBlock->ReplaceEdgeTo(pNewBB, pOldBB);

  for (auto& phi_inst: pOldBB->phi_instructions())
    phi_inst.RemoveOperand(phi_inst.GetOperandBox(pBBlock));

  pBBlock->ReplaceLabels(pNewBB, pOldBB);
} // RedirectEdgeTo

void BBlock::RemoveI(const Instruction& inst) {
  const_cast<Instruction&>(inst).Unrealize();
  Instructions::Delete(&const_cast<Instruction&>(inst));
}

void BBlock::RemoveLabel(Label& label) {
  ASSERT(label.GetBB() == this);
  m_oLabels.Delete(&label);
  label.SetBB(nullptr);
}

void BBlock::ReplaceI(
    const Instruction& new_inst, 
    const Instruction& old_inst) {
  ASSERT(old_inst.GetBB() == this);
  if (!new_inst.HasSourceInfo()) {
      new_inst.SetSourceInfo(old_inst.GetSourceInfo());
  }
  InsertBeforeI(new_inst, old_inst);
  RemoveI(old_inst);
}

void BBlock::ReplaceLabels(
    BBlock* const pNewBB,
    BBlock* const pOldBB) {
  ASSERT(nullptr != pNewBB);
  ASSERT(nullptr != pOldBB);
  auto const pBBlock = this;
  for (auto& box: pBBlock->GetLastI()->operand_boxes()) {
    if (auto const label = box.operand().DynamicCast<Label>()) {
      if (label->GetBB() == pOldBB)
        box.SetOperand(&pNewBB->label());
    }
  }
}

void BBlock::ReplacePhiOperands(
  BBlock* const pNewBB, 
  BBlock* const pOldBB) {

  ASSERT(nullptr != pNewBB);
  ASSERT(nullptr != pOldBB);

  auto const pBBlock = this;

  for (auto& phi_inst: pBBlock->phi_instructions()) {
    // Operand must be found. If not, it is invalid IR.
    auto pBox = phi_inst.GetOperandBox(pOldBB);
    pBox->SetBB(pNewBB);
  }
}

void BBlock::Reset() {
  SetFlag(0);
  SetIndex(0);
  SetWork(nullptr);
} // Reset

// [S]
/// <summary>
///   Split this bblock before specified instruction.
/// </summary>
/// <returns>A newly created block contains pRefI</returns>
/// <param name="pRefI">This block is split after this instruction.</param>
BBlock* BBlock::SplitBefore(Instruction* const pRefI) {
  ASSERT(nullptr != pRefI);

  auto const pBBlock = this;

  ASSERT(pRefI->GetBBlock() == pBBlock);

  auto pAfterBB = &GetFunction()->module().NewBBlock();

  pBBlock->GetFunction()->InsertAfter(*pAfterBB, *pBBlock);

  // Move instructions to new bblock
  {
      auto pRunnerI = pRefI;
      while (nullptr != pRunnerI) {
          auto const pNextI = pRunnerI->GetNext();
          pAfterBB->MoveBeforeI(*pRunnerI, nullptr);
          pRunnerI = pNextI;
      } // while
  }

  for (;;) {
      BBlock::EnumSucc oEnum(pBBlock);
      if (oEnum.AtEnd()) {
          break;
      }
      oEnum.Get()->RedirectEdgeFrom(pAfterBB, pBBlock);
  } // while

  auto& zone = GetFunction()->module().zone();
  pBBlock->AppendI(*new(zone) JumpI(pAfterBB));

  return pAfterBB;
} // BBlock::SplitBefore

// [T]
String BBlock::ToString() const {
  return String::Format("BB%d", name());
}

} // Ir
} // Il
