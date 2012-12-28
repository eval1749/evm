#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - CleanTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CleanTask.h"

#include "../Ir.h"

namespace Il {
namespace Tasks {

namespace {

static const BBlock& ComputeTargetBlock(const BBlock& bblock) {
  auto const jump_inst = bblock.GetFirstI()->DynamicCast<JumpI>();
  return jump_inst ? *jump_inst->GetLabel()->GetBB() : bblock;
}

static const BBlock* ComputeDiamondBBlock(const BranchI& branch_inst) {
  auto& false_block = ComputeTargetBlock(*branch_inst.GetFalseBB());
  auto& true_block = ComputeTargetBlock(*branch_inst.GetTrueBB());

  return false_block == true_block && false_block.CountInEdges() == 2
      ? &false_block
      : nullptr;
}

} // namespace

// ctor
CleanTask::CleanTask(Session& session, Module& module)
  : Base(L"CleanTask", session, module),
    normalizer_(session, module) {}

// [F]
void CleanTask::FoldBranch(
    const BranchI& branch_inst,
    const BBlock& succ_block) {
  DEBUG_FORMAT("Fold %s => %s", branch_inst, succ_block);
  auto& jump_inst = *new JumpI(succ_block);
  branch_inst.GetBBlock()->ReplaceI(jump_inst, branch_inst);
  HandleJump(jump_inst);
}

// [H]
void CleanTask::HandleBranch(const BranchI& branch_inst) {
  DEBUG_FORMAT("CleanBranch %s", branch_inst);

  auto& true_block = *branch_inst.GetTrueBB();
  auto& false_block = *branch_inst.GetFalseBB();

  if (branch_inst.GetSx() == False) {
    RemovePhis(*branch_inst.GetBBlock(), true_block);
    FoldBranch(branch_inst, false_block);
    SetChanged();
    return;
  }

  if (branch_inst.GetSx() == True) {
    RemovePhis(*branch_inst.GetBBlock(), false_block);
    FoldBranch(branch_inst, true_block);
    SetChanged();
    return;
  }

  if (true_block == false_block) {
    FoldBranch(branch_inst, true_block);
    SetChanged();
    return;
  }

  if (false_block.HasOnlyOnePred() && true_block.HasOnlyOnePred()) {
    auto& bblock = *branch_inst.GetBBlock();

    // Branch+Ret+Ret
    if (false_block.GetFirstI()->Is<RetI>()
            && true_block.GetFirstI()->Is<RetI>()) {
      auto& r1 = module().NewRegister();
      auto& fn = *bblock.GetFunction();

      bblock.InsertBeforeI(
          *new IfI(
              fn.function_type().return_type(),
              r1,
              *branch_inst.GetBx(),
              *true_block.GetFirstI()->GetSx(),
              *false_block.GetFirstI()->GetSx()),
          &const_cast<BranchI&>(branch_inst));

      bblock.ReplaceI(*new RetI(r1), branch_inst);
      fn.RemoveBBlock(false_block);
      fn.RemoveBBlock(true_block);

      SetChanged();
      return;
    }
  }

  // Branch+Jump+Phi
  if (auto const phi_block = ComputeDiamondBBlock(branch_inst)) {
    auto& bblock = *branch_inst.GetBBlock();
    DEBUG_FORMAT("Optimize Diamon Flow %s", bblock);
    auto false_block2 = false_block == *phi_block ? &bblock : nullptr;
    auto true_block2 = true_block == *phi_block ? &bblock : nullptr;

    foreach (BBlock::EnumPred, oEnum, phi_block) {
      auto& pred_block = *oEnum.Get();
      if (pred_block == false_block) {
        false_block2 = &pred_block;

      } else if (pred_block == true_block) {
        true_block2 = &pred_block;
      }
    }

    ASSERT(false_block2 != nullptr);
    ASSERT(true_block2 != nullptr);

    for (;;) {
      auto const phi_inst = phi_block->GetFirstI()->DynamicCast<PhiI>();
      if (phi_inst == nullptr) {
        break;
      }

      auto& if_inst = *new IfI(
          phi_inst->output_type(),
          *phi_inst->GetRd(),
          *branch_inst.GetBx(),
          *phi_inst->GetOperand(true_block2),
          *phi_inst->GetOperand(false_block2));

      bblock.InsertBeforeI(if_inst, &const_cast<BranchI&>(branch_inst));
      const_cast<BBlock*>(phi_block)->RemoveI(*phi_inst);
      normalizer_.Add(if_inst);
    }

    FoldBranch(branch_inst, true_block);
    SetChanged();
  }
}

void CleanTask::HandleJump(const JumpI& jump_inst) {
  DEBUG_FORMAT("CleanJump %s", jump_inst);

  if (jump_inst.GetBBlock()->HasMoreThanOneSucc()) {
    // Maybe OpenTry
    DEBUG_FORMAT("More than one succ %s", jump_inst.GetBBlock());
    return;
  }

  if (TryRemoveEmpty(jump_inst)) {
    SetChanged();
    return;
  }

  if (TryCoalesce(jump_inst)) {
    SetChanged();
    return;
  }

  if (TryJumpPhiRet(jump_inst)) {
    SetChanged();
    return;
  }
}

void CleanTask::HandleSwitch(const SwitchI& switch_inst) {
  DEBUG_FORMAT("CleanSwitch %s", switch_inst);

  auto& default_block = *switch_inst.GetDefaultBB();

  WorkList_<OperandBox> defaults;

  for (auto& case_box: switch_inst.case_boxes()) {
    if (case_box.GetBB() == &default_block)
      defaults.Push(&case_box);
  }

  while (!defaults.IsEmpty()) {
    auto& box = *defaults.Pop()->StaticCast<SwitchOperandBox>();
    DEBUG_FORMAT("remove defualt case %s", box.GetOperand());
    switch_inst.GetBB()->RemoveOutEdge(box.GetBB());
    const_cast<SwitchI&>(switch_inst).RemoveOperand(&box);
    SetChanged();
  }

  if (switch_inst.CountOperands() == 2) {
    DEBUG_FORMAT("Fold %s => %s", switch_inst, default_block);
    SetChanged();
    auto& jump_inst = *new JumpI(&default_block);
    switch_inst.GetBB()->ReplaceI(jump_inst, switch_inst);
    HandleJump(jump_inst);
  }
}

// [P]
void CleanTask::ProcessBBlock(const BBlock& bblock) {
  DEBUG_FORMAT("CleanBBlock %s", bblock);
  auto& last_inst = *bblock.GetLastI();

  if (auto const branch_inst = last_inst.DynamicCast<BranchI>()) {
    normalizer_.Add(branch_inst);
    HandleBranch(*branch_inst);

  } else if (auto const jump_inst = last_inst.DynamicCast<JumpI>()) {
    HandleJump(*jump_inst);

  } else if (auto* switch_inst = last_inst.DynamicCast<SwitchI>()) {
    HandleSwitch(*switch_inst);
  }
}

void CleanTask::ProcessFunction(Function& fn) {
  auto count = 0;

  for (;;) {
    count++;

    DEBUG_FORMAT("Clean[%d] %s", count, fn);
    ResetChanged();

    foreach (Function::EnumBBlockPostorder, bblocks, fn) {
      auto& block = *bblocks.Get();
      if (fn.GetEntryBB() != &block) {
        ProcessBBlock(block);
      }
    }

    normalizer_.Start();
    if (normalizer_.IsChanged()) {
      SetChanged();
      normalizer_.ResetChanged();
    }

    if (!IsChanged()) {
      break;
    }

    WorkList_<BBlock> unreachables;
    for (auto& bblock: fn.bblocks()) {
      if (!bblock.HasInEdge() && fn.GetEntryBB() != &bblock) {
        unreachables.Push(&bblock);
      }
    }

    while (!unreachables.IsEmpty()) {
      fn.RemoveBBlock(*unreachables.Pop());
    }
  }
}

// [R]
void CleanTask::RemovePhis(const BBlock& block, const BBlock& target_block) {
  for (auto& phi_inst: target_block.phi_instructions()) {
    auto& box = *phi_inst.GetOperandBox(&block);
    phi_inst.RemoveOperand(&box);
    normalizer_.Add(phi_inst);
  }
}

// [T]
bool CleanTask::TryCoalesce(const JumpI& jump_inst) {
  auto& curr_block = *jump_inst.GetBBlock();
  ASSERT(!curr_block.HasMoreThanOneSucc());

  auto& succ_block = *jump_inst.GetTargetBB();

  if (succ_block.HasMoreThanOnePred()) {
    DEBUG_FORMAT("%s has more than one in edge.", succ_block);
    return false;
  }

  DEBUG_FORMAT("Coalesce %s => %s", succ_block, curr_block);

  // Fold PHI in succ
  while (auto const phi_inst = succ_block.GetFirstI()->DynamicCast<PhiI>()) {
    if (auto const out = phi_inst->GetSsaOutput()) {
      ReplaceAll(*phi_inst->GetOperand(&curr_block), *out);
    }
    succ_block.RemoveI(*phi_inst);
  }

  // Move instructions from succ to curr
  {
    auto& ref_inst = *curr_block.GetLastI();
    while (auto const inst = succ_block.GetFirstI()) {
      curr_block.MoveBeforeI(*inst, &ref_inst);
    }
    curr_block.RemoveI(ref_inst);
  }

  // Move all out edge from succ to curr
  {
    BBlock::EnumSucc succ_blocks(&succ_block);
    while (!succ_blocks.AtEnd()) {
      auto const succ_succ_block = succ_blocks.Get();
      succ_blocks.Next();
      succ_succ_block->RedirectEdgeFrom(&curr_block, &succ_block);
    }
  }

  succ_block.MoveLabelsTo(curr_block);
  succ_block.GetFunction()->RemoveBBlock(succ_block);

  return true;
}

// return a == 0 && b == 0;
bool CleanTask::TryJumpPhiRet(const JumpI& jump_inst) {
  auto& curr_block = *jump_inst.GetBBlock();
  ASSERT(!curr_block.HasMoreThanOneSucc());

  auto& succ_block = *jump_inst.GetTargetBB();

  auto const ret_inst = succ_block.GetLastI()->DynamicCast<RetI>();
  if (!ret_inst) {
    return false;
  }

  auto const  phi_inst = succ_block.GetFirstI()->DynamicCast<PhiI>();
  if (!phi_inst) {
    return false;
  }

  if (!phi_inst->GetSsaOutput()) {
    return false;
  }

  if (phi_inst->GetNext() != ret_inst) {
    return false;
  }

  DEBUG_FORMAT("Fold Jump+Phi+Ret: %s", phi_inst);

  // Evaluate instructions in Succ block since we remove edge to succ.
  for (auto& inst: succ_block.instructions())
    normalizer_.Add(inst);

  auto& box = *phi_inst->GetOperandBox(&curr_block);
  auto& operand = *box.GetOperand();
  auto& new_ret_inst = *new RetI(operand);
  curr_block.ReplaceI(new_ret_inst, jump_inst);
  normalizer_.Add(new_ret_inst);

  return true;
}

bool CleanTask::TryRemoveEmpty(const JumpI& jump_inst) {
  auto& curr_block = *jump_inst.GetBBlock();
  ASSERT(!curr_block.HasMoreThanOneSucc());

  auto& succ_block = *jump_inst.GetTargetBB();

  if (curr_block.GetFirstI() != &jump_inst) {
    return false;
  }

  if (succ_block.GetFirstI()->Is<PhiI>()) {
    return false;
  }

  // for (defun foo () (foo))
  if (curr_block == succ_block) {
    return false;
  }

  DEBUG_FORMAT("Remove empty %s => %s", curr_block, succ_block);

  // Redirect pred=>curr to pred=>succ
  {
    BBlock::EnumPred pred_blocks(&curr_block);
    while (!pred_blocks.AtEnd()) {
      auto& pred_block = *pred_blocks.Get();
      pred_blocks.Next();
      pred_block.RedirectEdgeTo(&succ_block, &curr_block);
    }
  }

  curr_block.MoveLabelsTo(succ_block);
  curr_block.GetFunction()->RemoveBBlock(curr_block);

  return true;
}

} // Ir
} // Il
