#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Cg - Register Allocation Pass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./RegAllocPass.h"

#include "../../Il/Analysis/DataFlowAnalysis.h"
#include "../../Il/Analysis/DominanceAnalyzer.h"

#include "../../Il/Ir/CfgEdge.h"
#include "../../Il/Ir/BBlock.h"
#include "../../Il/Ir/Operands.h"
#include "../../Il/Ir/Instructions.h"
#include "../../Il/Ir/VmErrorInfo.h"

#include "../../Il/Tasks/FunctionEditor.h"
#include "../../Il/Tasks/Tasklet.h"

#include "../../Common/LocalMemoryZone.h"

#include "./StackManager.h"

namespace Il {
namespace Cg {

using Il::Io::LogWriter;
using Il::Io::XhtmlWriter;

namespace CgRa {

typedef Common::LocalMemoryZone Mm;
typedef Common::Allocable LocalObject;

namespace Really {
  enum Value {
    MayBe,
    Yes,
  };
}

enum BBlockHasCall {
   HasNoCall,
   HasCall,
};

enum RegAcrossCall {
  NotAcrossCall,
  AcrossCall,
};

/// <summary>
///   Map from physical register or RegDesc to RegMap::Entry.
/// </summary>
class RegMap : public LocalObject {
  public: struct Entry {
    Really::Value   m_eReally;
    int             m_iNextUse;
    Register*       m_pRx;
    Physical*       m_pPx;
  };

  private: uint   m_c;
  private: Entry* m_prgoEntry;

  // ctor
  public: RegMap(Mm* pMm, const CgTarget& target) {
    m_c = target.GetFprGroup()->m_pAll->m_c
        + target.GetGprGroup()->m_pAll->m_c;

    m_prgoEntry = reinterpret_cast<Entry*>(
        pMm->Alloc(sizeof(Entry) * m_c));

    Load(target.GetFprGroup());
    Load(target.GetGprGroup());
  }

  // [C]
  public: void Copy(const RegMap* pRegMap) {
    Entry* p = m_prgoEntry;
    foreach (RegMap::Enum, oEnum, pRegMap) {
      *p++ = *oEnum.Get();
    }
  }

  // [E]
  public: class Enum {
    private: Entry* m_pEnd;
    private: Entry* m_pRunner;

    public: Enum(const RegMap* p)
        : m_pRunner(const_cast<RegMap*>(p)->m_prgoEntry),
          m_pEnd(const_cast<RegMap*>(p)->m_prgoEntry + p->m_c) {}

    public: bool AtEnd() const { return m_pEnd == m_pRunner; }
    public: Entry* Get() const { ASSERT(!AtEnd()); return m_pRunner; }
    public: void Next() { ASSERT(!AtEnd()); m_pRunner++; }
  };

  // [G]
  public: Entry* Get(const Physical* const pPx) const {
    return Get(pPx->GetDesc());
  }

  public: Entry* Get(const RegDesc* const pReg) const {
    return &m_prgoEntry[pReg->m_nIndex];
  }

  // [L]
  public: void Load(const RegGroup* pRegGroup) {
    foreach (RegSet::Enum, oEnum, pRegGroup->m_pFree){
      auto const pEntry = Get(oEnum.Get());
      pEntry->m_pPx = CgTarget::Get().GetPhysical(oEnum.Get());
      pEntry->m_eReally = Really::Yes;
    }
  }
};

class RegPair
    : public LocalObject,
      public DoubleLinkedItem_<RegPair> {

  public: Really::Value const m_eReally;
  public: Physical* const m_pPx;
  public: Register* const m_pRx;

  public: RegPair(
      Physical* const pPx,
      Register* const pRx,
      Really::Value const e) :
      m_eReally(e), m_pPx(pPx), m_pRx(pRx) {}

  DISALLOW_COPY_AND_ASSIGN(RegPair);
};

typedef DoubleLinkedList_<RegPair> RegPairs;

struct BBlockExt : LocalObject {
  RegMap*     m_pRegMap;
  RegPairs    m_oLiveIn;
};

static void dumpLiveness(BBlock* pBB, RegList* pRegList) {
  ASSERT(pBB != nullptr);
  ASSERT(pRegList != nullptr);
#if 0
  auto const pLogWriter = pBB->session()()->GetLogWriter();

  if (!pLogWriter) {
      return;
  }

  pLogWriter->StartElement("table", "border", "1", "cellpadding", "3");

  // TODO 2011-01-07 yosi@msn.com DataFlowAnalysis.cpp has same code
  // fragment.
  {
    pLogWriter->StartElement("tr");
    pLogWriter->WriteElement("td", "LiveIn");
    pLogWriter->StartElement("td");

    auto cRegs = 0;
    foreach (RegList::Enum, oEnum, pRegList) {
      auto const pRx = oEnum.Get();
      if (pBB->IsLiveIn(pRx->GetIndex())) {
        pLogWriter->Write(pRx);
        cRegs += 1;
      }
    }

    if (!cRegs) {
      pLogWriter->WriteElement("i", "None");
    }

    pLogWriter->EndElement("td");
    pLogWriter->EndElement("tr");
  }

  {
    pLogWriter->StartElement("tr");
    pLogWriter->WriteElement("td", "LiveOut");
    pLogWriter->StartElement("td");

    auto cRegs = 0;
    foreach (RegList::Enum, oEnum, pRegList) {
        auto const pRx = oEnum.Get();
        if (pBB->IsLiveOut(pRx->GetIndex())) {
            pLogWriter->Write(pRx);
            cRegs += 1;
        }
    }

    if (0 == cRegs) {
        pLogWriter->WriteElement("i", "None");
    }

    pLogWriter->EndElement("td");
    pLogWriter->EndElement("tr");
  }

  pLogWriter->EndElement("table");
#endif
}

/// <summary>
///   Allocate physical register to virtual register. Results are stored into
///   OperandBox.
/// </summary>
class SubPassAllocate
    : public Tasklet,
      protected CgFunctor {
  private: Function* m_pFun;
  private: RegMap* m_pRegMap;
  private: RegList* m_pRegList;
  private: CgSession& session_;
  private: StackManager stack_manager_;

  // ctor
  private: SubPassAllocate(
      CgSession& session,
      Function* const pFun,
      RegList* const pRegList)
      : Tasklet(L"Ra.Allocate", session),
        m_pFun(pFun),
        m_pRegList(pRegList),
        session_(session),
        stack_manager_(session){
    DEBUG_PRINTF("%p\n", this);
  }

  public: virtual ~SubPassAllocate() {
    DEBUG_PRINTF("%p\n", this);
  }

  // [A]
  private: Physical* Allocate(
      Instruction* const pI,
      Register* const pRx) {
    if (auto const pPx = AllocateAux(pRx)) {
      return pPx;
    }

    auto const reg_group = RegGroupOf(*pRx);

    // No available register
    DEBUG_FORMAT("spilling %s", pRx);

    RegMap::Entry* pToSpill =
        m_pRegMap->Get(reg_group.m_pAllocable->m_prgpReg[0]);

    ComputeNextUse(pToSpill, pI);

    foreach (RegSet::Enum, oEnum, reg_group.m_pAllocable) {
      auto const pEntry = m_pRegMap->Get(oEnum.Get());

      if (pToSpill->m_iNextUse < ComputeNextUse(pEntry, pI)) {
        pToSpill = pEntry;
      }
    }

    if (pToSpill->m_iNextUse == pI->GetIndex()) {
      DEBUG_FORMAT("No available register. %s", pI);
      COMPILER_INTERNAL_ERROR();
    }

    if (pI->Is<PhiI>()
        && pI->GetRd() == pRx
        && pToSpill->m_pRx->GetDefI()->Is<PhiI>()) {
      DEBUG_FORMAT("make unbound %s", pToSpill->m_pRx);
      pToSpill->m_pRx->m_pPhysical = nullptr;
    }

    Spill(pToSpill);
    assign(pToSpill, pRx);
    return pToSpill->m_pPx;
  }

  private: Physical* AllocateAux(Register* const pRx) {
    RegMap::Entry* pFree = nullptr;
    auto& reg_group = RegGroupOf(*pRx);
    foreach (RegSet::Enum, oEnum, reg_group.m_pAllocable) {
      auto const pEntry = m_pRegMap->Get(oEnum.Get());

      if (!pEntry->m_pPx) {
        continue;
      }

      if (pRx == pEntry->m_pRx) {
        DEBUG_FORMAT("%s is %s", pRx, pEntry->m_pPx);
        return pEntry->m_pPx;
      }

      if (!pEntry->m_pRx) {
        if (pRx->m_pPhysical == pEntry->m_pPx) {
          pFree = pEntry;

        } else if (!pFree) {
          pFree = pEntry;
        }
      }
    }

    if (pFree) {
      assign(pFree, pRx);
      return pFree->m_pPx;
    }

    return nullptr;
  }

  // For PhiI and ValuesI operands
  private: void AllocateIfPossible(OperandBox* const pBox) {
    if (auto const pRx = ToCandidate(pBox->GetRx())) {
      if (auto const pPx = findPhysical(pRx)) {
        DEBUG_FORMAT("%s %s", pRx, pPx);
        pBox->m_pPhysical = pPx;

      } else if (!pRx->m_pSpill) {
        // Parallel copy requires source operand in register.
        Spill(pRx);
      }
    }
  }

  private: Physical* AllocateRd(
      Instruction* const pI,
      Register* const pRx) {
    if (auto const pAssignI = pI->GetPrev()
            ? pI->GetPrev()->DynamicCast<AssignI>()
            : nullptr) {
      auto const pPx = pAssignI->GetRd()->m_pPhysical;

      foreach (RegMap::Enum, oEnum, m_pRegMap) {
          RegMap::Entry* pEntry = oEnum.Get();
          if (pEntry->m_pPx == pPx)
          {
              assign(pEntry, pRx);
              return pPx;
          }
      }

      DEBUG_FORMAT("Failed to assign %s", pRx);
      COMPILER_INTERNAL_ERROR();
    }

    if (pI->Is<CopyI>()) {
      // Try to allocate %rd to %rx.
      if (auto const pR2 = pI->GetRx()){
        auto const pPx = pR2->m_pPhysical;
        foreach (RegMap::Enum, oEnum, m_pRegMap) {
          auto const pEntry = oEnum.Get();
          if (pEntry->m_pPx == pPx){
            assign(pEntry, pRx);
            return pPx;
          }
        }
      }
    }

    return Allocate(pI, pRx);
  }

  private: Physical* AllocateRx(
      Instruction* const pI,
      Register* const pRx) {
    return Allocate(pI, pRx);
  }

  private: void assign(RegMap::Entry* const pEntry, Register* const pRx) {
    pEntry->m_eReally = Really::Yes;
    pEntry->m_pRx = pRx;
    DEBUG_FORMAT("assign %s to %s", pRx, pEntry->m_pPx);
  }

  private: StackSlot& AllocateStackSlot(
      RegClass const reg_class,
      int const size,
      int const align) {
    auto const offset = stack_manager_.Allocate(size, align);
    // TODO(yosi) 2012-03-25 We should get stack alignment from somewhere.
    auto const stack_align = sizeof(void*);
    auto const next_offset = RoundUp(offset + size, stack_align);
    m_pFun->m_cbFrame = max(m_pFun->m_cbFrame, uint(next_offset));
    DEBUG_FORMAT("StackSlot: offset=%d next=%d", offset, next_offset);
    return *new StackSlot(reg_class, offset, size);
  }

  // [C]
  private: int ComputeNextUse(
      RegMap::Entry* const pEntry,
      Instruction* const pRefI) {

    auto const pRx = pEntry->m_pRx;
    auto const pRefBB = pRefI->GetBBlock();
    auto iIndex = 0;

    Instruction* pUseI = nullptr;

    foreach (Register::EnumUser, oEnum, pRx) {
      auto const pI = oEnum.Get()->GetI();
      if (pRefBB->DoesDominate(*pI->GetBBlock())) {
        if (pI->GetIndex() > pRefI->GetIndex()) {
          if (!pUseI || iIndex > pI->GetIndex()) {
            iIndex = pI->GetIndex();
            pUseI = pI;
          }
        }
      }
    }

    if (!pUseI) {
      DEBUG_FORMAT("%s is no more used", pRx);
      iIndex = 0;

    } else {
      DEBUG_FORMAT("next use of %s is %d: %s",
        pRx, pUseI->GetIndex(), pUseI);
    }

    return pEntry->m_iNextUse = iIndex;
  }

  // [D]
  private: void dumpMap() {
#if 0
    auto const pLogWriter = session()()->GetLogWriter();

    if (!pLogWriter) {
      return;
    }

    DEBUG_FORMAT("Register Map");

    foreach (RegSet::Enum, oEnum, target_.GetGprGroup()->m_pAllocable) {
      const RegMap::Entry* const pEntry = m_pRegMap->Get(oEnum.Get());
      if (!pEntry->m_pRx) {
        continue;
      }

      DEBUG_FORMAT("%s %s %s",
          pEntry->m_pRx,
          pEntry->m_eReally ? "is" : "maybe",
          pEntry->m_pPx);
    }
#endif
  }

  // [F]
  private: Physical* findPhysical(Register* const pRx) {
    auto& reg_group = RegGroupOf(*pRx);
    foreach (RegSet::Enum, oEnum, reg_group.m_pAllocable) {
      auto const pEntry = m_pRegMap->Get(oEnum.Get());
      if (!pEntry->m_pPx) {
        continue;
      }

      if (pRx == pEntry->m_pRx) {
        return pEntry->m_pPx;
      }
    }
    return nullptr;
  }

  // [H]
  private: void HandleCallerSave(
      const CallI& call_inst,
      const RegGroup& reg_group) {
    foreach (RegSet::Enum, oEnum, reg_group.m_pCallerSave) {
      auto const pEntry = m_pRegMap->Get(oEnum.Get()); 

      if (auto const pRx = pEntry->m_pRx) {
        if (IsUsedAfter(call_inst, pRx)) {
          Spill(pEntry);
        } else {
          DEBUG_FORMAT("release %s from %s", pEntry->m_pPx, pRx);
        }
        pEntry->m_pRx = nullptr;
      }
    }
  }

  // [I]
  private: bool IsCallerSave(Register* const pRx) {
    auto& reg_group = RegGroupOf(*pRx);
    foreach (RegSet::Enum, oEnum, reg_group.m_pArgs) {
      auto const pEntry = m_pRegMap->Get(oEnum.Get());
      if (pEntry->m_pRx == pRx){
        return true;
      }
    }
    return false;
  }

  private: bool IsUsedAfter(
      const Instruction& inst,
      Register* const pRx) {
    auto const pBB = inst.GetBBlock();

    if (pBB->IsLiveOut(pRx->GetIndex())) {
      DEBUG_FORMAT("%s is live out.", pRx);
      return true;
    }

    if (NextUser(inst, *pRx)) {
      return true;
    }

    DEBUG_FORMAT("%s is no more used.", pRx);
    return false;
  }

  // [N]
  private: Instruction* NextUser(
      const Instruction& inst,
      const Register& reg) {
    auto const pBB = inst.GetBBlock();
    foreach (Register::EnumUser, oEnum, &reg) {
      Instruction* const pUserI = oEnum.Get()->GetI();
      if (pUserI->GetBBlock() == pBB &&
          pUserI->GetIndex() > inst.GetIndex()) {
        DEBUG_FORMAT("%s is used at %s", reg, pUserI);
        return pUserI;
      }
    }
    return nullptr;
  }

  // [P]
  private: void PrepareBBlock(BBlock* const pBB) {
    auto num_preds = 0;
    foreach (BBlock::EnumInEdge,  oEnum, pBB) {
      auto const pEdge = oEnum.Get();

      DEBUG_FORMAT("in edge %s", pEdge);

      if (pEdge->GetEdgeKind() == CfgEdge::Kind_Nonlocal) {
        foreach (RegMap::Enum, oEnum, m_pRegMap) {
          oEnum.Get()->m_pRx = nullptr;
        }
        break;
      }

      BBlock* const pPredBB = pEdge->GetFrom();

      if (!num_preds) {
        m_pRegMap->Copy(pPredBB->GetWork<BBlockExt>()->m_pRegMap);

      } else {
        auto const pRegMap2 =
            pPredBB->GetWork<BBlockExt>()->m_pRegMap;

        foreach (RegMap::Enum, oEnum, m_pRegMap) {
          auto const pEntry1 = oEnum.Get();
          if (pEntry1->m_pPx && pEntry1->m_pRx) {
            auto const pEntry2 = pRegMap2->Get(pEntry1->m_pPx);
            if (pEntry1->m_pRx != pEntry2->m_pRx) {
              pEntry1->m_pRx = nullptr;
            } else if (pEntry2->m_eReally == Really::MayBe) {
              pEntry1->m_eReally = Really::MayBe;
            }
          }
        }
      }

        num_preds += 1;
    }

    RegPairs* pLiveIn = &pBB->GetWork<BBlockExt>()->m_oLiveIn;

    {
      DEBUG_FORMAT("Inherited registers %s", pBB);
      foreach (RegMap::Enum, oEnum, m_pRegMap) {
        RegMap::Entry* const pEntry = oEnum.Get();

        if (!pEntry->m_pRx || !pEntry->m_pPx) {
            continue;
        }

        DEBUG_FORMAT("%s %s %s",
            pEntry->m_pRx,
            pEntry->m_eReally == Really::Yes ? "is" : "maybe",
            pEntry->m_pPx);

        if (pBB->IsLiveIn(pEntry->m_pRx->GetIndex())) {
          pLiveIn->Append(
              new RegPair(
                  pEntry->m_pPx,
                  pEntry->m_pRx,
                  pEntry->m_eReally));
        } else {
          pEntry->m_pRx = nullptr;
        }
      }
    }

    {
      DEBUG_FORMAT("LiveIn %s", pBB);

      stack_manager_.Reset();

      foreach (RegList::Enum, oEnum, m_pRegList){
        auto const pRx = oEnum.Get();
        if (!pBB->IsLiveIn(pRx->GetIndex())) {
          continue;
        }

        if (auto const slot = pRx->m_pSpill) {
          DEBUG_FORMAT("assign stack slot for %s to %s+%s",
              pRx,
              slot->offset(),
              slot->size());
          if (!stack_manager_.Assign(slot->offset(), slot->size())) {
            session().AddErrorInfo(
                VmErrorInfo(
                    SourceInfo(__FILE__, __LINE__, 0),
                    VmError_Optimize_InternalError,
                    String::Format("overlapped stack slot %d+%d",
                      slot->offset(),
                      slot->size())));
            return;
          }
        }

        if (pRx->GetRegClass() == RegClass_Frame) {
          continue;
        }

        if (pRx->m_pSpill) {
          DEBUG_FORMAT("spilled %s", pRx);

        } else {
          ASSERT(pRx->m_pPhysical != nullptr);
          auto const pEntry = m_pRegMap->Get(pRx->m_pPhysical);

          if (pRx == pEntry->m_pRx) {
            DEBUG_FORMAT("%s %s %s",
                pEntry->m_pRx,
                pEntry->m_eReally == Really::Yes ? "is" : "maybe",
                pEntry->m_pPx);

          } else if (!pEntry->m_pRx) {
            DEBUG_FORMAT("%s maybe %s",
                pRx, pEntry->m_pPx);

            pEntry->m_eReally = Really::MayBe;
            pEntry->m_pRx = pRx;

            pLiveIn->Append(
                new RegPair(
                    pEntry->m_pPx,
                    pEntry->m_pRx,
                    Really::MayBe));

          } else {
            session().AddErrorInfo(
                VmErrorInfo(
                    SourceInfo(__FILE__, __LINE__, 0),
                    VmError_Optimize_InternalError,
                    String::Format("Broken RegMap entry %d %d %s %s",
                        pEntry->m_eReally,
                        pEntry->m_iNextUse,
                        pEntry->m_pRx,
                        pEntry->m_pPx)));
          }
        }
      }
    }
  }

  private: void ProcessBBlock(BBlock* const pBB) {
    {
      DEBUG_FORMAT("Process %s", pBB);

      dumpLiveness(pBB, m_pRegList);

      m_pRegMap = pBB->GetWork<BBlockExt>()->m_pRegMap;

      PrepareBBlock(pBB);

      for (auto& inst: pBB->instructions()) {
        DEBUG_FORMAT("process %d: %s", inst.GetIndex(), inst);
        inst.Apply(this);
      }

      dumpMap();

      foreach (BBlock::EnumSucc, oEnum, pBB) {
        auto const pSuccBB = oEnum.Get();

        DEBUG_FORMAT("process succ %s", pSuccBB);

        for (auto& phi_inst: pSuccBB->phi_instructions())
          AllocateIfPossible(phi_inst.GetOperandBox(pBB));
      }
    }

    foreach (BBlock::EnumChild, oEnum, pBB) {
      auto const pChild = oEnum.Get();
      ProcessBBlock(pChild);
    }
  }

  // SubPassAllocte Instruction handlers
  public: virtual void Process(CallI* const pI) override {
    // Register callee is handled specially. It is done by another pass
    // before RA.
    ASSERT(!pI->GetRx());

    HandleCallerSave(*pI, *session_.target().GetGprGroup());
    HandleCallerSave(*pI, *session_.target().GetFprGroup());
    ProcessOutput(pI);
  }

  public: virtual void Process(CloseI* const pI) override {
    auto& close_inst = *pI;
    auto& frame_reg = *close_inst.GetRx();
    if (!NextUser(close_inst, frame_reg)
        && !close_inst.GetBBlock()->IsLiveOut(frame_reg.GetIndex())) {
      auto& stack_slot = *frame_reg.m_pSpill;
      DEBUG_FORMAT("release %s of %s", stack_slot, close_inst);
      stack_manager_.Release(stack_slot.offset());
    }
  }

  public: virtual void Process(Instruction* const pI) override {
    for (auto& box: pI->operand_boxes())
      UpdateOperand(&box);

    for (auto& box: pI->operand_boxes())
      ReleaseIfNotUsed(&box);

    ProcessOutput(pI);
  }

  // Allocate stack for frame.
  public: virtual void Process(OpenInstruction* const pI) {
    auto& open_inst = *pI;
    auto& frame_reg = *open_inst.output().StaticCast<FrameReg>();
    auto const frame_size = session_.target().ComputeFrameSize(frame_reg);
    auto& stack_slot = AllocateStackSlot(
        RegClass_Frame,
        frame_size,
        sizeof(void*));
    frame_reg.m_pSpill = &stack_slot;
    DEBUG_FORMAT("allocate %s for %s", stack_slot, open_inst);
  }

  public: virtual void Process(PhiI* const pI) override {
    // Process all Phis if we are at the last PHI
    if (pI->GetNext()->Is<PhiI>()) {
      return;
    }

    // Allocates register for Phi outptus
    for (auto& phi_inst: pI->bblock().phi_instructions())
      ProcessOutput(&phi_inst);
  }

  /// <summary>
  ///   Processes VALUES instruction:
  ///   <list>
  ///     <item><description>
  ///       We process Values instruction by parallel copy.
  ///     </description></item>
  ///     <item><description>
  ///       We can't allocate register for VALUES instruction since it can
  ///       exceeds number of allocatable registers.
  ///     </description></item>
  ///   </list>
  /// </summary>
  public: virtual void Process(ValuesI* const pI) override {
    for (auto& box: pI->operand_boxes())
      AllocateIfPossible(&box);

    for (auto& box: pI->operand_boxes())
      ReleaseIfNotUsed(&box, pI);
  }

  private: void ProcessOutput(Instruction* const pI) {
    auto const pRd = ToCandidateOutput(pI);
    if (!pRd) {
      return;
    }

    DEBUG_FORMAT("output %s", pRd);

    pRd->m_pPhysical = AllocateRd(pI, pRd);

    if (pRd->GetFlag()) {
      Spill(pRd);
    }
  }

  // [R]
  private: const RegGroup& RegGroupOf(const Register& reg) {
    switch (reg.GetRegClass()) {
      case RegClass_Fpr:
        return *session_.target().GetFprGroup();

      case RegClass_Gpr:
        return *session_.target().GetGprGroup();

      case RegClass_Frame:
      default:
        CAN_NOT_HAPPEN();
    }
  }

  private: void ReleaseIfNotUsed(OperandBox* const pBox) {
    ReleaseIfNotUsed(pBox, pBox->GetI());
  }

  private: void ReleaseIfNotUsed(
      OperandBox* const pBox,
      Instruction* const pUserI) {

    auto const pRx = ToCandidate(pBox->GetRx());
    if (!pRx)  {
      return;
    }

    auto const pNextUserI = NextUser(*pUserI, *pRx);

    if (!pNextUserI) {
      // pRx isn't used in this bblock after pUserI.
      if (!pUserI->GetBBlock()->IsLiveOut(pRx->GetIndex())) {
        DEBUG_FORMAT("%s is no more used.", pRx);
        ReleasePhysicalRegister(pRx);
        return;
      }

        DEBUG_FORMAT("%s is live out.", pRx);
    }

    // If next use point is over Call, we spill it.
    if (IsCallerSave(pRx)) {
      for (
          auto pRunnerI = pUserI->GetNext();
          pNextUserI != pRunnerI;
          pRunnerI = pRunnerI->GetNext()) {
        if (pRunnerI->Is<CallI>()) {
            DEBUG_FORMAT("over Call %s", pRx);
            Spill(pRx);
            ReleasePhysicalRegister(pRx);
            return;
        }
      }
    }
  }

  private: void ReleasePhysicalRegister(Register* const pRx) {
    foreach (RegMap::Enum, oEnum, m_pRegMap) {
      auto const pEntry = oEnum.Get();
      if (pEntry->m_pRx == pRx) {
        DEBUG_FORMAT("release %s from %s", pEntry->m_pPx, pRx);
        pEntry->m_pRx = nullptr;
        break;
      }
    }
  }

  // Entry point.
  public: static void Run(
      CgSession& session,
      Function* const pFun,
      RegList* const pRegList) {
    SubPassAllocate oPass(session, pFun, pRegList);
    oPass.Run(pFun);
  }

  private: void Run(Function* const pFun) {
    DEBUG_FORMAT("Process %s", pFun);
    ProcessBBlock(pFun->GetEntryBB());
  }

  // [S]
  private: void Spill(RegMap::Entry* const pEntry) {
    Spill(pEntry->m_pRx);
    pEntry->m_pRx = nullptr;
  }

  // Allocate stack slot for spilled register.
  // TODO(2012-03-25) We should assign stack slot for life time of spilled
  // slot or reclaim  stack slots at end of life time.
  private: void Spill(Register* const pRx) {
    DEBUG_FORMAT("spill %s", pRx);

    if (pRx->m_pSpill) {
      DEBUG_FORMAT("%s is already spilled", pRx);
      return;
    }

    auto const bits = pRx->type().bit_width();
    auto const size = bits ? bits / 8 : sizeof(void*);
    auto const align = size;
    pRx->m_pSpill = &AllocateStackSlot(pRx->GetRegClass(), size, align);
    DEBUG_FORMAT("spill %s to %s", pRx, pRx->m_pSpill);
  }

  // [T]
  private: Register* ToCandidateOutput(Instruction* const pI) {
      return ToCandidate(pI->GetRd());
  }

  private: Register* ToCandidate(Register* const pRx) {
    if (!pRx) {
      return nullptr;
    }

    if (pRx->GetRegClass() == RegClass_Frame) {
      return nullptr;
    }

    return pRx;
  }

  // [U]
  private: void UpdateOperand(OperandBox* const pBox) {
    auto const pRx = ToCandidate(pBox->GetRx());
    if (!pRx) {
      return;
    }

    auto pPx = findPhysical(pRx);
    if (!pPx) {
      pPx = AllocateRx(pBox->GetI(), pRx);
    }

    pBox->m_pPhysical = pPx;
  }

  DISALLOW_COPY_AND_ASSIGN(SubPassAllocate);
};

/// <summary>
///  Assign instructions in layout order for checking next use
///  position.
/// </summary>
class SubPassAssign :
      public Tasklet,
      protected CgFunctor,
      protected Mm {

  private: class LiveMap {
    private: class LiveList {
      private: uint       m_c;
      private: Register** m_prgp;

      public: LiveList(Mm* pMm, const RegGroup* pRegGroup)
          : m_c(pRegGroup->m_pAll->m_c) {
        m_prgp = reinterpret_cast<Register**>(
            pMm->Alloc(sizeof(Register*) * m_c));
      }

      // [G]
      public: Register* Get(const Physical* const pPx) const {
        return m_prgp[pPx->GetDesc()->m_nIndex];
      }

      // [R]
      public: void Reset() {
        ::ZeroMemory(m_prgp, sizeof(Register*) * m_c);
      }

      // [S]
      public: void Set(const Physical* pPx, Register* pRx) {
        m_prgp[pPx->GetDesc()->m_nIndex] = pRx;
      }
    };

    private: LiveList m_oFprList;
    private: LiveList m_oGprList;

    // ctor
    public: LiveMap(Mm* pMm)
        : m_oFprList(pMm, CgTarget::Get().GetFprGroup()),
          m_oGprList(pMm, CgTarget::Get().GetGprGroup()) {}

    // [G]
    public: Register* Get(const Physical* const pPx) const {
      switch (pPx->GetDesc()->m_eRegClass){
        case RegClass_Fpr:
          return m_oFprList.Get(pPx);

        case RegClass_Gpr:
          return m_oGprList.Get(pPx);

        case RegClass_Frame:
        default:
          CAN_NOT_HAPPEN();
      }
    }

    // [R]
    public: void Reset() {
      m_oFprList.Reset();
      m_oGprList.Reset();
    }

    // [S]
    public: void Set(const Physical* const pPx, Register* const pRx) {
      switch (pPx->GetDesc()->m_eRegClass) {
        case RegClass_Fpr:
          m_oFprList.Set(pPx, pRx);
          break;

        case RegClass_Gpr:
          m_oGprList.Set(pPx, pRx);
          break;

        case RegClass_Frame:
        default:
          CAN_NOT_HAPPEN();
      }
    }
  };

  private: LiveMap m_oLiveMap;
  private: const RegGroup*    m_pFprGroup;
  private: const RegGroup*    m_pGprGroup;

  // ctor
  private: SubPassAssign(Session& session, Function*)
    : Tasklet(L"Ra.Assing", session),
      ALLOW_THIS_IN_INITIALIZER_LIST(m_oLiveMap(this)),
      m_pFprGroup(CgTarget::Get().GetFprGroup()),
      m_pGprGroup(CgTarget::Get().GetGprGroup()) {}

  // Entry Point
  public: static void Run(Session& session, Function* const pFun) {
    SubPassAssign oSubPass(session, pFun);
    oSubPass.run(pFun);
  }

  // [P]
  private: void ProcessBBlock(BBlock* const pBB) {
    DEBUG_FORMAT("Assign %s", pBB);

    m_oLiveMap.Reset();

    {
      DEBUG_FORMAT("LiveIn %s", pBB);
      foreach (RegPairs::Enum, oEnum, &pBB->GetWork<BBlockExt>()->m_oLiveIn) {
        const RegPair* const pPair = oEnum.Get();
        if (!pPair->m_pRx || Really::Yes == pPair->m_eReally) {
          m_oLiveMap.Set(pPair->m_pPx, pPair->m_pRx);
          DEBUG_FORMAT("%s is %s", pPair->m_pPx, pPair->m_pRx);
        }
      }
    }

    for (auto& inst: pBB->instructions())
      inst.Apply(this);
  }

  private: virtual void Process(Instruction* const pI) override {
    processOperands(pI);
    ProcessOutput(pI);
  }

  private: void processOperands(Instruction* const pI) {
    auto const pBB = pI->GetBBlock();

    for (auto& box: pI->operand_boxes()) {
      Register* const pRx = box.operand().DynamicCast<Register>();
      if (!pRx)
        continue;
      if (auto const pPx = box.m_pPhysical) {
        if (auto const pSpill = pRx->m_pSpill) {
          if (m_oLiveMap.Get(pPx) != pRx) {
            pBB->InsertBeforeI(
                *new ReloadI(pRx->GetTy(), *pPx, *pSpill),
                *pI);

            m_oLiveMap.Set(pPx, pRx);
          }
        }
        box.Replace(*pPx);
      }
    }
  }

  private: void ProcessOutput(Instruction* const pI) {
    if (auto const pRd = pI->GetRd()) {
      auto const pPd = pRd->m_pPhysical;
      if (auto const pMd = pRd->m_pSpill) {
        pI->GetBBlock()->InsertAfterI(
            *new SpillI(pRd->GetTy(), *pMd, *pPd),
            *pI);
      }
      m_oLiveMap.Set(pPd, pRd);
      pI->set_output(*pPd);
    }
  }

  // [R]
  private: void ResetRegSet(const RegSet* const pRegSet) {
    foreach (RegSet::Enum, oEnum, pRegSet) {
      auto const pReg = oEnum.Get();
      m_oLiveMap.Set(CgTarget::Get().GetPhysical(pReg), nullptr);
    }
  }

  private: void run(Function* const pFun) {
    DEBUG_FORMAT("Assign %s", pFun);

    for (auto& bblock: pFun->bblocks())
      ProcessBBlock(&bblock);
  }

  ////////////////////////////////////////////////////////////
  //
  // SubPassAssign Instruction Handlers
  //

  //  * Reset register tracker for caller save registers
  //  * Remove arguments reference
  public: virtual void Process(CallI* const pI) override {
    ResetRegSet(m_pFprGroup->m_pCallerSave);
    ResetRegSet(m_pGprGroup->m_pCallerSave);
    ProcessOutput(pI);

    pI->GetOperandBox(1)->Replace(*Void);

    if (auto pPd = pI->GetOutput()->DynamicCast<Physical>()) {
      auto const pP0 = CgTarget::Get().GetPhysical(
          m_pGprGroup->m_pArgs->m_prgpReg[0]);
      if (pPd != pP0) {
        pI->set_output(*pP0);
        pI->GetBB()->InsertAfterI(
            *new CopyI(pI->output_type(), *pPd, *pP0),
            *pI);
      }
    }
  }

  // Parallel copy pass handles spilling.
  public: virtual void Process(SelectI* const pI) override {
    if (auto const pRd = pI->GetRd()) {
      if (auto const pMd = pRd->m_pSpill) {
        pI->set_output(*pMd);

      } else if (auto const pPd = pRd->m_pPhysical) {
        m_oLiveMap.Set(pPd, pRd);
        pI->set_output(*pPd);
      }
    }
  }

  // Remove arguments reference in OPENFINALLY instruction.
  public: virtual void Process(OpenFinallyI* const pI) override {
    pI->GetOperandBox(1)->Replace(*Void);
  }

  public: virtual void Process(PhiI* const pI) override {
    if (auto const pRd = pI->GetRd()) {
      if (auto const pPd = pRd->m_pPhysical) {
        m_oLiveMap.Set(pPd, pRd);
        pI->set_output(*pPd);

        if (auto const pMd = pRd->m_pSpill) {
          Instruction* pRefI = pI;
          while (pRefI->Is<PhiI>()) {
            pRefI = pRefI->GetNext();
          }

          pRefI->GetBBlock()->InsertBeforeI(
              *new SpillI(pI->output_type(), *pMd, *pPd),
              *pRefI);
        }
      } else if (auto const pMd = pRd->m_pSpill) {
        pI->set_output(*pMd);
      }
    }
  }

  public: virtual void Process(VarHomeI* const pI) override {
    class Local {
      public: static StackSlot* Optimize(Instruction* const pI) {
        // TODO 2010-01-08 yosi@msn.com VarHome shold use Ty_StackCell
        if (pI->output_type() == *Ty_ClosedCell) {
          return nullptr;
        }

        auto const pMd = pI->GetOutput()->DynamicCast<VarHome>();
        if (!pMd) return nullptr;

        auto const pRy = pI->GetRy();
        if (!pRy) return nullptr;

        return pRy->m_pSpill;
      }
    };

    if (auto pMy = Local::Optimize(pI)) {
      DEBUG_FORMAT("Use %s instead of %s", pMy, pI->GetOutput());
      pI->set_output(*pMy);
    }
  }

  DISALLOW_COPY_AND_ASSIGN(SubPassAssign);
};

/// <summary>
///   Emit instructions for parallel copy.
///   <para>
///     Note: We don't allow Mx and Md are same location.
///   </para>
/// </summary>
class ParallelCopy :
      public Tasklet,
      protected Mm {

  private: class Task : public LocalObject {
     protected: Task() {}
     DISALLOW_COPY_AND_ASSIGN(Task);
  };

  private: class CopyTask
      : public Task,
        public WorkListItem_<CopyTask> {
      public: Physical* const m_pRd;
      public: Physical* m_pRx;    // for pending queue.
      public: const Type& type_;

      // Note: pRd == pRx to avoid using pRx for MemCopy.
      public: CopyTask(
          const Type& type,
          Physical* const pRd,
          Physical* const pRx) :
              m_pRd(pRd),
              m_pRx(pRx),
              type_(type) {
          m_pRx->Use();
      }

      DISALLOW_COPY_AND_ASSIGN(CopyTask);
  };

  private: typedef WorkList_<CopyTask> CopyTasks;

  private: class LoadTask
      : public Task,
        public WorkListItem_<LoadTask> {
      public: Physical* const m_pRd;
      public: MemSlot* const m_pMx;
      public: const Type& type_;

      public: LoadTask(
          const Type& type,
          Physical* const pRd,
          MemSlot* const pMx) :
              m_pRd(pRd), m_pMx(pMx), type_(type) {}

      DISALLOW_COPY_AND_ASSIGN(LoadTask);
  };

  private: typedef WorkList_<LoadTask> LoadTasks;

  private: class MemCopyTask
      : public Task,
        public WorkListItem_<MemCopyTask> {
      public: MemSlot* const m_pMd;
      public: MemSlot* const m_pMx;
      public: const Type& type_;

      public: MemCopyTask(
          const Type& type,
          MemSlot* const pMd,
          MemSlot* const pMx) :
          m_pMd(pMd), m_pMx(pMx), type_(type) {}

      DISALLOW_COPY_AND_ASSIGN(MemCopyTask);
  };

  private: typedef WorkList_<MemCopyTask> MemCopyTasks;

  private: class MemSetTask
      : public Task,
        public WorkListItem_<MemSetTask> {
      public: MemSlot* const m_pMd;
      public: Operand* const m_pSx;

      public: MemSetTask(MemSlot* const pMd, Operand* const pSx) :
          m_pMd(pMd), m_pSx(pSx) {}

      DISALLOW_COPY_AND_ASSIGN(MemSetTask);
  };

  private: typedef WorkList_<MemSetTask> MemSetTasks;

  private: class SetTask
      : public Task,
        public WorkListItem_<SetTask> {
      public: Physical* const m_pRd;
      public: Operand* const m_pSx;

      public: SetTask(Physical* const pRd, Operand* const pSx) :
          m_pRd(pRd), m_pSx(pSx) {}

      DISALLOW_COPY_AND_ASSIGN(SetTask);
  };

  private: typedef WorkList_<SetTask> SetTasks;

  private: class StoreTask
      : public Task,
        public WorkListItem_<StoreTask> {
      public: MemSlot* const m_pMd;
      public: Physical* const m_pRx;

      public: StoreTask(MemSlot* const pMd, Physical* const pRx) :
              m_pMd(pMd), m_pRx(pRx) {
          m_pRx->Use();
      }

      DISALLOW_COPY_AND_ASSIGN(StoreTask);
  };

  private: typedef WorkList_<StoreTask> StoreTasks;

  private: typedef WorkList_<CopyTask> CopyTasks;
  private: WorkList_<CopyTask>    m_oCopyTasks;
  private: WorkList_<LoadTask>    m_oLoadTasks;
  private: WorkList_<MemCopyTask> m_oMemCopyTasks;
  private: WorkList_<MemSetTask>  m_oMemSetTasks;
  private: WorkList_<SetTask>     m_oSetTasks;
  private: WorkList_<StoreTask>   m_oStoreTasks;
  private: BBlock*                m_pBB;
  private: Instruction*           m_pRefI;
  private: const RegSet*          m_pRegSet;

  // ctor
  public: ParallelCopy(Session& session, const RegGroup* pRegGroup)
      : Tasklet(L"ParallelCopy", session),
        m_pBB(nullptr),
        m_pRefI(nullptr),
        m_pRegSet(pRegGroup->m_pFree) {}

  // [A]
  public: void AddTask(
      const Type& type,
      MemSlot* const pMd,
      OperandBox* const pBox) {

      if (auto const pPx = pBox->m_pPhysical) {
          AddTask(type, pMd, pPx);

      } else if (auto const pRx = pBox->GetRx()) {
          if (auto const pMx = pRx->m_pSpill) {
              AddTask(type, pMd, pMx);

          } else {
              DEBUG_FORMAT("Can't add task %s => %s",
                  pRx, pMd);

              COMPILER_INTERNAL_ERROR();
          }

      } else {
          AddTask(type, pMd, pBox->GetOperand());
      }
  }

  // Note: pBox->GetI()->GetTy() != type for ValuesI.
  public: void AddTask(
      const Type& type,
      Physical* const pRd,
      OperandBox* const pBox) {

      if (auto const pPx = pBox->m_pPhysical) {
          AddTask(type, pRd, pPx);

      } else if (auto const pRx = pBox->GetRx()) {
          if (auto const pMx = pRx->m_pSpill) {
              AddTask(type, pRd, pMx);

          } else {
              DEBUG_FORMAT("Can't add task %s => %s",
                  pRx, pRd);

              COMPILER_INTERNAL_ERROR();
          }

      } else {
          AddTask(type, pRd, pBox->GetOperand());
      }
  }

  public: void AddTask(
      const Type& type,
      Register* const pRd,
      Physical* const pRx) {

      ASSERT(pRd->GetTy() == type);

      if (auto const pMd = pRd->m_pSpill) {
          AddTask(type, pMd, pRx);

      } else if (auto const pPd = pRd->m_pPhysical) {
          AddTask(type, pPd, pRx);

      } else {
          DEBUG_FORMAT("Can't add task %s => %s",
              pRx, pRd);

          COMPILER_INTERNAL_ERROR();
      }
  }

  public: void AddTask(Register* const pRd, MemSlot* const pMx) {
    auto& type = pRd->GetTy();
    if (auto const pMd = pRd->m_pSpill) {
      AddTask(type, pMd, pMx);
    } else if (auto const pPd = pRd->m_pPhysical) {
      AddTask(type, pPd, pMx);
    } else {
      COMPILER_INTERNAL_ERROR();
    }
  }

  public: void AddTask(
      const Type& type,
      Physical* const pRd,
      Operand* const pSx) {

      if (auto const pRx = pSx->DynamicCast<Physical>()) {
          DEBUG_FORMAT("CopyTask %s := %s %s", pRd, pRx, type);
          m_oCopyTasks.Push(new(this) CopyTask(type, pRd, pRx));

      } else if (auto const pMx = pSx->DynamicCast<MemSlot>()) {
          DEBUG_FORMAT("LoadTask %s := %s %s", pRd, pMx, type);
          m_oLoadTasks.Push(new(this) LoadTask(type, pRd, pMx));

      } else {
          DEBUG_FORMAT("SetTask %s := %s %s", pRd, pSx, type);
          m_oSetTasks.Push(new(this) SetTask(pRd, pSx));
      }
  }

  public: void AddTask(
      const Type& type,
      MemSlot* const pMd,
      Operand* const pSx) {

      if (auto const pRx = pSx->DynamicCast<Physical>()) {
          DEBUG_FORMAT("StoreTask %s := %s", pMd, pRx);
          m_oStoreTasks.Push(new(this) StoreTask(pMd, pRx));

      } else if (auto const pMx = pSx->DynamicCast<MemSlot>()) {
          DEBUG_FORMAT("MemCopyTask %s := %s", pMd, pMx);
          m_oMemCopyTasks.Push(new(this) MemCopyTask(type, pMd, pMx));

      } else {
          DEBUG_FORMAT("MemSetTask %s := %s", pMd, pSx);
          m_oMemSetTasks.Push(new(this) MemSetTask(pMd, pSx));
      }
  }

  // [E]
  private: void EmitCopy(
      const Type& type,
      CgOutput* const pRd,
      Operand* const pSx) {
    m_pBB->InsertBeforeI(*new PhiCopyI(type, *pRd, *pSx), *m_pRefI);
  }

  // [G]
  public: RegClass GetRegClass() const
      { return m_pRegSet->m_eRegClass; }

  private: static Physical* getPhysical(const RegDesc* p)
      { return CgTarget::Get().GetPhysical(p); }

  // [P]
  private: void processCopyTasks(CopyTasks* const pIn) {
      CopyTasks oPendings;
      CopyTasks oReadies;

      auto pTemps = pIn;
      auto pPendings = &oPendings;
      auto pReadies  = &oReadies;

      while (!pTemps->IsEmpty()) {
          auto const pTask = pTemps->Pop();

          if (pTask->m_pRd == pTask->m_pRx) {
              pTask->m_pRx->UnUse();

          } else if (pTask->m_pRd->use_count()) {
              pPendings->Push(pTask);

          } else {
              pReadies->Push(pTask);
          }
      }

      for (;;) {
          while (!pReadies->IsEmpty()) {
              auto const pTask = pReadies->Pop();
              EmitCopy(pTask->type_, pTask->m_pRd, pTask->m_pRx);
              ASSERT(0 == pTask->m_pRd->use_count());
              //ASSERT(pTask->m_pRx->use_count() >= 1);
              pTask->m_pRx->UnUse();

              ASSERT(pTemps->IsEmpty());
              while (!pPendings->IsEmpty())
              {
                  auto const pTask = pPendings->Pop();
                  if (0 == pTask->m_pRd->use_count()) {
                      pReadies->Push(pTask);

                  } else {
                      pTemps->Push(pTask);
                  }
              }

              swap(pPendings, pTemps);
          }

          ASSERT(pTemps->IsEmpty());
          if (pPendings->IsEmpty())
          {
              // All tasks are finished or no more pending tasks.
              break;
          }

          // Free %rd since %rd is source of other tasks.
          {
              CopyTask* const pTask = pPendings->Pop();

              ASSERT(pTask->m_pRd->use_count() >= 1);
              ASSERT(pTask->m_pRx->use_count() >= 1);

              m_pBB->InsertBeforeI(
                  *new SwapI(pTask->type_, *pTask->m_pRd, *pTask->m_pRx),
                  *m_pRefI);

              foreach (CopyTasks::Enum, oEnum, pPendings) {
                  auto const pPending = oEnum.Get();
                  if (pPending->m_pRx == pTask->m_pRd) {
                      pPending->m_pRx = pTask->m_pRx;
                      ASSERT(pTask->m_pRd->use_count() >= 1);
                      pTask->m_pRd->UnUse();
                      pTask->m_pRx->Use();
                  }
              }

              ASSERT(pTemps->IsEmpty());
              while (!pPendings->IsEmpty()) {
                  auto const pPending = pPendings->Pop();

                  if (pPending->m_pRd == pPending->m_pRx) {
                      // ignore

                  } else if (0 == pPending->m_pRd->use_count()) {
                      pReadies->Push(pPending);

                  } else {
                      pTemps->Push(pPending);
                  }
              }

              swap(pPendings, pTemps);
          }
      }
  }

  private: void processLoadTasks(LoadTasks* const pTasks) {
      while (!pTasks->IsEmpty()) {
          auto const pTask = pTasks->Pop();
          EmitCopy(pTask->type_, pTask->m_pRd, pTask->m_pMx);
      }
  }

  private: void processMemCopyTasks(MemCopyTasks* const pTasks) {
      Physical* pRx = nullptr;

      foreach (RegSet::Enum, oEnum, m_pRegSet) {
          auto const pPx = getPhysical(oEnum.Get());
          if (0 == pPx->use_count()) {
              pRx = pPx;
              break;
          }
      }

      ThreadSlot* pSpill = nullptr;

      if (!pRx) {
          // TODO 2011-01-09 yosi@msn.com We should allocate spill slot
          // on stack rarther than thread.
          CAN_NOT_HAPPEN();
          #if 0
              pSpill = new ThreadSlot(
                  GetRegClass(),
                  //offsetof(Thread, mv_value)
                  128);

              pRx = getPhysical(m_pRegSet->m_prgpReg[0]);

              EmitCopy(pSpill, pRx);
          #endif
      }

      while (!pTasks->IsEmpty()) {
          auto const pTask = pTasks->Pop();
          EmitCopy(pTask->type_, pRx, pTask->m_pMx);
          EmitCopy(pTask->type_, pTask->m_pMd, pRx);
      }

      if (pSpill) {
        EmitCopy(pRx->GetTy(), pRx, pSpill);
      }
  }

  private: void processMemSetTasks(MemSetTasks* const pTasks) {
    while (!pTasks->IsEmpty()) {
      auto const pTask = pTasks->Pop();
      auto& type = pTask->m_pSx->GetTy();

      Physical* pPx = nullptr;

      WorkList_<SetTask> oTemp;
      while (m_oSetTasks.IsEmpty()) {
        auto const pSetTask = m_oSetTasks.Pop();

        if (*pSetTask->m_pSx == *pTask->m_pSx) {
          EmitCopy(type, pSetTask->m_pRd, pTask->m_pSx);

          if (!pPx) {
            pPx = pSetTask->m_pRd;
          }

        } else {
          oTemp.Push(pSetTask);
        }
      }

      while (!oTemp.IsEmpty()) {
        m_oSetTasks.Push(oTemp.Pop());
      }

      if (!pPx) {
        EmitCopy(type, pTask->m_pMd, pTask->m_pSx);

      } else {
        EmitCopy(type, pTask->m_pMd, pPx);
      }
    }
  }

  private: void processSetTasks(SetTasks* const pTasks) {
    WorkList_<SetTask> oSoFar;

    while (!pTasks->IsEmpty()) {
      auto const pTask = pTasks->Pop();

      Physical* pPx = nullptr;
      foreach (WorkList_<SetTask>::Enum, oEnum, &oSoFar) {
        if (*oEnum.Get()->m_pSx == *pTask->m_pSx) {
          pPx = oEnum.Get()->m_pRd;
          break;
        }
      }

      auto& type = pTask->m_pSx->GetTy();
      if (!pPx) {
        EmitCopy(type, pTask->m_pRd, pTask->m_pSx);
        oSoFar.Push(pTask);

      } else {
        EmitCopy(type, pTask->m_pRd, pPx);
      }
    }
  }

  private: void processStoreTasks(StoreTasks* pTasks) {
    while (!pTasks->IsEmpty()) {
      auto const pTask = pTasks->Pop();
      EmitCopy(pTask->m_pRx->GetTy(), pTask->m_pMd, pTask->m_pRx);
      pTask->m_pRx->UnUse();
    }
  }

  // [R]
  public: void Reset() {
    foreach (RegSet::Enum, oEnum, m_pRegSet) {
      auto const pPx = getPhysical(oEnum.Get());
      pPx->ResetUseCount();
    }
  }

  public: void Run(Instruction* pRefI) {
    m_pRefI = pRefI;
    m_pBB   = pRefI->GetBBlock();

    DEBUG_FORMAT("Parallel Copy %s", pRefI);

    processStoreTasks(&m_oStoreTasks);      // Md <= Rx
    processMemCopyTasks(&m_oMemCopyTasks);  // Md <= Mx
    processCopyTasks(&m_oCopyTasks);        // Rd <= Rx
    processLoadTasks(&m_oLoadTasks);        // Rd <= Md
    processMemSetTasks(&m_oMemSetTasks);    // Md <= Im
    processSetTasks(&m_oSetTasks);          // Rd <= Im
  }

  DISALLOW_COPY_AND_ASSIGN(ParallelCopy);
};

/// <summary>
///   Invert Phi instructions in layout order for next use position.
/// </summary>
class SubPassInvertPhi : public Tasklet {

  private: ParallelCopy    m_oParallelCopy;

  // ctor
  private: SubPassInvertPhi(CgSession& session)
      : Tasklet(L"Ra.InvertPhi", session),
        m_oParallelCopy(session, session.target().GetGprGroup()) {}

  // [I]
  private: void InvertPhi(BBlock* const pCurr, BBlock* const pSucc) {
    DEBUG_FORMAT("InvertPhi %s => %s", pCurr, pSucc);

    m_oParallelCopy.Reset();

    auto fHasTask = false;

    for (auto& phi_inst: pSucc->phi_instructions()) {
        auto const pRd = phi_inst.GetRd();
      if (!pRd) {
        continue;
      }

      if (pRd->GetRegClass() != m_oParallelCopy.GetRegClass()) {
        continue;
      }

      DEBUG_FORMAT("%s pd=%s spill=%s",
          phi_inst, pRd->m_pPhysical, pRd->m_pSpill);

      auto& type = phi_inst.output_type();

      auto const pBox = phi_inst.GetOperandBox(pCurr);

      updatePhiOperand(pBox);

      if (auto const pPd = pRd->m_pPhysical) {
        m_oParallelCopy.AddTask(type, pPd, pBox);
        fHasTask = true;

      } else if (auto const pMd = pRd->m_pSpill) {
        m_oParallelCopy.AddTask(type, pMd, pBox);
        fHasTask = true;

      } else {
        COMPILER_INTERNAL_ERROR();
      }
    }

    if (fHasTask) {
      m_oParallelCopy.Run(pCurr->GetLastI());
    }
  }

  // [P]
  private: void ProcessBBlock(BBlock* const pCurr) {
    foreach (BBlock::EnumSucc, oEnum, pCurr) {
      InvertPhi(pCurr, oEnum.Get());
    }
  }

  // [R]
  // Entry Point
  public: static void Run(
      CgSession& session,
      const Function& fun) {
    SubPassInvertPhi oPass(session);
    oPass.Run(fun);
  }

  private: void Run(const Function& fun) {
    DEBUG_FORMAT("Process %s", fun);
    for (auto& bblock: fun.bblocks()) {
      ProcessBBlock(&bblock);
    }
  }

  // [U]
  private: void updatePhiOperand(PhiOperandBox* const pBox) {
    auto const pRx = pBox->GetRx();

    if (!pRx) {
      // Literal operand
      return;
    }

    if (pRx->m_pSpill) {
      // Spilled regiser
      return;
    }

    if (!pRx->m_pPhysical) {
      session().AddErrorInfo(
          VmErrorInfo(
            pBox->GetI()->source_info(),
            VmError_Optimize_InternalError,
            String::Format(
                "Broken operand in %s,"
                " expect physical register but %s",
                pBox->GetI(),
                pRx)));
      return;
    }

    pBox->m_pPhysical = pRx->m_pPhysical;
  }

  DISALLOW_COPY_AND_ASSIGN(SubPassInvertPhi);
};

/// <summary>
///   ParallelCopy instructions in layout order for checking next use
///   position.
/// </summary>
class SubPassParallelCopy
    : public Tasklet,
      protected CgFunctor,
      protected Mm {

  private: ParallelCopy m_oParallelCopy;
  private: CgTarget* m_pTarget;
  private: Session& session_;

  // ctor
  private: SubPassParallelCopy(Session& session, const RegGroup* pRegGroup)
      : Tasklet(L"Ra.ParallelCopy", session),
        m_oParallelCopy(session, pRegGroup),
        m_pTarget(&CgTarget::Get()),
        session_(session) {}

  // Entry Point
  public: static void Run(
      Session& session,
      const Function& fun,
      const RegGroup* pRegGroup) {
    SubPassParallelCopy oPass(session, pRegGroup);
    oPass.Run(fun);
  }

  // [G]
  private: static Physical* getPhysical(const RegDesc* const p) {
    return CgTarget::Get().GetPhysical(p);
  }

  // [I]
  private: static bool IsFirstSelect(Instruction& inst) {
    auto const prev = inst.GetPrev();
    return !prev || !prev->Is<SelectI>();
  }

  // [P]
  private: void AddTask(
      const Type& outy,
      const Output& out,
      Operand& src) {
    if (src.Is<Physical>() || src.Is<ThreadSlot>()) {
      if (auto const pRd = out.DynamicCast<Physical>()) {
        m_oParallelCopy.AddTask(outy, pRd, &src);
        return;
      }

      if (auto const pMd = out.DynamicCast<MemSlot>()) {
        m_oParallelCopy.AddTask(outy, pMd, &src);
        return;
      }
    }

    session().AddErrorInfo(
        VmErrorInfo(
            out.GetDefI()->source_info(),
            VmError_Optimize_InternalError,
            String::Format("Broken instruction %s",
                *out.GetDefI())));
  }

  // [R]
  private: void Run(const Function& fun) {
    DEBUG_FORMAT("Process %s", fun);
    for (auto& inst: fun.instructions())
      inst.Apply(this);
  }

  // Instruction handlers
  public: virtual void Process(SelectI* const pSelectI) override {
    if (!IsFirstSelect(*pSelectI)) {
      return;
    }

    DEBUG_FORMAT("Process %s", pSelectI);
    m_oParallelCopy.Reset();

    Instruction* pI;
    for (pI = pSelectI;  pI && pI->Is<SelectI>(); pI = pI->GetNext()) {
      DEBUG_FORMAT("process SelectI: %s", pI);
      auto const nNth = static_cast<uint>(pI->GetIy());
      auto const pSx = m_pTarget->GetArgReg(nNth);
      AddTask(pI->output_type(), pI->output(), *pSx);
      pI->set_output(*Useless);
    }

    m_oParallelCopy.Run(pI);
  }

  public: virtual void Process(ValuesI* const pI) override {
      DEBUG_FORMAT("process %s", pI);

      m_oParallelCopy.Reset();

      ValuesType::Enum oEnumType(
          pI->output_type().StaticCast<ValuesType>());

      auto nNth = 0;
      for (auto& box: pI->operand_boxes()) {
        auto const pOd = m_pTarget->GetArgReg(nNth);
        nNth += 1;

        auto& type = oEnumType.Get();
        oEnumType.Next();

        if (auto const pRd = pOd->DynamicCast<Physical>()) {
          m_oParallelCopy.AddTask(type, pRd, &box);

        } else if (auto const pMd = pOd->DynamicCast<ThreadSlot>()) {
          m_oParallelCopy.AddTask(type, pMd, &box);

        } else {
          COMPILER_INTERNAL_ERROR();
        }
      }

      m_oParallelCopy.Run(pI);
  }

  DISALLOW_COPY_AND_ASSIGN(SubPassParallelCopy);
};

/// <summary>
///  Sub pass for allocating specified register group.
/// </summary>
class SubPassRa : public Tasklet, protected Mm {
  private: bool no_callee_save_;
  private: RegList reg_list_;
  private: CgSession& session_;

  // ctor
  private: SubPassRa(CgSession& session)
      : Tasklet(L"Ra.Core", session),
        //= TODO(yosi) 2009-01-10 no_callee_save_ must be target specific.
        no_callee_save_(true),
        session_(session) {}

  // [B]
  private: void BuildRegList(const Function& fun) {
    auto& target = session_.target();

    for (auto& bblock: fun.bblocks()) {
      auto const pExt = new BBlockExt();

      pExt->m_pRegMap = new(this) RegMap(this, target);
      bblock.SetWork(pExt);
      bblock.SetFlag(HasNoCall);

      for (auto& inst: bblock.instructions()) {
        if (auto const pRx = inst.GetRd()) {
            DEBUG_FORMAT("Add %s to list", *pRx);
            reg_list_.Append(pRx);
            pRx->SetFlag(NotAcrossCall);
        }

        if (inst.Is<CallI>()) {
          bblock.SetFlag(HasCall);
        }
      }
    }
  }

  // [C]
  private: void ComputeSpillHint(const Function& fun) {
    for (auto& bblock: fun.bblocks()) {
      if (bblock.GetFlag() != HasCall) {
        // No CALL in this bblock
        continue;
      }

      foreach (RegList::Enum, regs, &reg_list_) {
        auto& reg = *regs.Get();
        if (IsAcrossCall(reg, bblock)) {
          reg.SetFlag(AcrossCall);
        }
      }
    }
  }

  // [I]
  private: static bool IsAcrossCall(
      const Register& reg,
      const BBlock& bblock) {
    auto const index = reg.GetIndex();
    return bblock.IsLiveIn(index) && bblock.IsLiveOut(index);
  }

  // [R]
  // Entry Point
  public: static void Run(CgSession& session, Function& fun) {
    SubPassRa oPass(session);
    oPass.Run(fun);
  }

  private: void Run(Function& fun) {
    DEBUG_FORMAT("Process %s", fun);

    BuildRegList(fun);

    if (no_callee_save_) {
      ComputeSpillHint(fun);
    }

    SubPassAllocate::Run(session_, &fun, &reg_list_);
    if (session_.HasError()) {
      return;
    }
    SubPassInvertPhi::Run(session_, fun);
  }

  DISALLOW_COPY_AND_ASSIGN(SubPassRa);
};

/// <summary>
///   Remove Phi instructions for non Values register.
/// </summary>
class SubPassRemovePhi : public Tasklet {
  private: SubPassRemovePhi(Session& session) :
      Tasklet(L"Ra.RemovePhi", session) {}

  // Entry Point
  public: static void Run(Session& session, Function* const pFun) {
    SubPassRemovePhi oPass(session);
    oPass.Run(pFun);
  }

  // [R]
  // See (defun foo (x) (multiple-value-call #'bar (if x (baz) (quux))))
  private: void Run(Function* const pFun) {
    DEBUG_FORMAT("Remove Phi in %s", pFun);

    for (auto& bblock: pFun->bblocks()) {
      DEBUG_FORMAT("Remove Phi in %s", bblock);
      WorkList_<Instruction> oPhiIs;
      for (auto& phi_inst: bblock.phi_instructions()) {
        if (!phi_inst.GetOutput()->Is<Values>())
            oPhiIs.Push(&phi_inst);
      }

      while (!oPhiIs.IsEmpty()) {
        bblock.RemoveI(*oPhiIs.Pop());
      }
    }
  }

  DISALLOW_COPY_AND_ASSIGN(SubPassRemovePhi);
};

} // CgRa

using namespace CgRa;

RegAllocPass::RegAllocPass(Session& session, Module& module)
    : Base(L"RegAlloc", session, module),
      session_(static_cast<CgSession&>(session)) {}

void RegAllocPass::ProcessFunction(Function& fun) {
  DEBUG_FORMAT("Start RA in %s ~~~~~~~~~~~~~~~~~~~~", fun);

  {
    FunctionEditor editor(session_);
    editor.RemoveCriticalEdges(fun);
    DominanceAnalyzer analyzer(session_);
    analyzer.ComputeDominance(fun);
    editor.Renumber(fun);
    analyzer.ComputePostDominance(fun);
    fun.ComputeLiveness();
  }

  // Put Phi-operands to LiveOut for easy checking of live range.
  for (auto& bblock: fun.bblocks()) {
    for (auto& phi_inst: bblock.phi_instructions()) {
      if (phi_inst.GetRd()) {
        foreach (BBlock::EnumPred, oEnum, bblock) {
            auto const pPred = oEnum.Get();
            if (auto const pRx = phi_inst.GetRx(pPred)) {
                pPred->SetLiveOut(pRx->GetIndex());
            }
        }
      }
    }
  }

  auto& target = CgTarget::Get();

  SubPassRa::Run(session_, fun);
  if (session_.HasError()) {
    return;
  }

  SubPassAssign::Run(session_, &fun);
  if (session_.HasError()) {
    return;
  }

  SubPassParallelCopy::Run(session_, *&fun, target.GetGprGroup());
  if (session_.HasError()) {
    return;
  }

  // Note: We don't support FPR in values.
  //SubPassParallelCopy::Run(&fun, target.GetFprGroup());

  //= <TODO date="2007-10-10" by="yosi@msn.com">
  //=   We need to eliminate
  //=     PHICOPY %r1 <= %m2
  //=     PHICOPY %r3 <= %m4
  //=     PHICOPY %m2 <= %r1
  //=     PHICOPY %m4 <= %r3
  //=     e.g. (lambda (a b ... g) (list a b ... g))
  //= </TODO>

  SubPassRemovePhi::Run(session_, &fun);
  if (session_.HasError()) {
    return;
  }

  // Remove COPY eax <= eax
  {
    DEBUG_FORMAT("Remove Useless %s", &fun);
    WorkList_<Instruction> useless_insts;
    for (auto& inst: fun.instructions()) {
      if (inst.IsUseless()) {
        useless_insts.Push(&inst);

      } else if (inst.Is<CloseI>() || inst.Is<OpenInstruction>()) {
        useless_insts.Push(&inst);
      }
    }

    while (!useless_insts.IsEmpty()) {
      auto const pI = useless_insts.Pop();
      DEBUG_FORMAT("Remove %s", pI);
      pI->GetBBlock()->RemoveI(*pI);
    }
  }

  DEBUG_FORMAT("End RA in %s ~~~~~~~~~~~~~~~~~~~~", fun);
}

} // Cg
} // Il
