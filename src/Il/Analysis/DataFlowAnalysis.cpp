#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Data Flow Analysis
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./DataFlowAnalysis.h"

#include "../Ir/BBlock.h"
#include "../Ir/CfgEdge.h"
#include "../Ir/Instructions.h"
#include "../Ir/Operands/Function.h"

#include "../Tasks/Tasklet.h"

namespace Il {
namespace Ir {

using Il::Io::XhtmlWriter;

bool BBlock::IsLiveIn(int k) const {
  return Extend<DataFlowBB>()->GetIn().IsOne(k);
}

bool BBlock::IsLiveOut(int k) const {
  return Extend<DataFlowBB>()->GetOut().IsOne(k);
}

void BBlock::SetLiveIn(int k) {
  Extend<DataFlowBB>()->GetIn().SetOne(k);
}

void BBlock::SetLiveOut(int k) {
  Extend<DataFlowBB>()->GetOut().SetOne(k);
}

void DataFlowBB::InitDataFlow(uint const cRegs) {
  if (m_pDfData == nullptr) {
    m_pDfData = new DataFlowData();
  }

  m_pDfData->in_vec_.Adjust(cRegs).FillZero();
  m_pDfData->kill_vec_.Adjust(cRegs).FillZero();
  m_pDfData->out_vec_.Adjust(cRegs).FillZero();
}


/// <summary>
///   Solve DFA equation backward.
///   <code>
///     LiveIn[B]  := UEVar[B] union (LiveOut[B] - VarKill[B])
///     LiveOut[B] := union LiveIn[S] for all succ
///         UEVar[B]   = Upward Exposed Variable in B
///         VarKill[B] = Variable killed in B
///   </code>
///   <list>
///     <item>Note: We use LiveKill[exit] for work bitvec.</item>
///     <item>Note: All bblock in pFun must be reachable.</item>
///   </list>
/// </summary>
void Dfa::SolveBackward(const Function& fun) {
  BitVec work(fun.GetExitBB()->Extend<DataFlowBB>()->GetKill().Count());

  auto changed = true;
  auto count = 0;
  while (changed) {
    ++count;

    changed = false;
    foreach (Function::EnumBBlockPreorderReverse, oEnum, fun) {
      auto const pCurr = oEnum.Get()->Extend<DataFlowBB>();

      foreach (BBlock::EnumOutEdge, oEnum, pCurr) {
          auto const pSucc = oEnum.Get()->GetTo()
              ->Extend<DataFlowBB>();

          pCurr->GetOut().Ior(pSucc->GetIn());
      }

      work.Copy(pCurr->GetOut());
      work.AndC2(pCurr->GetKill());
      work.Ior(pCurr->GetIn());

      if (!pCurr->GetIn().Equals(work)) {
        pCurr->GetIn().Copy(work);
        changed = true;
      }
    }
  }

  //fun.WriteLog("SolveBackward %p %d iteration", fun, count);

  if (fun.GetEntryBB()->Extend<DataFlowBB>()->GetIn().FindOne() >= 0) {
    CAN_NOT_HAPPEN();
    //COMPILER_INTERNAL_ERROR();
  }
}

namespace DfaPrivate {

class RegLiveness {
  // Entry Point
  public: static void Run(Function& fun) {
    RegList oRegList;

    auto cRegs = 0;
    foreach (Function::EnumReg, oEnum, fun) {
      auto const pRx = oEnum.Get();
      pRx->SetIndex(cRegs);
      cRegs += 1;
      oRegList.Append(pRx);
    }

    if (cRegs == 0) {
      for (auto& bblock: fun.bblocks())
        bblock.Extend<DataFlowBB>()->InitDataFlow(0);
      return;
    }

    for (auto& bblock: fun.bblocks()) {
      auto& data_flow_block = *bblock.Extend<DataFlowBB>();
      data_flow_block.InitDataFlow(cRegs);
      ComputeLocal(data_flow_block);
      //DumpLiveness(bblock, &oRegList);
    }

    Dfa::SolveBackward(fun);
  }

  // [C]
  //  Computes VarKill[b] and UEVar[b] for specified bblock.
  private: static void ComputeLocal(DataFlowBB& bblock) {
    auto it = bblock.instructions().begin();
    const auto it_end = bblock.instructions().end();

    // Process Phi instruction.
    while (it != it_end) {
      auto const pPhiI = it->DynamicCast<PhiI>();
      if (!pPhiI)
        break;

      if (auto const pRd = pPhiI->GetRd()) {
        bblock.SetKill(pRd->GetIndex());
      }

      ++it;
    }

    // Process non-Phi instruction.
    while (it != it_end) {
      auto& inst = *it;
      ++it;

      for (auto& operand: inst.operands())
        MarkUse(bblock, operand.DynamicCast<Register>());

      if (auto const pRd = inst.GetRd()) {
        bblock.SetKill(pRd->GetIndex());
      }
    }

    // Mark Phi operands in succs
    foreach (BBlock::EnumSucc, oEnum, &bblock) {
      auto const pSucc = oEnum.Get();
      for (auto& phi_inst: pSucc->phi_instructions()) {
        if (auto const pRx = phi_inst.GetRx(&bblock)) {
          MarkUse(bblock, pRx);
        }
      }
    }
  }

  // [M]
  private: static void MarkUse(DataFlowBB& bblock, Register* const pRx) {
    if (pRx == nullptr) {
      return;
    }

    if (!bblock.IsKill(pRx->GetIndex())) {
      bblock.SetIn(pRx->GetIndex());
    }
  }

  DISALLOW_COPY_AND_ASSIGN(RegLiveness);
}; // RegLiveness

class VarLiveness {
  // Entry Point
  public: static void Run(Function& fun) {
    uint cVars = 0;
    for (auto& var: fun.variables()) {
      var.SetIndex(cVars);
      cVars += 1;
    }

    if (cVars == 0) {
      for (auto& bblock: fun.bblocks())
        bblock.Extend<DataFlowBB>()->InitDataFlow(0);
      return;
    }

    for (auto& bblock: fun.bblocks()) {
      auto& data_flow_block = *bblock.Extend<DataFlowBB>();
      data_flow_block.InitDataFlow(cVars);
      ComputeLocal(data_flow_block);
    }

    Dfa::SolveBackward(fun);
  }

  // [C]
  // ComputeLocal
  //  Computes VarKill[b] and UEVar[b] for specified bblock.
  private: static void ComputeLocal(DataFlowBB& bblock) {
    for (auto& inst: bblock.instructions()) {
      if (inst.Is<LoadI>()) {
        if (Variable* const pVar = mapRegToVar(inst.GetRx())) {
          if (!bblock.IsKill(pVar->GetIndex())) {
            bblock.SetIn(pVar->GetIndex());
          }
        }
      } else if (inst.Is<StoreI>()) {
        if (Variable* const pVar = mapRegToVar(inst.GetRx())) {
          bblock.SetKill(pVar->GetIndex());
        }
      } else if (inst.Is<VarDefI>()) {
        bblock.SetKill(inst.variable().GetIndex());
      }
    }
  }

  // [M]
  private: static Variable* mapRegToVar(Register* const pRx) {
    auto const pFieldPtrI = pRx->GetDefI()->DynamicCast<FieldPtrI>();
    if (pFieldPtrI == nullptr) {
        return nullptr;
    }

    auto const pRz = pFieldPtrI->GetRz();
    if (pRz == nullptr) {
        return nullptr;
    }

    auto const pVarDefI = pRz->GetDefI()->DynamicCast<VarDefI>();
    if (pVarDefI == nullptr) {
        return nullptr;
    }

    return &pVarDefI->variable();
  }

  private: static void MarkUse(DataFlowBB& bblock, Variable* const pVar) {
    auto const k = pVar->GetIndex();
    if (!bblock.IsKill(k)) {
      bblock.SetIn(k);
    }
  }

  DISALLOW_COPY_AND_ASSIGN(VarLiveness);
}; // VarLiveness

}

using namespace DfaPrivate;

bool Function::ComputeLiveness() {
  RegLiveness::Run(*this);
  return true;
}

bool Function::ComputeVarLiveness() {
  VarLiveness::Run(*this);
  return true;
}

} // Ir
} // Il
