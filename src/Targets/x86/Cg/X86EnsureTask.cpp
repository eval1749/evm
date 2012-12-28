#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Targets - X86 Asm
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86EnsureTask.h"

#include "./X86Defs.h"

#include "../../../Il/Ir/VmErrorInfo.h"
#include "../../../Il/Tasks/NormalizeTasklet.h"
#include "../../../Il/Tasks/Tasklet.h"

namespace Il {
namespace Cg {

using namespace Il::Tasks;

namespace {

using namespace X86;

// COMISS
//  less than       ZF=0 CF=1
//  greater than    ZF=0 CF=0
//  equal           ZF=1 CF=0
static Tttn k_rgeInt2FloatTttn[] = {
  tttn_P,     //  0 O     => not used
  tttn_P,     //  1 NO    => not used
  tttn_P,     //  2 B     => not sued
  tttn_P,     //  3 NB    => not used
  tttn_E,     //  4 E     => ZF=1
  tttn_NE,    //  5 NE    => ZF=0
  tttn_P,     //  6 BE
  tttn_P,     //  7 NBE
  tttn_P,     //  8 S
  tttn_P,     //  9 NS
  tttn_P,     // 10 P
  tttn_NP,    // 11 NP
  tttn_B,     // 12 L     => CF=1
  tttn_NB,    // 13 GE    => CF=0
  tttn_NA,    // 14 LE    => ZF=1 and CF=1
  tttn_A,     // 15 G     => ZF=0 and CF=0
};


#define INTERNAL_ERROR(mp_format, param, ...) \
  session().AddErrorInfo( \
      VmErrorInfo( \
          SourceInfo(__FILE__, __LINE__, 0), \
          VmError_Optimize_InternalError, \
          String::Format(__FUNCTION__ ":" mp_format, param, __VA_ARGS__)))

class EnsureTasklet :
  public X86Functor,
  public Tasklet {

  private: const Module& module_;
  private: NormalizeTasklet normalizer_;

  public: EnsureTasklet(Session& session, const Module& module)
    : Tasklet("EnsureTasklet", session),
      module_(module),
      normalizer_(session, module) {}

  public: virtual ~EnsureTasklet() {}

  // [E]
  /// <summary>
  ///  Make output and the first operand same.
  /// </summary>
  private: void EnsureRdRx(Instruction* const pI) {
      EnsureRdRn(pI, pI->GetOperandBox(0));
  }

  private: void EnsureRdRn(
      Instruction* const pI,
      OperandBox*  const pBox) {
    ASSERT(pI->GetOutput() != pI->GetSx());

    DEBUG_FORMAT("Ensure Rd == Rx: %s", pI);

    if (auto const pFd = pI->GetOutput()->DynamicCast<Float>()) {
      auto const pFx = new Float();
      auto& outy = pFd->GetTy();

      pI->GetBBlock()->InsertBeforeI(
          *new AssignI(outy, *pFx, *pBox->GetOperand()),
          *pI);

      pBox->Replace(*pFx);

      auto const pFnew = new Float();
      pI->set_output(*pFnew);
      pI->GetBBlock()->InsertAfterI(*new CopyI(outy, *pFd, *pFnew), *pI);

    } else if (auto const pRd = pI->GetRd()) {
      auto const pRx = &module_.NewRegister();
      auto& outy = pRd->GetTy();

      pI->GetBBlock()->InsertBeforeI(
          *new AssignI(outy, *pRx, *pBox->GetOperand()),
          *pI);

      pBox->Replace(*pRx);

      auto const pRnew = &module_.NewRegister();
      pI->set_output(*pRnew);
      pI->GetBBlock()->InsertAfterI(*new CopyI(outy, *pRd, *pRnew), *pI);

    } else {
      INTERNAL_ERROR("Unexpected %s", *pI);
    }
  }

  // [G]
  private: Tttn ComputeTttnForintCmp(Instruction* const pI) {
    ASSERT(pI->Is<RelationalInstruction>());

    if (pI->Is<EqI>()) return tttn_E;
    if (pI->Is<GeI>()) return tttn_GE;
    if (pI->Is<GtI>()) return tttn_G;
    if (pI->Is<LeI>()) return tttn_LE;
    if (pI->Is<LtI>()) return tttn_L;
    if (pI->Is<NeI>()) return tttn_NE;

    // TODO 2011-05-02 eval1749@ NYI Cast to Boolean.
    INTERNAL_ERROR("Unsupported %s", *pI);
    return tttn_Z;
  }

  // [H]
  private: void HoisttSelects(
      Values* const pVd,
      Instruction* const pDefI) {
    auto pRefI = pDefI->GetNext();

    while (pRefI->Is<PhiI>()) {
      pRefI = pRefI->GetNext();
    }

    while (pRefI->Is<SelectI>()) {
      pRefI = pRefI->GetNext();
    }

    foreach (Values::EnumUser, oEnum, pVd) {
      auto const pUserI = oEnum.GetI();
      if (pUserI->Is<SelectI>()) {
        if (pUserI != pRefI->GetPrev()) {
          pRefI->GetBB()->MoveBeforeI(*pUserI, pRefI);
        }
      }
    }
}

  // [I]
  private: static bool IsNull(Operand* const pSx) {
    if (auto const pRx = pSx->DynamicCast<Register>()) {
      if (pRx->GetDefI()->Is<NullI>()) {
        return true;
      }
    }

      return false;
  }

  private: static bool IsRegOperand(Operand* const pSx) {
    return pSx->Is<Physical>() || pSx->Is<Register>();
  }

  // [P]
  /// <summary>
  ///  Make an output to pseudo output.
  /// </summary>
  private: void Pseudofy(Instruction* const pI) {
    if (SsaOutput* const pRd =  pI->GetOutput()->DynamicCast<SsaOutput>()) {
      auto const pQ2 = new PseudoOutput();
      ReplaceAll(*pQ2, *pRd);
      pI->set_output(*pQ2);
    }
  }

  // [R]

  // Replace BoolOutput users.
  private: void RewriteBoolOutput(BoolOutput* const pBd) {
    auto pI = pBd->GetDefI();
    if (pI->Is<x86BoolI>()) {
      return;
    }

    // Remove pI.
    normalizer_.Add(pI);

    while (auto const pBox = pBd->GetFirst()) {
      RewriteBoolUser(pBd, pBox);
    }
  }

  // Note: We don't use integer equality for comparing float32. Since, we
  // don't want to handle -0, infinity and NaN, e.g. (= +0f0 -0f0) = true.
  private: void RewriteBoolUser(
      BoolOutput* const pBd,
      OperandBox* const pBox) {
    auto const pR1 = new PseudoOutput();
    auto const pB2 = &module_.NewBoolOutput();

    auto const pDefI = pBd->GetDefI();
    ASSERT(pDefI->Is<RelationalInstruction>());

    auto const pUseI = pBox->GetI();

    auto eTttn = (Tttn) -1;

    if (*pDefI->GetSy() == BooleanLiteral(true)) {
      pUseI->GetBBlock()->InsertBeforeI(
          *new x86TestI(*pR1, *pDefI->GetSx(), *pDefI->GetSx()),
          *pUseI);
      eTttn = pDefI->Is<EqI>() ? tttn_NE : tttn_E;

    } else if (*pDefI->GetSy() == BooleanLiteral(false) 
                  || IsNull(pDefI->GetSy())) {
      pUseI->GetBBlock()->InsertBeforeI(
          *new x86TestI(*pR1, *pDefI->GetSx(), *pDefI->GetSx()),
          *pUseI);

      eTttn = pDefI->Is<EqI>() ? tttn_E : tttn_NE;

    } else if (pDefI->GetSy()->GetTy() == *Ty_Float32) {
      pUseI->GetBBlock()->InsertBeforeI(
          *new x86CmpF32I(*pR1, *pDefI->GetSx(), *pDefI->GetSy()),
          *pUseI);

      eTttn = k_rgeInt2FloatTttn[ComputeTttnForintCmp(pDefI)];

    } else if (pDefI->GetSy()->GetTy() == *Ty_Float64) {
      pUseI->GetBBlock()->InsertBeforeI(
          *new x86CmpF64I(*pR1, *pDefI->GetSx(), *pDefI->GetSy()),
          *pUseI);

      eTttn = k_rgeInt2FloatTttn[ComputeTttnForintCmp(pDefI)];

    } else {
      pUseI->GetBBlock()->InsertBeforeI(
          *new x86CmpI(*pR1, *pDefI->GetSx(), *pDefI->GetSy()),
          *pUseI);

      eTttn = ComputeTttnForintCmp(pDefI);
    }

    ASSERT(eTttn != (Tttn) -1);

    pUseI->GetBBlock()->InsertBeforeI(
        *new x86BoolI(*pB2, *pR1, eTttn),
        *pUseI);

    pBox->Replace(*pB2);

      normalizer_.Add(pUseI);
  }

  public: void Run(Function& fn) {
    auto const pFun = &fn;
    DEBUG_FORMAT("Process %s", pFun);

    {
      DEBUG_FORMAT("Bool user %s", pFun);

      foreach (Function::EnumOutput, oEnum, pFun) {
        if (auto const pBd = oEnum.Get()->DynamicCast<BoolOutput>()) {
          RewriteBoolOutput(pBd);
        }
      }
    }

    for (auto& bblock: pFun->bblocks()) {
      BBlock::EnumI oEnumI(bblock);
      while (!oEnumI.AtEnd()) {
        auto const pI = oEnumI.Get();
        oEnumI.Next();

        if (auto const pVd = pI->GetVd()) {
          HoisttSelects(pVd, pI);
        }
        pI->Apply(this);
      }
    }

    normalizer_.Start();
  }

  // [U]
  private: void Unexpected(Instruction* const pI) {
    DEBUG_FORMAT("Unexpected instruction %s", pI);

    AddErrorInfo(
        VmErrorInfo(
          pI->source_info(),
          VmError_Compiler_InternalError,
          pI));
  }

  ////////////////////////////////////////////////////////////
  //
  // Instruction Rules
  //
  #define DefProcI(mp_Name) \
      public: void virtual Process(mp_Name ## I* const pI) override

  DefProcI(Add) { EnsureRdRx(pI); }

  DefProcI(Closure) {
    ASSERT(pI != nullptr);
    // TODO 2011-01-23 yosi@msn.com Implment CLOSURE instruction
    CAN_NOT_HAPPEN();
    //rewriteToCall(FunName::Intern(QBmake_closure), pI);
  }

  DefProcI(Div) { EnsureRdRx(pI); }

  #if 0
  DefProcI(Go) {
    Values* const pVy = new Values;
    pI->GetBB()->InsertBeforeI(
        *new ValuesI(pVy, pI->GetSx()),
        *pI);

    pI->GetBB()->ReplaceI(
        *new CallI(tyVoid, Void, QBgo, pVy),
        *pI);
  }
  #endif

  DefProcI(If) {
    if (!IsRegOperand(pI->GetSz())) {
      if (IsRegOperand(pI->GetSy())) {
        DEBUG_FORMAT("Swap operands: %s", pI);

        auto const pBoolI =
            pI->GetBx()->GetDefI()->StaticCast<x86BoolI>();

        auto const pBx = &module_.NewBoolOutput();

        pI->GetBB()->InsertBeforeI(
            *new x86BoolI(
                *pBx,
                *pBoolI->GetSx(),
                static_cast<Tttn>(pBoolI->GetTttn() ^ 1)),
            *pI);

        auto const pSy = pI->GetSy();
        auto const pSz = pI->GetSz();

        pI->GetOperandBox(0)->Replace(*pBx);
        pI->GetOperandBox(1)->SetOperand(pSz);
        pI->GetOperandBox(2)->SetOperand(pSy);

        DEBUG_FORMAT("%s %s", pI, pBoolI);

    } else {
      DEBUG_FORMAT("Liteal to Reg: %s", pI);

      //= <TODO date="2008-06-24" by="yosi@msn.com">
      //=     Should we do this by ensuring %bx def-use?
      //= </TODO>

      // Insert Sz before CMP to allow using XOR.
      // Example:
      //  (defun foo (&optional (x 0)) x)
      //
      // CMP  %q1 <= ...          <- pRefI
      // BOOL %q2 <= %q1 ...
      auto const pRefI = pI->GetBx()->GetDefI()->
          GetSx()->StaticCast<PseudoOutput>()->GetDefI();

      auto const pRz = &module_.NewRegister();

      pI->GetBBlock()->InsertBeforeI(
          *new CopyI(
              pI->output_type(), *pRz, *pI->GetSz()->StaticCast<Output>()),
          *pRefI);

          pI->GetOperandBox(2)->Replace(*pRz);
      }
    }

      EnsureRdRn(pI, pI->GetOperandBox(1));
  }

  DefProcI(LogAnd) { EnsureRdRx(pI); }
  DefProcI(LogEqv) { Unexpected(pI); }
  DefProcI(LogIor) { EnsureRdRx(pI); }
  DefProcI(LogXor) { EnsureRdRx(pI); }

  DefProcI(Mul) { EnsureRdRx(pI); }
  DefProcI(MvSave) { Pseudofy(pI); }
  //DefProcI(ReturnFrom) { rewriteToCall(QBreturn, pI); }
  //DefProcI(Slot) { Pseudofy(pI); }
  DefProcI(Sub)  { EnsureRdRx(pI); }
  //DefProcI(Tlv)  { Pseudofy(pI); }
  //DefProcI(Throw) { rewriteToCall(QBthrow, pI); }

  DefProcI(UnBox) {
    auto& type = pI->output_type();
    if (type.IsFloat()) {
      auto const pR1 = &module_.NewRegister();

      pI->GetBB()->InsertBeforeI(
          *new CopyI(type, *pR1, *pI->GetSx()->StaticCast<Output>()),
          *pI);

      pI->GetOperandBox(0)->Replace(*pR1);
    }
  }

  DefProcI(UpVarRef) { Pseudofy(pI); }

  // Inserts USE instruction for LEA for STORE.
  //  LEA %r1 <= %r2 %r3
  //  STORE %r1 %r4
  //  USE %r1
  DefProcI(x86Lea) {
    auto const pRd = pI->GetOutput()->DynamicCast<PseudoOutput>();

    if (!pRd) {
      return;
    }

    foreach (PseudoOutput::EnumUser, oEnum, pRd) {
      auto const pUserI = oEnum.Get()->GetI();

      if (pUserI->Is<StoreI>()) {
        for (auto& operand: pI->operands()) {
          if (auto const pRx = operand.DynamicCast<Register>()) {
            if (pRx->GetRegClass() == RegClass_Gpr) {
              pUserI->GetBB()->InsertAfterI(
                  *new(module_.zone()) UseI(pRx),
                  *pUserI);
            }
          }
        }
      }
    }
  }

  DISALLOW_COPY_AND_ASSIGN(EnsureTasklet);
};

} // namespace

// ctor
X86EnsureTask::X86EnsureTask(Session& session, Module& module)
    : Base("X86EnsureTask", session, module) {}

// [P]
void X86EnsureTask::ProcessFunction(Function& fn) {
  EnsureTasklet oTasklet(session(), fn.module());
  oTasklet.Run(fn);
}

} // Cg
} // Il
