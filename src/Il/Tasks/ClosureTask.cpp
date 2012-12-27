#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - ClosureTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ClosureTask.h"

#include "../Ir.h"

namespace Il {
namespace Tasks {

namespace {

class VarEx : public Variable {
  public: enum Usage {
      Usage_None      = 0,
      Usage_Read      = 1 << 0,
      Usage_Write     = 1 << 1,
  }; // Usage
  
  // ctor
  private: VarEx() {}

  // [G]
  public: Usage GetUsage() const
      { return static_cast<Usage>(GetFlag()); }

  // [R]
  public: void ResetUsage() {
      SetFlag(Usage_None);
      SetStorage(Storage_Register);
  } // ResetUsage

  // [S]
  public: void SetUsage(Usage e) { SetFlag(e); }

  DISALLOW_COPY_AND_ASSIGN(VarEx);
};

// Description:
//  This pass does following:
//    1 Collect upvars
//    2 Remove unreachable functions
class FunUsageTask : public Tasklet {
  public: static const char* GetName_() { return "Closure"; }

  private: typedef WorkList_<Variable> VarList;

  public: FunUsageTask(Session& session) :
      Tasklet(L"FunUsage", session) {}

  // [A]
  private: void AddUpVars(Function* const pFun, VarList* const pVars) {
      foreach (VarList::Enum, oEnum, pVars) {
          auto const pVar = oEnum.Get();
          if (pVar->GetOwner() != pFun) {
              pFun->InternUpVar(pVar);
          }
      } // for each upvar
  } // AddUpVars

  // [P]
  private: void ProcessScc(Function* const pLeader) {
      DEBUG_FORMAT("Scc %d %s", pLeader->m_nSccNum, pLeader);

      WorkList_<Function> oFuns;
      foreach (Function::EnumSccMember, oEnum, pLeader) {
          auto const pFun = oEnum.Get();
          if (pFun->IsInList()) {
              continue;
          } // if

          oFuns.Push(pFun);
          foreach (Function::EnumOutEdge, oEnum, pFun) {
              auto const pCallee = oEnum.GetNode();
              if (pCallee->IsInList()) continue;
              if (pCallee->m_pSccId == pLeader) continue;
              oFuns.Push(pCallee);
          } // for each callee
      } // for each member

      WorkList_<Variable> oUpVars;
      while (!oFuns.IsEmpty()) {
          auto const pFun = oFuns.Pop();
          foreach (Function::EnumUpVar, oEnum, pFun) {
              auto const pVar = oEnum.Get();
              if (!pVar->IsInList()) {
                  oUpVars.Push(pVar);
              }
          } // for each upvar
      } // while

      foreach (Function::EnumSccMember, oEnum, pLeader) {
          auto const pFun = oEnum.Get();

          DEBUG_FORMAT("Scc Member %s", pFun);

          AddUpVars(pFun, &oUpVars);
          // TODO 2011-01-23 yosi@msn.com Do we need to update function
          // return value?
          //pFun->UpdateValueTy();

          // Mark closed variable. from inner functions.
          if (pFun->IsClosure()) {
              foreach (Function::EnumUpVar, oEnum, pFun) {
                  auto const pVar = oEnum.Get()->Extend<VarEx>();
                  if (VarEx::Storage_Stack == pVar->GetStorage()) {
                      if (pVar->GetUsage() & VarEx::Usage_Write) {
                          DEBUG_FORMAT("Closed %s", pVar);
                          pVar->SetStorage(VarEx::Storage_Closed);

                      } else {
                          DEBUG_FORMAT("Literal %s", pVar);
                          pVar->SetStorage(VarEx::Storage_Literal);
                      } // if
                  } // if
              } // for
          } // if closure
      } // for each member
  } // ProcessScc

  // [R]
  public: void Run(Module& module) {
      module.ComputeScc();

      foreach (Module::EnumScc, oEnum, &module) {
          ProcessScc(oEnum.Get());
      } // for each fun

      // Note: Removing unused funciton is done in Pass destructor?
      {
          WorkList_<Function> oUnreachables;
          for (auto& fun: module.functions()) {
            if (!fun.m_nSccNum)
              oUnreachables.Push(&fun);
          } // for each fun

          while (!oUnreachables.IsEmpty()) {
              auto const pFun = oUnreachables.Pop();
              module.RemoveFunction(*pFun);
          } // while
      }
  } // Run

  DISALLOW_COPY_AND_ASSIGN(FunUsageTask);
};

/// <summary>
///     Change instructions with variable storage.
/// </summary>
class VarStorageTask : public Tasklet {
  private: typedef WorkList_<Variable> VarList;

  public: VarStorageTask(Session& session) :
      Tasklet(L"VarStorage", session) {}

  private: static Class* ComputeStorageType(Variable* const pVar) {
      ASSERT(nullptr != pVar);

      auto& type = pVar->GetTy();

      switch (pVar->GetStorage()) {
      case VarEx::Storage_Closed:
          return &Ty_ClosedCell->Construct(type);

      case VarEx::Storage_Literal:
          return &Ty_LiteralCell->Construct(type);

      case VarEx::Storage_Register:
      case VarEx::Storage_Stack:
          return &Ty_StackCell->Construct(type);

      default:
          COMPILER_INTERNAL_ERROR();
          return nullptr;
      } // switch storage
  } // ComputeStorageType

  // [R]
  private: void RewriteSlots(
      VarEx* const pVar,
      Instruction* const pVarDefI) {

      DEBUG_FORMAT("Rewrite %s for %s", pVarDefI, pVar);

      auto const pStorageType = ComputeStorageType(pVar);
      if (nullptr == pStorageType) return;

      pVarDefI->set_output_type(*pStorageType);

      auto const pField = pStorageType->Find(*Q_value)
          ->StaticCast<Field>();

      foreach (SsaOutput::EnumUser, oEnum, pVarDefI->GetSsaOutput()) {
          if (FieldPtrI* pFieldPtrI =
                  oEnum.Get()->GetI()->DynamicCast<FieldPtrI>()) {
              DEBUG_FORMAT("%s => %s", pFieldPtrI, pField);
              pFieldPtrI->GetOperandBox(1)->SetOperand(pField);
          }
      } // RewriteSlots
  } // RewriteSlots

  // [R]
  public: void Run(Module& module) {
      for (auto& fun: module.functions()) {
          foreach (Function::EnumVar, oEnum, fun) {
              auto const pVar = oEnum.Get()->Extend<VarEx>();
              RewriteSlots(pVar, oEnum.GetI());
          } // for each var

          foreach (Function::EnumUpVar, oEnum, fun) {
              auto const pVar = oEnum.Get()->Extend<VarEx>();
              RewriteSlots(pVar, oEnum.GetI());
          } // for each upvar
      } // for each fun
  } // Run

  DISALLOW_COPY_AND_ASSIGN(VarStorageTask);
};

class VarUsageTask : public Tasklet {
  public: VarUsageTask(Session& session) :
      Tasklet(L"VarUsage", session) {}

  // [C]
  private: void ComputeUsage(VarEx* const pVar, SsaOutput* const pRd) {
      foreach (SsaOutput::EnumUser, oEnum, pRd) {
          auto const pRp = oEnum.GetI()->GetRd();
          foreach (Register::EnumUser, oEnum, pRp) {
              if (oEnum.GetI()->Is<StoreI>()) {
                  pVar->SetUsage(VarEx::Usage_Write);
                  DEBUG_FORMAT("%s is written.", pVar);
                  break;
              }
          } // for each user
      } // for each user
  } // computeUsage

  // [P]
  private: void ProcessFun(Function* const pFun) {
      DEBUG_FORMAT("Process %s", pFun);

      foreach (Function::EnumVar, oEnum, pFun) {
          auto const pVar = oEnum.Get()->Extend<VarEx>();
          ComputeUsage(pVar, pVar->GetRd());
      } // for each var

      auto fClosure = pFun->IsClosure();

      foreach (Function::EnumUpVar, oEnum, pFun) {
          auto const pVar = oEnum.Get()->Extend<VarEx>();
          ComputeUsage(pVar, oEnum.GetI()->GetQd());

          if (fClosure) {
              if (pVar->GetUsage() & VarEx::Usage_Write) {
                  DEBUG_FORMAT("Closed %s", pVar);
                  pVar->SetStorage(VarEx::Storage_Closed);

              } else {
                  DEBUG_FORMAT("Literal %s", pVar);
                  pVar->SetStorage(VarEx::Storage_Literal);
              }

          } else if (VarEx::Storage_Register == pVar->GetStorage()) {
              DEBUG_FORMAT("Stack %s", pVar);
              pVar->SetStorage(VarEx::Storage_Stack);
          }
      } // for each var
  } // processFun

  // [R]
  public: void Run(Module& module) {
      for (auto& fun: module.functions()) {
          foreach (Function::EnumVar, oEnum, fun) {
              auto const pVar = oEnum.Get()->Extend<VarEx>();
              pVar->ResetUsage();
          } // for eah var
      } // for each fun

      for (auto& fun: module.functions()) {
        ProcessFun(&fun);
      } // for each fun
  } // run

  DISALLOW_COPY_AND_ASSIGN(VarUsageTask);
};

} // namespace

// ctor
ClosureTask::ClosureTask(Session& session, Module& module)
    : Base(L"Closure", session, module) {}

// [P]
void ClosureTask::ProcessFunction(Function& fn) {
  auto const pFun = &fn;
  foreach (Function::EnumVar, oEnum, pFun) {
      auto const pVar = oEnum.Get();
      pVar->SetFlag(0);
      pVar->SetWork(nullptr);
  } // for var
} // ProcessFunction

// [S]
void ClosureTask::Start() {
  {
      VarUsageTask oTask(session());
      oTask.Run(module());
  }
  {
      FunUsageTask oTask(session());
      oTask.Run(module());
  }

  {
      VarStorageTask oTask(session());
      oTask.Run(module());
  }

  Base::Start();
} // Start

} // Tasks
} // Il
