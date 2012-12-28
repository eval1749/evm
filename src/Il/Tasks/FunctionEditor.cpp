#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Tasklet - FunctionEditor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FunctionEditor.h"

#include "../Ir/CfgEdge.h"
#include "../Ir/Instructions.h"
#include "../Ir/Module.h"
#include "../Ir/Operands/Function.h"
#include "../Ir/VmErrorInfo.h"

namespace Il {
namespace Tasks {

// ctor
FunctionEditor::FunctionEditor(Session& session)
    : FunctionTasklet("FunctionEditor", session) {}

// [E]
/// <summary>
///  Eliminates inifnite loop adding pseudo edge from arbitrary block in
///  infinite block to exit block.
/// </summary>
bool FunctionEditor::EliminateInfiniteLoop(Function& fun) {
  class DfsWalker {
    protected: int apply_count_;
    private: Function& function_;

    protected: DfsWalker(Function& fun)
        : apply_count_(0),
          function_(fun) {
      ResetFlags();
    }

    public: ~DfsWalker() {
      ResetFlags();
    }

    // [G]
    public: int GetCount() const { return apply_count_; }

    // [O]
    protected: bool OnApply(BBlock& bblock) {
      if (bblock.GetFlag()) {
          return true;
      }

      bblock.SetFlag(1);
      apply_count_ += 1;
      return false;
    }

    // [R]
    private: void ResetFlags() {
      for (auto& bblock: function_.bblocks())
        bblock.SetFlag(0);
    }

    DISALLOW_COPY_AND_ASSIGN(DfsWalker);
  };

  class BackwardDfs : public DfsWalker {
    public: BackwardDfs(Function& fun)
        : DfsWalker(fun) {
      Apply(*fun.GetExitBB());
    }

    public: void Apply(BBlock& bblock) {
      if (OnApply(bblock)) {
        return;
      }

      foreach (BBlock::EnumInEdge, oEnum, &bblock) {
        Apply(*oEnum.GetNode());
      }
    }

    DISALLOW_COPY_AND_ASSIGN(BackwardDfs);
  };

  class ForewardDfs : public DfsWalker {
    private: ForewardDfs(Function& fun)
        : DfsWalker(fun) {
      Visit(*fun.GetEntryBB());
    }

    public: static int Run(Function& fun) {
      ForewardDfs oForewardDfs(fun);
      return oForewardDfs.GetCount();
    }

    private: void Visit(BBlock& bblock) {
      if (OnApply(bblock)) {
        return;
      }

      foreach (BBlock::EnumOutEdge, oEnum, &bblock) {
        Visit(*oEnum.GetNode());
      }
    }

    DISALLOW_COPY_AND_ASSIGN(ForewardDfs);
  };

  auto const num_forwards = ForewardDfs::Run(fun);

  BackwardDfs backward_dfs(fun);

  if (backward_dfs.GetCount() == num_forwards) {
    return false;
  }

  if (backward_dfs.GetCount() > num_forwards) {
    session().AddErrorInfo(
        VmErrorInfo(
            fun.GetEntryBB()->GetFirstI()->source_info(),
            VmError_Optimize_InternalError,
            "There are unreachable blocks."));
    return false;
  }

  DEBUG_FORMAT("Eliminate Infinite Loop for %s", fun);

  for (;;) {
    DEBUG_FORMAT("back/fore=%d/%d", backward_dfs.GetCount(), num_forwards);

    if (backward_dfs.GetCount() == num_forwards) {
      break;
    }

    if (backward_dfs.GetCount() > num_forwards) {
      session().AddErrorInfo(
          VmErrorInfo(
              fun.GetEntryBB()->GetFirstI()->source_info(),
              VmError_Optimize_InternalError,
              "There are unreachable blocks."));
      return false;
    }

    // Pick a bblock up in infinite loop.
    BBlock* bblock = nullptr;
    foreach (Function::EnumBBlockPostorder, oEnum, fun) {
      if (!oEnum.Get()->GetFlag()) {
        bblock = oEnum.Get();
        break;
      }
    }

    if (!bblock) {
      session().AddErrorInfo(
          VmErrorInfo(
              fun.GetEntryBB()->GetFirstI()->source_info(),
              VmError_Optimize_InternalError,
              "No changes."));
      return false;
    }

    bblock->AddEdge(fun.GetExitBB())->SetEdgeKind(CfgEdge::Kind_Pseudo);
    backward_dfs.Apply(*bblock);
  }
  return true;
}

// [R]
void FunctionEditor::Renumber(Function& fun) {
  auto num_bblocks = 0;
  auto num_insts = 0;
  auto num_outputs = 0;
  for (auto& bblock: fun.bblocks()) {
    bblock.SetName(++num_bblocks);
    for (auto& inst: bblock.instructions()) {
      inst.SetIndex(++num_insts);
      if (auto const out = inst.output().DynamicCast<SsaOutput>()) {
        out->set_name(++num_outputs);
      }
    }
  }
}

void FunctionEditor::RemoveCriticalEdges(Function& fun) {
  class Local {
    public: static bool HasPhiI(const BBlock& bblock) {
      BBlock::EnumPhiI phi_insts(&bblock);
      return !phi_insts.AtEnd();
    }
  };

  DEBUG_FORMAT("%s", fun);

  WorkList_<BBlock> phi_bblocks;

  for (auto& bblock: fun.bblocks()) {
    if (Local::HasPhiI(bblock))
      phi_bblocks.Push(&bblock);
  }

  if (phi_bblocks.IsEmpty()) {
    DEBUG_FORMAT("No critical edges in %s", fun);
  }

  // Note: We don't need to remove critical edge for back edge if
  // phi operands aren't live out in other succs.
  auto& module = fun.module();
  while (!phi_bblocks.IsEmpty()) {
    auto& succ = *phi_bblocks.Pop();
    BBlock::EnumPred succs(succ);
    while (!succs.AtEnd()) {
      auto& pred = *succs;
      succs.Next();
      if (pred.HasMoreThanOneSucc() && succ.HasMoreThanOnePred()) {
        auto& curr = module.NewBBlock();
        pred.GetFunction()->InsertBefore(curr, succ);
        pred.ReplaceEdgeTo(&curr, &succ);
        pred.ReplaceLabels(&curr, &succ);
        succ.ReplacePhiOperands(&curr, &pred);
        curr.AppendI(*new(module.zone()) JumpI(succ));
      }
    }
  }
}

} // Tasks
} // Il
