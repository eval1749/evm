#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Cfg2SsaTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Cfg2SsaTask.h"

#include "../Analysis/DominanceAnalyzer.h"
#include "./NormalizeTasklet.h"

#include "../Ir.h"

namespace Il {
namespace Tasks {

namespace {

class VarEx : public Variable {
  public: enum Flag {
    Flag_None,

    Flag_Global,
    Flag_Liveness,
    Flag_Local,
  };

  private: VarEx() {}

  public: bool IsGlobal(BBlock* const pBB) const {
    switch (GetFlag()) {
      case Flag_Global:
        return true;

      case Flag_Liveness:
        return pBB->IsLiveIn(GetIndex());

      case Flag_Local:
        return false;
    }
    CAN_NOT_HAPPEN();
  }
  DISALLOW_COPY_AND_ASSIGN(VarEx);
};

class RenameWork :
  public LocalObject,
  public SingleLinkedList_<Operand, RenameWork> {

  public: typedef SingleLinkedList_<Operand, RenameWork> OperandList;

  // [G]
  public: Operand* GetTop() const {
    ASSERT(!IsEmpty());
    return GetFirst()->m_pNode;
  }

  // [I]
  public: bool IsEmpty() const  { return GetFirst() == nullptr; }

  // [P]
  public: Operand* Pop() {
    ASSERT(!IsEmpty());
    auto const pCons = GetFirst();
    auto const pSx = pCons->m_pNode;
    SetFirst(pCons->m_pNext);
    return pSx;
  }
};

class InsertPhiTasklet : public Tasklet {
  private: const Module& module_;

  // ctor
  public: InsertPhiTasklet(Session& session, const Module& module)
    : Tasklet(L"InsertPhi", session), module_(module) {}

  // [I]
  private: void InsertPhi(BBlock* const pBB, Variable* const pVar) {
    auto& out = module_.NewRegister();
    const_cast<Register&>(out).SetVariable(*pVar);
    auto const pPhiI = new PhiI(pVar->GetTy(), out);
    auto const pFirstI = pBB->GetFirstI();

    // Associate this PHI instruction to corresponding variable.
    pPhiI->SetWork(pVar);

    pPhiI->SetSourceInfo(pFirstI->GetSourceInfo());
    pBB->InsertBeforeI(*pPhiI, pFirstI);
  }

  // [P]
  private: void ProcessVar(
      Function* const pFun,
      Variable* const pVar) {

    ASSERT(pFun != nullptr);
    ASSERT(pVar != nullptr);

    ASSERT(pVar->GetOwner() == pFun);

    DEBUG_FORMAT("Process %s", pVar);

    auto const pRcell = pVar->GetRd();

    WorkList_<BBlock> oDoneList;
    oDoneList.Push(pFun->GetExitBB());

    WorkList_<BBlock> oWorkList;

    // Initialize work list
    foreach (BBlock::EnumFrontier, oEnum, pFun->GetEntryBB()) {
      auto const pFrontierBB = oEnum.Get();
      if (!pFrontierBB->IsInList()) {
        oWorkList.Push(pFrontierBB);
      }
    }

    // Add all STORE to work list
    foreach (Register::EnumUser, oEnum, pRcell) {
      auto const pFieldPtrI = oEnum.GetI()->DynamicCast<FieldPtrI>();
      if (!pFieldPtrI) {
          continue;
      }

      foreach (Register::EnumUser, oEnum, pFieldPtrI->GetRd()) {
        auto const pStoreI = oEnum.GetI()->DynamicCast<StoreI>();
        if (!pStoreI) {
          DEBUG_FORMAT("Ignore %s", oEnum.GetI());
          continue;
        }

        auto const pBB = pStoreI->GetBBlock();
        if (pBB->IsInList()) {
          continue;
        }

        DEBUG_FORMAT("Found %s", pStoreI);

        foreach (BBlock::EnumFrontier, oEnum, pBB) {
          auto const pFrontierBB = oEnum.Get();
          if (!pFrontierBB->IsInList()) {
            oWorkList.Push(pFrontierBB);
          }
        }
      }
    }

    while (!oWorkList.IsEmpty()) {
      auto const pBB = oWorkList.Pop();
      if (pVar->Extend<VarEx>()->IsGlobal(pBB)) {
        InsertPhi(pBB, pVar);
      }

      oDoneList.Push(pBB);

      foreach (BBlock::EnumFrontier, oEnum, pBB) {
        auto const pFrontierBB = oEnum.Get();
        if (!pFrontierBB->IsInList()) {
          oWorkList.Push(pFrontierBB);
        }
      }
    }
  }

  // [R]
  public: void Run(Function* const pFun) {
    ASSERT(pFun != nullptr);
    DEBUG_FORMAT("Insert Phi to %s", pFun);
    foreach (Function::EnumVar, oEnum, pFun) {
      auto const pVar = oEnum.Get();
      if (VarEx::Flag_None != pVar->GetFlag()) {
        ProcessVar(pFun, pVar);
      }
    }
  }
  DISALLOW_COPY_AND_ASSIGN(InsertPhiTasklet);
};

/// <summary>
///     This task selects SSA flavor, Semi-Pruned or Pruned, and computes
///     liveness of variables.
///     <list type="number">
///         <item>Number varaible for liveness.</item>
///         <item>Compute liveness</item>
///     </list>
/// </summary>
class PrepareTasklet : public Tasklet {
  private: WorkList_<BBlock> m_oNonlocalBBs;

  public: PrepareTasklet(Session& session)
    : Tasklet(L"Cfg2Ssa.Prepare", session) {}

  // [C]
  private: void ComputeLiveness(Function* const pFun) {
    DEBUG_FORMAT("Compute Full Liveness %s", pFun);

    pFun->ComputeVarLiveness();

    foreach (Function::EnumVar, oEnum, pFun) {
      auto const pVar = oEnum.Get();
      if (pVar->GetStorage() == Variable::Storage_Register) {
        DEBUG_FORMAT("Live %s", pVar);
        pVar->SetFlag(VarEx::Flag_Liveness);
      } else {
        DEBUG_FORMAT("Skip %s", pVar);
      }
    }

    while (!m_oNonlocalBBs.IsEmpty()) {
      auto const pBB = m_oNonlocalBBs.Pop();

      DEBUG_FORMAT("Nonlocal %s", pBB);

      foreach (Function::EnumVar, oEnum, pFun) {
        auto const pVar = oEnum.Get();
        if (pVar->GetFlag() == VarEx::Flag_Liveness) {
          if (pBB->IsLiveIn(pVar->GetIndex())) {
            pVar->SetStorage(Variable::Storage_Stack);
            pVar->SetFlag(VarEx::Flag_None);
            DEBUG_FORMAT("%s is nonlocal at %s", pVar, pBB);
          }
        }
      }
    }
  }

  private: void ComputeSemiPrune(Function* const pFun) {
    DEBUG_FORMAT("Compute Semi-Prune Liveness %s", pFun);

    foreach (Function::EnumVar, oEnum, pFun) {
      auto const pVar = oEnum.Get();

      if (pVar->GetStorage() != Variable::Storage_Register) {
        DEBUG_FORMAT("Skip %s", pVar);
        continue;
      }

      if (auto const pDefI = pVar->GetDefI()) {
        auto const pDefBB = pDefI->GetBBlock();
        foreach (Register::EnumUser, oEnum, pVar->GetRd()) {
          auto const pUseI = oEnum.GetI();
          if (pUseI->GetBBlock() != pDefBB) {
            DEBUG_FORMAT("Global %s", pVar);
            pVar->SetFlag(VarEx::Flag_Global);
            break;
          }
        }
      } else {
        DEBUG_FORMAT("Skip %s", pVar);
      }
    }
  }

  // [R]
  public: void Run(Function* const pFun) {
    ASSERT(pFun != nullptr);
    DEBUG_FORMAT("Select SSA Flavor %s", pFun);
    auto nIndex = 0;

    foreach (Function::EnumVar, oEnum, pFun) {
      auto const pVar = oEnum.Get();

      // Make sure later sub pass doesn't see Flag_Liveness.
      pVar->SetFlag(VarEx::Flag_None);
      pVar->SetWork(nullptr);

      if (!pVar->GetDefI()) {
        continue;
      }

      if (pVar->GetStorage() == Variable::Storage_Register) {
        pVar->SetFlag(VarEx::Flag_Local);
        pVar->SetIndex(nIndex);
        nIndex += 1;
      }
    }

    foreach (Function::EnumBBlock, oEnum, pFun) {
      auto const pBB = oEnum.Get();

      foreach (BBlock::EnumPhiI, oEnum, pBB) {
        auto const pPhiI = oEnum.Get();
        pPhiI->SetWork(nullptr);
      }

      foreach (BBlock::EnumInEdge, oEnum, pBB) {
        if (oEnum.Get()->GetEdgeKind() == CfgEdge::Kind_Nonlocal) {
          m_oNonlocalBBs.Push(pBB);
          break;
        }
      }
    }

    if (m_oNonlocalBBs.IsEmpty()) {
      ComputeSemiPrune(pFun);

    } else {
      ComputeLiveness(pFun);
    }
  }
  DISALLOW_COPY_AND_ASSIGN(PrepareTasklet);
};

class RenameTasklet :
  public Tasklet,
  public Functor,
  public LocalMemoryZone
{
  private: typedef SingleLinkedList_<RenameWork, LocalObject>
      RenameWorkList;

  private: NormalizeTasklet normalizer_;
  private: RenameWorkList* m_pKillList;

  // ctor
  public: RenameTasklet(Session& session, const Module& module)
    : Tasklet(L"Cfg2Ssa.Rename", session),
      normalizer_(session, module) {}

  // [G]
  private: RenameWork* GetRenameWork(Register* const pRx) {
    if (!pRx) return nullptr;

    auto const pFieldPtrI = pRx->GetDefI()->DynamicCast<FieldPtrI>();
    if (!pFieldPtrI) return nullptr;

    auto const pRcell = pFieldPtrI->GetRx();
    if (!pRcell) return nullptr;
    if (!pRcell->GetDefI()) return nullptr;

    auto const pVarDefI = pRcell->GetDefI()->DynamicCast<VarDefI>();
    if (!pVarDefI) return nullptr;

    return pVarDefI->GetSx()->StaticCast<Variable>()
        ->GetWork<RenameWork>();
  }

  // [P]
  public: void Process(LoadI* const pI) {
    auto const pRenameWork = GetRenameWork(pI->GetRx());
    if (!pRenameWork) {
        return;
    }

    normalizer_.Add(pI->GetRd());

    ReplaceAll(*pRenameWork->GetTop(), *pI->GetRd());
    pI->GetBBlock()->RemoveI(*pI);
  }

  public: void Process(PhiI* const pI) {
    auto const pRd = pI->GetRd();
    if (!pRd) {
      return;
    }

    auto const pVar = pRd->GetVariable();
    if (!pVar) {
      return;
    }

    auto const pRenameWork = pVar->GetWork<RenameWork>();
    if (!pRenameWork) {
      return;
    }

    if (pI->GetWork<Variable>() == pVar) {
      normalizer_.Add(pI);
      SetNewName(pRenameWork, pRd);
    }
  }

  public: void Process(StoreI* const pI) {
    auto const pRenameWork = GetRenameWork(pI->GetRx());
    if (!pRenameWork) {
      return;
    }

    SetNewName(pRenameWork, pI->GetSy());
    pI->GetBBlock()->RemoveI(*pI);
  }

  public: void Process(VarDefI* const pI) {
    auto const pVar = pI->GetSx()->StaticCast<Variable>();

    if (pVar->Extend<VarEx>()->GetFlag() == VarEx::Flag_None) {
      return;
    }

    DEBUG_FORMAT("Process %s", pI);

    auto const pRenameWork = new(this) RenameWork;
    pVar->SetWork(pRenameWork);

    normalizer_.Add(pI);
    normalizer_.Add(pI->GetRd());

    SetNewName(pRenameWork, pI->GetSy());
  }

  private: void ProcessBBlock(BBlock* const pBB) {
    DEBUG_FORMAT("Rename %s", pBB);

    RenameWorkList oKillList;
    m_pKillList = &oKillList;

    {
      BBlock::EnumI oEnumI(pBB);
      while (!oEnumI.AtEnd()) {
        auto const pI = oEnumI.Get();
        oEnumI.Next();
        pI->Apply(this);
      }
    }

    foreach (BBlock::EnumSucc, oEnum, pBB) {
      UpdatePhis(pBB, oEnum.Get());
    }

    foreach (BBlock::EnumChild, oEnum, pBB) {
      ProcessBBlock(oEnum.Get());
    }

    // Pop operand stack
    foreach (RenameWorkList::Enum, oEnum, &oKillList) {
      oEnum.Get()->Pop();
    }
  }

  // [R]
  public: void Run(Function* const pFun) {
    ASSERT(pFun != nullptr);
    DEBUG_FORMAT("Rename %s", pFun);
    ProcessBBlock(pFun->GetEntryBB());
    normalizer_.Start();
  }

  // [S]
  private: void SetNewName(
      RenameWork* const pRenameWork,
      Operand* const pSx) {
    pRenameWork->SetFirst(
        new(this) RenameWork::OperandList::Cons(
            pSx,
            pRenameWork->GetFirst()));

    m_pKillList->SetFirst(
        new(this) RenameWorkList::Cons(
            pRenameWork,
            m_pKillList->GetFirst()));
  }

  // [U]
  private: void UpdatePhis(BBlock* const pCurr, BBlock* const pSucc) {
    foreach (BBlock::EnumPhiI, oEnum, pSucc) {
      auto const pPhiI = oEnum.Get();

      auto const pVar = pPhiI->GetWork<Variable>();
      if (!pVar) {
        DEBUG_FORMAT("Skip %s", pPhiI);
        continue;
      }

      auto const pRenameWork = pVar->GetWork<RenameWork>();
      if (!pRenameWork) {
        continue;
      }

      // Useless PHI instruction. It is created by loop.
      if (pRenameWork->IsEmpty()) {
        continue;
      }

      auto const pSx = pRenameWork->GetTop();
      DEBUG_FORMAT("Add (%s %s) to %s", pCurr, pSx, pPhiI);
      pPhiI->AddOperand(pCurr, pSx);
    }
  }
  DISALLOW_COPY_AND_ASSIGN(RenameTasklet);
};

} // namespace

// ctor
Cfg2SsaTask::Cfg2SsaTask(Session& session, Module& module)
  : Base(L"Cfg2Ssa", session, module) {}

// [P]
void Cfg2SsaTask::ProcessFunction(Function& fn) {
  auto const pFun = &fn;
  {
    DominanceAnalyzer analyzer(session());
    analyzer.ComputeDominance(fn);
  }

  {
    PrepareTasklet tasklet(session());
    tasklet.Run(pFun);
  }

  {
    InsertPhiTasklet tasklet(session(), fn.module());
    tasklet.Run(pFun);
  }

  {
    RenameTasklet tasklet(session(), fn.module());
    tasklet.Run(pFun);
  }

  foreach (Function::EnumBBlock, oEnum, pFun) {
    auto const pBBlock = oEnum.Get();
    foreach (BBlock::EnumPhiI, oEnum, pBBlock) {
      auto const pPhiI = oEnum.Get();
      pPhiI->SetWork(nullptr);
    }
  }

  foreach (Function::EnumVar, oEnum, pFun) {
    auto const pVar = oEnum.Get();
    pVar->SetFlag(0);
    pVar->SetWork(nullptr);
  }
}

} // Tasks
} // Il
