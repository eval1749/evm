#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Targets - X86 Asm
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86AsmTask.h"

#include "./X86Defs.h"

#include "../../../Cg/ByteSink.h"
#include "../../../Cg/Tasks/CicsAssembler.h"
#include "../../../Ee/CodeManager.h"
#include "../../../Il/Tasks/Tasklet.h"
#include "../../../Il/Ir/VmErrorInfo.h"

#include "../../../Om.h"
#include "../../../Om/Object.h"
#include "../../../Om/Thread.h"

namespace Ee {
namespace X86 {
// See X86RuntimeWin32.cpp for implementation of InvokeInterfaceMethod.
Function* gInvokeInterfaceMethod;
} // X86
} // Ee

namespace Il {
namespace Cg {

using namespace X86;
using namespace Il::Tasks;
using Ee::CodeDesc;

struct FunObj {
  enum { Align = 16 };

  struct FunDesc {
      int m_i;
  }; // FunDec

  Function* m_pFunction;

  uint8* GetCodeStart() {
      return reinterpret_cast<uint8*>(this) + 1;
  }
}; // FunObj

void ComputeGcMap(MemoryZone* const, Function* const, ByteSink* const) {
}

inline int Ceiling(int const x, int const n) {
  return (x + n - 1) / n;
}

inline int RoundUp(int const x, int const n) {
  return Ceiling(x, n) * n;
}

#define INTERNAL_ERROR(mp_format, param, ...) \
  session().AddErrorInfo( \
      VmErrorInfo( \
          SourceInfo(__FILE__, __LINE__, 0), \
          VmError_Optimize_InternalError, \
          String::Format(__FUNCTION__ ":" mp_format, param, __VA_ARGS__)))

// TODO(yosi) We should have object manager in Cg.
Om::Thread* s_Thread;

//typedef Il::X86::Op Op;

/// <summary>
///   X86 assembler
/// </summary>
class X86Assembler :
  public CicsAssembler,
  public Tasklet {

  protected: X86Assembler(
      const char16* const pwszName,
      Session& session) :
          Tasklet(pwszName, session) {}

  protected: struct Ea {
      enum Mode {
          Mode_None,

          Mode_Annot,
          Mode_Base,
          Mode_Index,
      }; // Mode

      Annot::Kind m_eAnnot;
      Mode        m_eMode;
      Reg         m_rb;
      Reg         m_rx;
      Int         m_iDisp;
      Val         m_datum;

      Ea(Mode eMode = Mode_Base, Reg rb = $r0, Int ofs = 0) :
          m_eAnnot(Annot::Kind_None),
          m_eMode(eMode),
          m_iDisp(ofs),
          m_rb(rb),
          m_rx($r0),
          m_datum(nullptr) {}

      Ea(Reg rb, Int ofs) :
          m_eAnnot(Annot::Kind_None),
          m_eMode(Mode_Base),
          m_iDisp(ofs),
          m_rb(rb),
          m_rx($r0),
          m_datum(nullptr) {}
  }; // Ea

  protected: void asmMov(Reg rd, Reg rx) {
      when (rx == rd) return;

      emitOp(op_MOV_Gv_Ev);
      emitU8(ModRm(Mod_Reg, rd, rx));
  }

  protected: void asmMov(Reg rd, Operand* pSx) {
    if (auto const pRx = pSx->DynamicCast<Physical>()) {
      auto const rx = mapGpr(pRx);
      if (rd != rx) {
        emitOp(op_MOV_Gv_Ev);
        emitU8(ModRm(Mod_Reg, rd, rx));
      }
    } else if (StackSlot* pMx = pSx->DynamicCast<StackSlot>()) {
      asmMov(rd, $sp, pMx->offset());
    } else if (ThreadSlot* pMx = pSx->DynamicCast<ThreadSlot>()) {
      asmMov(rd, $tcb, pMx->offset());
    } else {
      emitOp(static_cast<X86::Op>(op_MOV_eAX_Iv + (rd & 7)));
      emitIz(pSx);
    }
  }

  protected: void asmMov(Reg rb, size_t ofs, Operand* const pSx) {
    if (auto const pRx = pSx->DynamicCast<Physical>()) {
      emitOp(op_MOV_Ev_Gv);
      emitDisp(mapGpr(pRx), rb, ofs);

    } else {
      emitOp(op_MOV_Ev_Iz);
      emitDisp(opext_MOV_Ev_Iz, rb, ofs);
      emitIz(pSx);
    }
  }

  protected: void asmMov(Reg rb, size_t ofs, Reg rx) {
    emitOp(op_MOV_Ev_Gv);
    emitDisp(rx, rb, ofs);
  }

  protected: void asmMov(Reg rd, Reg rb, size_t ofs) {
    emitOp(op_MOV_Gv_Ev);
    emitDisp(rd, rb, ofs);
  }

  protected: void asmXCHG(Reg r1, Reg r2) {
    if (r1 == r2) return;
    if (r2 == $r0) swap(r1, r2);

    if (r1 == $r0) {
      emitOp(static_cast<X86::Op>(op_XCHG_eAX_eAX + r2));

    } else {
      emitOp(op_XCHG_Ev_Gv);
      emitU8(ModRm(Mod_Reg, r1, r2));
    }
  }

  // [C]
  private: int computeAnnexLocation(
      Function* const pClosure,
      Variable* const pVar) {
    ASSERT(pClosure->IsClosure());

    ClosedMarker* pQd = pClosure->FindUpVar(pVar)->
        DynamicCast<ClosedMarker>();
    if (!pQd) {
      INTERNAL_ERROR("computeAnnexLocation: broken closure",  *pClosure);
      return 0;
    }

    foreach (ClosedMarker::EnumUser, oEnum, pQd) {
      if (auto const pVarAnnexI = oEnum.GetI()->DynamicCast<VarAnnexI>()) {
        return pVarAnnexI->GetOutput()->StaticCast<StackSlot>()->offset();
      }
    }

    INTERNAL_ERROR("computeAnnexLocation: broken VarAnnex",  *pClosure);
    return 0;
  }

  protected: int ComputeFixedPathOffset(
      Function* const pCallee,
      Function* const pOwner) {
    auto pCaller = pCallee;
    auto ofs = 0;
    while (nullptr != pCaller) {
      if (pCaller == pOwner) {
        return ofs;
      }

      if (pCaller->IsClosure()) {
        return ofs;
      }

      ofs += pCaller->m_cbFrame;
      pCaller = pCaller->GetSingleCaller();
    }

    // pCallee to variable owner isn't fixed path.
    DEBUG_FORMAT("Can't find path from %s to %s", pCallee, pOwner);

    INTERNAL_ERROR("computeFixedPathOffset: not fixed path", *pCallee);
    return 0;
  }

  private: int ComputeFixedPathOffset(
      Function* const pCallee,
      Variable* const pVar) {
    ASSERT(nullptr != pCallee);
    ASSERT(nullptr != pVar);

    auto const pOwner  = pVar->GetOwner();
    if (!pOwner) {
      INTERNAL_ERROR("computeFixedPathOffset: No variable owner",  *pCallee);
      return 0;
    }

    auto pCaller = pCallee;
    auto ofs = 0;
    while (nullptr != pCaller) {
      if (pCaller == pOwner) {
        if (!pVar->HasLocation()) {
          INTERNAL_ERROR("%s hasn't been allocated.", pVar);
          return 0;
        }
        return ofs + pVar->GetLocation();
      }

      if (pCaller->IsClosure()) {
          return ofs + computeAnnexLocation(pCaller, pVar);
      }

      ofs += pCaller->m_cbFrame;
      pCaller = pCaller->GetSingleCaller();
    }

    // pCallee to variable owner isn't fixed path.
    DEBUG_FORMAT("Can't find path for %s from %s to %s",
        pVar, pCallee, pOwner);

    INTERNAL_ERROR("computeFixedPathOffset: not fixed path", *pCallee);
    return 0;
  }

  // [E]
  protected: void emitDisp(Opext opext, Reg rb, ptrdiff_t ofs) {
    emitDisp(static_cast<Reg>(opext), rb, ofs);
  }

  //  [00 reg/opext r/m]             disp == 0, r/m != EBP
  //  [00 reg/opext r/m] 24          disp == 0, r/m == EBP
  //  [01 reg/opext r/m] disp8       r/m != ESP
  //  [01 reg/opext r/m] 24 disp8    r/m == ESP
  //  [10 reg/opext r/m] disp32      r/m != ESP
  //  [10 reg/opext r/m] 24 disp32   r/m == ESP
  protected: void emitDisp(Reg rd, Reg rb, ptrdiff_t ofs) {
    if (!ofs && Rm_Disp32 != (rb & 7)) {
      emitU8(ModRm(Mod_Disp0, rd, rb));
      if (Rm_Sib == (rb & 7)) emitU8(0x24);

    } else if (ofs >= -128 && ofs <= 127) {
      emitU8(ModRm(Mod_Disp8, rd, rb));
      if (Rm_Sib == (rb & 7)) emitU8(0x24);
      emitU8(static_cast<uint8>(ofs));

    } else {
      emitU8(ModRm(Mod_Disp32, rd, rb));
      if (Rm_Sib == (rb & 7)) emitU8(0x24);
      emitU32(static_cast<uint32>(ofs));
    }
  }

  protected: void emitEv(Opext opext, Physical* pRy) {
    emitEv(static_cast<Reg>(opext), pRy);
  }

  protected: void emitEv(Opext opext, Operand* pSy) {
    emitEv(static_cast<Reg>(opext), pSy);
  }

  protected: void emitEv(Reg rx, Operand* pSy) {
    if (auto const pRy = pSy->DynamicCast<Physical>()) {
      emitEv(rx, pRy);
    } else if (auto const pMy = pSy->DynamicCast<MemSlot>()) {
      emitEv(rx, pMy);

    } else if (auto const pQy = pSy->DynamicCast<PseudoOutput>()) {
      Ea oEa = GetEa(pQy);
      emitEa(&oEa, rx);
    } else {
      INTERNAL_ERROR("emitEv: unexpected operand", *pSy);
    }
  }

  protected: void emitEa(const Ea* pEa, Reg rx) {
    switch (pEa->m_eMode) {
      case Ea::Mode_Annot:
        if (Annot::Kind_TlvOfs == pEa->m_eAnnot) {
          emitU8(ModRm(Mod_Disp32, rx, $tcb));
        } else {
          emitU8(ModRm(Mod_Disp0, rx, Rm_Disp32));
        }
        emitLit(FixnumEncode(pEa->m_datum), pEa->m_eAnnot);
        break;

      case Ea::Mode_Base:
        emitDisp(rx, pEa->m_rb, pEa->m_iDisp);
        break;

      case Ea::Mode_Index:
      case Ea::Mode_None:
      default:
        INTERNAL_ERROR("emitEa: unexpecred mode %d", pEa->m_eMode);
        break;
    }
  }

  protected: void emitEa(const Ea* pEa, Opext opext) {
    emitEa(pEa, static_cast<Reg>(opext));
  }

  protected: void emitVsEv(
      X86::Op const op,
      Operand* const pSx,
      Operand* const pSy) {
    if (auto const pFx = pSx->DynamicCast<Physical>()){
      emitOp(op);
      emitEv(mapFpr(pFx), pSy);
      return;
    }

    INTERNAL_ERROR("emitVsEv: expect physical: %s", *pSx);
  }

  protected: void emitVsWs(
      X86::Op  const op,
      Operand* pSx,
      Operand* pSy) {
    if (auto const pFx = pSx->DynamicCast<Physical>()) {
      emitOp(op);
      emitEv(mapFpr(pFx), pSy);
      return;
    }

    INTERNAL_ERROR("emitVsWs: expect physical: %s", *pSx);
  }

  protected: void emitEv(Reg rx, Physical* pRy) {
    emitU8(ModRm(Mod_Reg, rx, mapGpr(pRy)));
  }

  protected: void emitEv(Reg rx, MemSlot* pMy) {
    if (pMy->Is<StackSlot>()) {
      emitDisp(rx, $sp, pMy->offset());

    } else if (pMy->Is<ThreadSlot>()) {
      emitDisp(rx, $tcb, pMy->offset());

    } else {
      INTERNAL_ERROR("emitEv: unexpected operand: %s", *pMy);
    }
  }

  protected: void emitIz(const Operand* const pSx) {
    if (auto const pLx = pSx->DynamicCast<Literal>()) {
      emitLit(reinterpret_cast<intptr_t>(pLx), Annot::Kind_Literal);
      return;
    }

    if (auto const pRx = pSx->DynamicCast<Physical>()) {
      // NYI: Closed
      // NYI: LoadTimeValue
    }

    if (auto const pFunLit = pSx->DynamicCast<FunLit>()) {
      emitLit(FixnumEncode(&pFunLit->value()), Annot::Kind_FunRef);
      return;
    }

    #if 0
    if (Integer* pInt = pSx->DynamicCast<Integer>()) {
      emitU32(static_cast<uint32>(pInt->GetValue()));
      return;
    }
    #endif

    if (auto const pCx = pSx->DynamicCast<ClosedMarker>()) {
      emitLit(FixnumEncode(pCx->GetNth()), Annot::Kind_ClosedLit);
      return;
    }

    if (auto const pLabel = pSx->DynamicCast<Label>()) {
      emitLit(FixnumEncode(pLabel->GetBB()), Annot::Kind_RelLabel);
      return;
    }

    #if 0
    if (TlvOffset* pTlvOfs = pSx->DynamicCast<TlvOffset>()) {
      emitLit(pTlvOfs->GetTlvRec(), Annot::Kind_TlvOfs);
      return;
    }
    #endif

    INTERNAL_ERROR("emitIz: unexpected operand: %s", *pSx);
  }

  protected: void EmitLit(Function* const pFunction) {
    ASSERT(nullptr != pFunction);
    EmitLit(
        Annot::Kind_LocalCallee,
        reinterpret_cast<intptr_t>(pFunction));
  }

  protected: void EmitLit(Method& method) {
    EmitLit(
        Annot::Kind_Method,
        reinterpret_cast<intptr_t>(&method));
  }

  protected: void EmitLit(Annot::Kind const eKind, intptr_t const iDatum) {
    Annotate(eKind, iDatum);
    m_oCodeBuf.EmitU32(0);
  }

  protected: void emitLit(intptr_t const iDatum, Annot::Kind const eKind) {
    Annotate(eKind, iDatum);
    m_oCodeBuf.EmitU32(0);
  }

  protected: void EmitU16(uint16 const value) {
    m_oCodeBuf.EmitU16(value);
  }

  protected: void EmitU32(uint32 const value) {
    m_oCodeBuf.EmitU32(value);
  }

  protected: void EmitU64(uint64 const value) {
    m_oCodeBuf.EmitU64(value);
  }

  // [G]
  protected: Ea GetEa(Instruction* const pI) {
    if (pI->Is<x86LeaI>()) {
      if (auto const pFx = pI->GetSx()->DynamicCast<FrameReg>()) {
        return Ea($sp, pFx->GetLocation() + pI->GetIy());
      }
      return Ea(mapGpr(pI->GetSx()), pI->GetIy());
    }

    #if 0
    // TODO 2011-01-23 yosi@msn.com instance field.
    if (pI->Is<SlotI>()) {
      if (auto const pCz = pI->GetSz()->DynamicCast<ClosedMarker>()) {
        Ea oEa(Ea::Mode_Annot);
        oEa.m_eAnnot = Annot::Kind_ClosedVar;
        oEa.m_datum  = FixnumEncode(pCz->GetNth());
        return oEa;
      }

      if (auto const pLz = pI->GetSz()->DynamicCast<Literal>()) {
        Val cell = pLz->GetValue();
        Ea oEa(Ea::Mode_Annot);
        oEa.m_eAnnot = Annot::Kind_SymVal;
        oEa.m_datum  = cell;
        return oEa;
      }

      if (pI->GetLx() == CLASS_stack_cell) {
        Ea oEa(mapGpr(pI->GetSz()), 0);
        return oEa;
      }
    }
    #endif

    #if 0
    if (pI->Is<TlvI>()) {
      Ea oEa(Ea::Mode_Annot);
      oEa.m_eAnnot = Annot::Kind_TlvOfs;
      oEa.m_datum  = pI->GetLx();
      return oEa;
    }
    #endif

    if (pI->Is<UpVarRefI>()) {
      auto const pVar = pI->GetSy()->StaticCast<PseudoOutput>()->
          GetDefI()->GetSx()->StaticCast<Variable>();

      auto const rb = mapGpr(pI->GetSx());
      if (rb == $sp) {
        auto const ofs = ComputeFixedPathOffset(
            pI->GetBB()->GetFunction(),
            pVar);
        return Ea($sp, ofs);
      }

      if (static_cast<Reg>(0) != rb) {
        if (!pVar->HasLocation()) {
          INTERNAL_ERROR("%s should be allocated.", *pVar);
        }
        return Ea(rb, pVar->GetLocation());
      }

      INTERNAL_ERROR("GetEa: UpVarRef base access: %s", *pI);
      return Ea();
    }

    DEBUG_FORMAT("GetEa: Unexpected instruction %s", pI);
    INTERNAL_ERROR("GetEa: unexpcted instruciton: %s", *pI);
    return Ea();
  }

  protected: Ea GetEa(Operand* const pSx) {
    if (auto const pQx = pSx->DynamicCast<PseudoOutput>()) {
      return GetEa(pQx->GetDefI());
    }

    if (auto const pRx = pSx->DynamicCast<Physical>()) {
      return Ea(Ea::Mode_Base, mapGpr(pRx));
    }

    INTERNAL_ERROR("GetEa: unexpected operand: %s", *pSx);
    return Ea();
  }

  protected: Reg mapFpr(Physical* pRx) {
    const RegDesc* const pDesc = pRx->GetDesc();
    if (pDesc->m_eRegClass != RegClass_Fpr) {
      INTERNAL_ERROR("Expect FPR: %s", *pRx);
      return static_cast<Reg>(0);
    }
    return static_cast<Reg>(pDesc->m_nId);
  }

  protected: Reg mapFpr(Operand* pSx) {
    if (auto const pRd = pSx->DynamicCast<Physical>()) {
      return mapFpr(pRd);
    }

    INTERNAL_ERROR("Expect FPR: %s", *pSx);
    return static_cast<Reg>(0);
  }

  protected: Reg mapGpr(Physical* pRx) {
    const RegDesc* const pDesc = pRx->GetDesc();
    #if 0
        if (pDesc->m_eRegClass != RegClass_Gpr) {
          INTERNAL_ERROR("Expect GPR.");
          return static_cast<Reg>(0);
        }
    #endif
    return static_cast<Reg>(pDesc->m_nId);
  }

  protected: Reg mapGpr(Operand* pSx) {
    if (auto const pRd = pSx->DynamicCast<Physical>()) {
      return mapGpr(pRd);
    }

    INTERNAL_ERROR("Expect GPR: %s", *pSx);
    return static_cast<Reg>(0);
  }

  protected: Int getIv(Immediate* pSx) {
    if (auto const pLx = pSx->DynamicCast<Literal>()) {
      if (pLx->CanBeInt32()) {
        return pLx->GetInt32();
      }
    }

    #if 0
    if (Integer* pInt = pSx->DynamicCast<Integer>()) {
      return pInt->GetValue();
    }
    #endif

      return 0x100;
  }

  protected: Tttn getTttn(BoolOutput* const pBx) {
    return pBx->GetDefI()->StaticCast<x86BoolI>()->GetTttn();
  }

  DISALLOW_COPY_AND_ASSIGN(X86Assembler);
}; // X86Assembler

//
/// <summary>
///   X86 assemble pass
/// </summary>
class X86AsmTasklet :
  protected X86Functor,
  protected X86Assembler,
  protected X86Utility {

  private: typedef X86Assembler Base;

  private: Module& module_;

  public: X86AsmTasklet(Session& session, Module& module)
      : Base(L"X86Asm", session), module_(module) {}

  public: void Run() {
    DEBUG_FORMAT("Assemble %s", module_);
    for (auto& fun: module_.functions()) {
      ProcessFun(&fun);
      ConstructFun(&fun);
    }

    DEBUG_FORMAT("Anootate %s", module_);
    for (auto& fun: module_.functions())
      AnnoteFun(fun);
  }

  // [A]
  /// <summary>
  ///     This method anootates code in function.
  /// </summary>
  private: void AnnoteFun(const Function& fun) {
    auto& code_desc = *fun.code_desc();
    foreach (Annots::Enum, oEnum, &fun.GetWork<FunExt>()->m_oAnnots) {
      auto& annot = *oEnum.Get();
      switch (annot.GetKind()) {
        case Annot::Kind_AbsLabel:
        case Annot::Kind_Callee:
        case Annot::Kind_ClosedLit:
        case Annot::Kind_ClosedVar:
          goto case_NotSupported;

        case Annot::Kind_FunRef: {
          auto& fun = *reinterpret_cast<Function*>(annot.m_value);
          DEBUG_FORMAT("0x%04X FunRef %s", annot.m_ofs, fun);
          if (!fun.code_desc()) {
            INTERNAL_ERROR("%s must have CodeDesc.", fun);
            break;
          }

          code_desc.SetPointer(
              CodeDesc::Annotation(
                  CodeDesc::Annotation::Kind_FunRef,
                  annot.GetAddr()),
               fun.code_desc()->codes());
          break;
        }

        case Annot::Kind_Literal: {
          auto& lit = *reinterpret_cast<Literal*>(annot.m_value);
          DEBUG_FORMAT("0x%04X Literal %s", annot.m_ofs, lit);
          if (auto const strlit = lit.DynamicCast<StringLiteral>()) {
            auto str = strlit->value();
            auto& strobj  = s_Thread->NewString(str);
            code_desc.SetPointer(
                CodeDesc::Annotation(
                    CodeDesc::Annotation::Kind_Object,
                    annot.GetAddr()),
                &strobj);
            break;
          }
          INTERNAL_ERROR("NYI: AnnoteFun: literal %s", lit);
          break;
        }

        case Annot::Kind_LocalCallee: {
          auto const fun = reinterpret_cast<Function*>(annot.m_value);
          if (!fun->code_desc()) {
            INTERNAL_ERROR("%s isn't ready", *fun);
            break;
          }
          DEBUG_FORMAT("0x%04X LocalCallee %s", annot.m_ofs, fun);
          code_desc.SetPointer(
              CodeDesc::Annotation(
                  CodeDesc::Annotation::Kind_Callee,
                  annot.GetAddr()),
              fun);
          break;
        }

        case Annot::Kind_Method: {
          auto const method = reinterpret_cast<Method*>(annot.m_value);
          DEBUG_FORMAT("0x%04X Method %s", annot.m_ofs, method);
          if (!method->GetFunction()) {
            if (method->IsExtern()) {
              INTERNAL_ERROR("NYI: external method %s", *method);
              break;
            }

            INTERNAL_ERROR("%s should have method function.", *method);
            break;
          }

          code_desc.SetPointer(
              CodeDesc::Annotation(
                  CodeDesc::Annotation::Kind_Method,
                  annot.GetAddr()),
              method);
          break;
        }

        case Annot::Kind_MethodRef: {
          auto& method = *reinterpret_cast<Method*>(annot.m_value);
          DEBUG_FORMAT("0x%04X MethodRef %s", annot.m_ofs, method);
          code_desc.SetPointer(
              CodeDesc::Annotation(
                  CodeDesc::Annotation::Kind_MethodRef,
                  annot.GetAddr()),
               &method);
          break;
        }

        case Annot::Kind_RelLabel:
        case Annot::Kind_SymSetf:
        case Annot::Kind_SymFun:
        case Annot::Kind_SymVal:
        case Annot::Kind_TlvOfs:
        case_NotSupported:
          INTERNAL_ERROR("NYI: AnnoteFun: annotation %d",
              annot.GetKind());
          break;

        case Annot::Kind_None:
        case Annot::Kind_Limit:
        default:
          INTERNAL_ERROR("Internal Error: Bad annotation %d",
              annot.GetKind());
          break;
      }
    }

    if (!session().HasError()) {
      code_desc.FixAfterMove();
    }
  }

  // [C]
  /// <summary>
  ///     <list type="number">
  ///         <item>Allocate function object</item>
  ///         <item>Copy code into code block</item>
  ///     </list>
  ///     This function object is still incomplete due by reference to
  ///     other functions in same function set.
  /// </summary>
  protected: void ConstructFun(Function* const pFun) {
      LocalMemoryZone oMm;
      ByteSink oGcMap(&oMm);

      ComputeGcMap(&oMm, pFun, &oGcMap);

      auto const pExt = pFun->GetWork<FunExt>();

      auto const pAnnotations = new ArrayList_<Ee::CodeDesc::Annotation>(
          pExt->m_oAnnots.Count());

      foreach (Annots::Enum, oEnum, &pFun->GetWork<FunExt>()->m_oAnnots) {
        auto& annot = *oEnum.Get();

        auto eKind = Ee::CodeDesc::Annotation::Kind_None;

        switch (annot.GetKind()) {
          case Annot::Kind_AbsLabel:
          case Annot::Kind_Callee:
          case Annot::Kind_ClosedLit:
          case Annot::Kind_ClosedVar:
            goto case_NotSupported;

          case Annot::Kind_FunRef:
            eKind = CodeDesc::Annotation::Kind_FunRef;
            break;

          case Annot::Kind_Literal: {
            auto& lit = *reinterpret_cast<Literal*>(annot.m_value);
            if (auto const strlit = lit.DynamicCast<StringLiteral>()) {
              eKind = CodeDesc::Annotation::Kind_Object;
            } else {
              INTERNAL_ERROR("NYI: literal %s", lit);
            }
            break;
          }

          case Annot::Kind_LocalCallee:
            eKind = CodeDesc::Annotation::Kind_Callee;
            break;

          case Annot::Kind_Method:
            eKind = CodeDesc::Annotation::Kind_Method;
            break;

          case Annot::Kind_MethodRef:
            eKind = CodeDesc::Annotation::Kind_MethodRef;
            break;

          case Annot::Kind_RelLabel:
          case Annot::Kind_SymSetf:
          case Annot::Kind_SymFun:
          case Annot::Kind_SymVal:
          case Annot::Kind_TlvOfs:
          case_NotSupported:
            INTERNAL_ERROR("NYI: annotation %d",
                annot.GetKind());
            break;

          case Annot::Kind_None:
          case Annot::Kind_Limit:
          default:
            INTERNAL_ERROR("Internal Error: Bad annotation %d",
                annot.GetKind());
            break;
        }

        if (eKind != Annot::Kind_None) {
          DEBUG_PRINTF("Annotation %d %p\n", eKind, annot.GetAddr());
          pAnnotations->Add(
              Ee::CodeDesc::Annotation(eKind, annot.GetAddr()));
        }
      }

      auto& code_desc = Ee::CodeManager::Get()->NewCodeDesc(
          *pFun,
          *pAnnotations,
          m_oCodeBuf.GetAddr());

      SerializeCode(code_desc.codes());
      pFun->set_code_desc(code_desc);
  }

  // [G]
  // for x86Test
  private: Int getIb(Operand* const pSy) {
      ASSERT(nullptr != pSy);

      if (auto const pLy = pSy->DynamicCast<Literal>()) {
          // For KEYSUPPLIED
          if (pLy->CanBeInt32()) {
              return pLy->GetInt32();
          }
      }
      INTERNAL_ERROR("Unsupported operand: %s", *pSy);
      return 0x100;
  }

  // [P]
  private: void  ProcessAdjustFrame(
      Opext const opextIb,
      Opext const opextIz,
      int const cbFrame) {

      if (isS8(cbFrame)) {
          emitOp(op_SUB_Ev_Ib);
          emitU8(ModRm(Mod_Reg, opextIb, $sp));
          emitU8(static_cast<uint8>(cbFrame));

      } else {
          emitOp(op_SUB_Ev_Iz);
          emitU8(ModRm(Mod_Reg, opextIz, $sp));
          emitU32(cbFrame);
      }
  }

  private: void parseArith(
      Instruction* const pI,
      Opext const opext,
      X86::Op const fop32,
      X86::Op const fop64) {

      if (pI->output_type() == *Ty_Float32) {
          emitOp(fop32);
          emitEv(mapFpr(pI->GetOutput()), pI->GetSy());
          return;
      }

      if (pI->output_type() == *Ty_Float64) {
          emitOp(fop64);
          emitEv(mapFpr(pI->GetOutput()), pI->GetSy());
          return;
      }

      parseIntArith(pI, opext);
  }

  private: void parseIntArith(
      Instruction* const pI,
      Opext const opext) {

      auto const pSx = pI->GetSx();

      if (opext_CMP_Ev_Iz != opext) {
          if (pI->GetOutput() != pSx) {
              INTERNAL_ERROR("parseIntArith: bad instruction: %s", *pI);
              return;
          }
      }

      auto pSy = pI->GetSy();

      // COPY EDX <= NIL
      // CMP  EAX, NIL
      //  =>
      // CMP EAX, EDX
      if (auto const pPrevI = pI->GetPrev()) {
          if (pPrevI->Is<CopyInstruction>()) {
              if (auto const pPd =
                      pPrevI->GetOutput()->DynamicCast<Physical>()) {
                  if (*pPrevI->GetSx() == *pSy) {
                      pSy = pPd;
                  }
              }
          }
      }

      if (auto const pRx = pSx->DynamicCast<Physical>()) {
          if (auto const pRy = pSy->DynamicCast<Physical>()) {
              emitOp(op_ADD_Gv_Ev + opext * 8);
              emitU8(ModRm(Mod_Reg, mapGpr(pRx), mapGpr(pRy)));
              return;
          }

          if (auto const pMy = pSy->DynamicCast<MemSlot>()) {
              emitOp(op_ADD_Gv_Ev + opext * 8);
              emitEv(mapGpr(pRx), pMy);
              return;
          }

          if (auto const pIy = pSy->DynamicCast<Immediate>()) {
              parseIntArith(opext, pSx, pIy);
              return;
          }
      }

      if (auto const pMx = pSx->DynamicCast<MemSlot>()) {
          if (auto const pRy = pSy->DynamicCast<Physical>()) {
              emitOp(op_ADD_Ev_Gv + opext * 8);
              emitEv(mapGpr(pRy), pMx);
              return;
          }

          if (auto const pIy = pSy->DynamicCast<Immediate>()) {
              parseIntArith(opext, pSx, pIy);
              return;
          }
      }

      INTERNAL_ERROR("parseIntArith: unexpected operand: %s", *pSx);
  }

  private: void parseIntArith(
      Opext const opext,
      Operand* const pSx,
      Immediate* const pIy) {

      intptr_t const iIv = getIv(pIy);

      // CMP r, 0 => TEST r, 0
      if (auto const pRx = pSx->DynamicCast<Physical>()) {
          if (opext_CMP_Ev_Iz == opext && 0 == iIv) {
              Reg rx = mapGpr(pRx);
              emitOp(op_TEST_Ev_Gv);
              emitU8(ModRm(Mod_Reg, rx, rx));
              return;
          }
      }

      if (isS8(iIv)) {
          emitOp(op_ADD_Ev_Ib);
          emitEv(opext, pSx);
          emitU8(static_cast<uint8>(iIv));
          return;
      }

      emitOp(op_ADD_Ev_Iz);
      emitEv(opext, pSx);
      emitIz(pIy);
  }

  private: void parseShift(Instruction* const pI, Opext const opext) {
    auto& shift_inst = *pI;
    auto& src = shift_inst.op0();
    auto& shift_amount = shift_inst.op1();

    if (auto const imm = shift_amount.DynamicCast<Literal>()) {
      if (imm->CanBeInt32()) {
        auto const i32 = imm->GetInt32();
        if (i32 == 1) {
          emitOp(op_SHL_Ev_1);
          emitEv(opext, &src);
        } else if (i32 < shift_inst.output_type().bit_width()) {
          emitOp(op_SHL_Ev_Ib);
          emitEv(opext, &src);
          emitU8(static_cast<uint8>(i32));
        } else {
          INTERNAL_ERROR("Shift: too larget operand %s", shift_amount);
        }
        return;
      }

      INTERNAL_ERROR("Shift: invalid operand %s", src);
      return;
    }

    if (mapGpr(&shift_amount) == $ECX) {
      emitOp(op_SHL_Ev_CL);
      emitEv(opext, &src);
      return;
    }

    INTERNAL_ERROR("Shift: invalid operand: %s", src);
  }

  private: void ProcessBBlock(BBlock* const pBBlock) {
      auto const pExt = pBBlock->GetWork<BBlockExt>();
      pExt->m_ofs   = m_oCodeBuf.GetOffset();
      pExt->m_nAddr = m_oCodeBuf.GetAddr();

      foreach (BBlock::EnumI, oEnum, pBBlock) {
          auto const pI = oEnum.Get();
          pI->SetIndex(m_oCodeBuf.GetAddr());
          pI->Apply(this);
      }
  }

  private: void ProcessFun(Function* const pFun) {
      m_pFun = pFun;

      // Allocate stack slot in frame for return address.
      pFun->m_cbFrame += sizeof(Val);

      FunExt* pExt = new(this) FunExt;
      pFun->SetWork(pExt);

      m_oCodeBuf.Reset();

      foreach (Function::EnumBBlock, oEnum, pFun) {
          auto const pBBlock = oEnum.Get();
          auto const pExt = new(this) BBlockExt;
          pBBlock->SetWork(pExt);
      }

      foreach (Function::EnumBBlock, oEnum, pFun) {
          ProcessBBlock(oEnum.Get());
      }

      FixSpans();
  }

  private: void processCopy(Instruction* const pCopy);

  private: void processEmit(Instruction*, uint nOp) { emitOp(nOp); }

  private: void processUnexpected(Instruction* const pI) {
    auto& inst = *pI;
    session().AddErrorInfo(
        VmErrorInfo(
            inst.source_info(),
            VmError_Pass_UnexpectedInstruction,
            name(),
            inst));
  }

  // Dispatch table

  // Note: We should not override Process(Instruction*). X86Functor
  // use it for dispatching CgInstruction and X86Instruction.
  private: virtual void Process(IrObject* const obj) override {
    if (auto const inst = obj->DynamicCast<Instruction>()) {
      processUnexpected(inst);
    }
  }

  #define DefProcI(mp_Name) \
      private: void virtual Process(mp_Name ## I* const pI) override

  #define DefProcI_Arith(mp_Name, mp_NAME) \
      DefProcI(mp_Name) { \
          parseArith(pI, \
              opext_ ## mp_NAME ## _Ev_Iz, \
              op_ ## mp_NAME ## SS_Vss_Wss, \
              op_ ## mp_NAME ## SD_Vsd_Wsd); \
      }

  //= <TODO date="2008-12-24" by="yosi@msn.com">
  //=  We must have parser for DIV and MUL.
  //= </TODO>
  #define opext_DIV_Ev_Iz opext_ADC_Ev_Iz
  #define opext_MUL_Ev_Iz opext_ADC_Ev_Iz

  DefProcI(Assign)    { processCopy(pI); }
  DefProcI_Arith(Add, ADD)

  DefProcI(Branch);

  DefProcI(Call);
  DefProcI(Copy) { processCopy(pI); }

  DefProcI_Arith(Div, DIV)

  DefProcI(Entry) { ASSERT(nullptr != pI); }
  DefProcI(Exit);

  DefProcI(If);

  DefProcI(Jump);

  DefProcI(Load);
  DefProcI(LogAnd)    { parseIntArith(pI, opext_AND_Ev_Iz); }
  DefProcI(LogIor)    { parseIntArith(pI, opext_OR_Ev_Iz); }
  DefProcI(LogXor)    { parseIntArith(pI, opext_XOR_Ev_Iz); }

  DefProcI(Mul);

  DefProcI(PhiCopy)   { processCopy(pI); }
  DefProcI(Prologue);

  DefProcI(ReinterpretCast) { processCopy(pI); }
  DefProcI(Reload);
  DefProcI(Ret);

  // TODO(yosi) 2012-03-25 Temporary ignore RuntimeCastI.
  DefProcI(RuntimeCast) { ASSERT(pI != nullptr); }

  DefProcI(Shl)        { parseShift(pI, opext_SHL_Ev_CL); }
  DefProcI(Shr);
  DefProcI(Spill)      { processCopy(pI); }
  DefProcI(StaticCast) { processCopy(pI); }
  DefProcI(Store);
  DefProcI_Arith(Sub, SUB)
  DefProcI(Swap);

  DefProcI(UnBox);
  DefProcI(UpVarBase);
  DefProcI(Use) { ASSERT(nullptr != pI); }

  DefProcI(VarDef);
  DefProcI(VarRef);
  DefProcI(VarAnnex)  { processCopy(pI); }

  ////////////////////////////////////////////////////////////
  //
  // Dispatch table for x86 instructions
  //
  DefProcI(x86Bool) { ASSERT(nullptr != pI); }
  DefProcI(x86Clc) { processEmit(pI, op_CLC); }
  DefProcI(x86Cmp) { parseIntArith(pI, opext_CMP_Ev_Iz); }

  DefProcI(x86CmpF32)
      { emitVsWs(op_COMISS_Vss_Wss, pI->GetSx(), pI->GetSy()); }

  DefProcI(x86CmpF64)
      { emitVsWs(op_COMISD_Vsd_Wsd, pI->GetSx(), pI->GetSy()); }

  DefProcI(x86CvtFloat);
  DefProcI(x86CvtInt);

  DefProcI(x86Encode);

  DefProcI(x86Lea);

  DefProcI(x86Stc) { processEmit(pI, op_STC); }

  DefProcI(x86Test);

  DefProcI(x86Zero);

  DISALLOW_COPY_AND_ASSIGN(X86AsmTasklet);
}; // X86AsmTasklet

#define THIS_PASS X86AsmTasklet
#define DefProcI_(mp_Name) \
void THIS_PASS :: Process(mp_Name ## I* const pI)

DefProcI_(Branch) {
  BranchI* pBranchI = pI->StaticCast<BranchI>();

  BBlock* pFalseBB = pBranchI->GetFalseBB();
  BBlock* pTrueBB  = pBranchI->GetTrueBB();

  Tttn eTttn = getTttn(pBranchI->GetBx());

  BBlock* pNextBB = pI->GetBB()->GetNext();
  if (pNextBB == pFalseBB) {
      emitJump(op_Jcc_Jb + eTttn, op_Jcc_Jv + eTttn, pTrueBB);

  } else if (pNextBB == pTrueBB) {
      eTttn = FlipTttn(eTttn);
      emitJump(op_Jcc_Jb + eTttn, op_Jcc_Jv + eTttn, pFalseBB);

  } else {
      emitJump(op_Jcc_Jb + eTttn, op_Jcc_Jv + eTttn, pTrueBB);
      emitJump(op_JMP_Jb, op_JMP_Jv, pFalseBB);
  }
}

#if 0
struct ThreadSvcEntry {
  Val m_name;
  int m_ofs;
}; // ThreadSvcEntry

ThreadSvcEntry const
k_rgoThreadSvc[] = {
  { QPtype_error,             ThreadExtra_(TypeError) },
  { QPundefined_function,     ThreadExtra_(UndefinedFunction) },
}; // k_rgoThreadSvc
#endif

DefProcI_(Call) {
  auto& call_inst = *pI;
  ASSERT(call_inst.op1() == *Void);

  auto const pSx = call_inst.GetSx();

  if (auto const pFunction = pSx->DynamicCast<Function>()) {
    emitOp(op_CALL_Jv);
    EmitLit(pFunction);
    return;
  }

  if (auto const method = pSx->DynamicCast<Method>()) {
    if (method->owner_class().IsInterface()) {
      // MOV [$thread.param_], Method
      // Call InvokeInterfaceMethod
      Ea ea(Ea::Mode_Base, $EBP, offsetof(Om::Thread::Context, param_));
      emitOp(op_MOV_Ev_Iz);
      emitEa(&ea, opext_MOV_Ev_Iz);
      EmitLit(Annot::Kind_MethodRef, reinterpret_cast<intptr_t>(method));
      emitOp(op_CALL_Jv);
      EmitLit(Ee::X86::gInvokeInterfaceMethod);
      return;
    }

    emitOp(op_CALL_Jv);
    EmitLit(*method);
    return;
  }

  INTERNAL_ERROR("Unsupported callee %s", *pI);
}

DefProcI_(Exit) {
  ASSERT(nullptr != pI);
}

DefProcI_(If) {
  if (pI->GetOutput() != pI->GetSy()) {
      AddErrorInfo(
          VmErrorInfo(
            pI->source_info(),
            VmError_Instruction_Invalid,
            pI));
      return;
  }

  if (pI->GetSy() == pI->GetSz()) {
      INTERNAL_ERROR("asm.IF Bad operands: %s", *pI);
      return;
  }

  auto const eTttn = FlipTttn(getTttn(pI->GetBx()));
  emitOp(op_CMOVcc_Gv_Ev + eTttn);
  emitEv(mapGpr(pI->GetOutput()), pI->GetSz());
}

DefProcI_(Jump) {
  auto const pJumpI = pI->StaticCast<JumpI>();
  auto const pTargetBB = pJumpI->GetTargetBB();

  if (pTargetBB == pI->GetBB()->GetNext()) {
      return;
  }

  emitJump(op_JMP_Jb, op_JMP_Jv, pTargetBB);
}

DefProcI_(Load) {
  auto const oEa = GetEa(pI->GetSx());
  emitOp(op_MOV_Gv_Ev);
  emitEa(&oEa, mapGpr(pI->GetOutput()));
  return;
}

#if 0
DefProcI_(LoadFun) {
  Val cell;
  if (auto const pLx = pI->GetSx()->DynamicCast<Literal>())
  {
      cell = pLx->GetValue();
  }
  else if (FunName* pFunName = pI->GetSx()->DynamicCast<FunName>())
  {
      cell = pFunName->name();
  }
  else
  {
      processUnexpected(pI);
      return;
  }

  Reg const rd = mapGpr(pI->GetOutput());

  if (symbolp(cell))
  {
      emitOp(op_MOV_Gv_Ev);
      emitU8(ModRm(Mod_Disp0, rd, Rm_Disp32));
      emitLit(cell, Annot::Kind_SymFun);
  }
  else if (cell->Is<SetfCell>())
  {
      emitOp(op_MOV_Gv_Ev);
      emitU8(ModRm(Mod_Disp0, rd, Rm_Disp32));
      emitLit(cell, Annot::Kind_SymSetf);
  }
  else
  {
      INTERNAL_ERROR("LoadFun: invalid cell");
  }
}
#endif

void X86AsmTasklet::processCopy(Instruction* pI)
{
  auto const pOd = pI->GetOutput();
  auto const pSx = pI->GetSx();

  if (auto const pRd = pOd->DynamicCast<Physical>()) {
      auto const rd = mapGpr(pRd);

      if (auto const pLx = pSx->DynamicCast<Literal>()) {
          if (pLx->CanBeInt32()
              && pLx->GetInt32() == 0
              && !pI->GetNext()->Is<IfI>()) {
              emitOp(op_XOR_Ev_Gv);
              emitU8(ModRm(Mod_Reg, rd, rd));
              return;
          }
      }

      asmMov(rd, pSx);

  } else if (auto const pMd = pOd->DynamicCast<StackSlot>()) {
      asmMov($sp, pMd->offset(), pSx);

  } else if (auto const pMd = pOd->DynamicCast<ThreadSlot>()) {
      asmMov($tcb, pMd->offset(), pSx);

  } else {
      INTERNAL_ERROR("Copy: invalid operand: %s", *pI);
  }
}

DefProcI_(Mul) {
  auto& mul_inst = *pI;
  if (auto const imm = mul_inst.op1().DynamicCast<Literal>()) {
    if (!imm->CanBeInt32()) {
      INTERNAL_ERROR("Expect int32: %s", mul_inst.op1());
      return;
    }
    auto const rd = mapGpr(&mul_inst.output());
    auto const rx = mapGpr(&mul_inst.op0());
    emitOp(op_IMUL_Gv_Ev_Iz);
    emitU8(ModRm(Mod_Reg, rd, rx));
    emitU32(imm->GetInt32());
    return;
  }

  if (mapGpr(&mul_inst.output()) == $EAX) {
    emitOp(op_IMUL_Ev);
    emitU8(ModRm(Mod_Reg, opext_MUL_Ev, mapGpr(mul_inst.GetRy())));
  } else {
    INTERNAL_ERROR("Invalid output: %s", mul_inst);
  }
}

DefProcI_(Prologue) {
  ASSERT(nullptr != pI);
#if 0
  // Restify
  Val const restify = pI->GetLy();
  if (nil != restify) {
      asmMov($tcb, offsetof(Thread, m_fn), new Literal(*Ty_Int32, max));
      emitOp(op_CALL_Jv);
      emitLit(restify, Annot::Kind_NamedCallee);
  }
#endif

  auto const cbFrame = m_pFun->m_cbFrame - 4;
  ASSERT(0 == cbFrame % sizeof(Val));

  if (cbFrame > 0) {
      ProcessAdjustFrame(opext_SUB_Ev_Ib, opext_SUB_Ev_Iz, cbFrame);
  }
}

DefProcI_(Ret) {
  if (pI->Equals(pI->GetBB()->GetNext()->GetFirstI())) {
      // Share RET instruction with next bblock.
      return;
  }

  auto const cbFrame = m_pFun->m_cbFrame - 4;
  ASSERT(0 == cbFrame % sizeof(void*));

  if (cbFrame > 0) {
      emitOp(op_LEA_Gv_M);
      emitDisp($sp, $sp, cbFrame);
  }

  //= <TODO date="2008-12-25" by="yosi@msn.com">
  //=   NYI: Tail call optimization:
  //=     CALL+RET = JMP
  //= </TODO>

  emitOp(op_RET);
}

DefProcI_(Reload) {
  auto const pMx = pI->GetSx()->StaticCast<StackSlot>();
  asmMov(
      mapGpr(pI->GetOutput()),
      $sp,
      pMx->offset());
}

DefProcI_(Shr) {
  auto& outy = pI->output_type();

  parseShift(
      pI,
      outy == *Ty_UInt8
          || outy == *Ty_UInt16
          || outy == *Ty_UInt32
      ? opext_SHR_Ev_1
      : opext_SAR_Ev_1);
}

// Dispatch by source operand type.
DefProcI_(Store) {
  auto& store_inst = *pI;
  auto& src = store_inst.op1();
  Ea oEa = GetEa(pI->GetSx());

  if (auto const pRy = src.DynamicCast<Physical>()) {
    auto const ry = mapGpr(pRy);

    if (Ea::Mode_Annot == oEa.m_eMode
            && Annot::Kind_TlvOfs != oEa.m_eAnnot
            && $EAX == ry) {
      emitOp(op_MOV_Ov_eAX);
      emitLit(FixnumEncode(oEa.m_datum), oEa.m_eAnnot);
      return;
    }

    switch (src.type().bit_width()) {
      case 0:
      case 32:
        emitOp(op_MOV_Ev_Gv);
        emitEa(&oEa, ry);
        return;

      case 1:
      case 8:
        emitOp(op_MOV_Eb_Gb);
        emitEa(&oEa, ry);
        return;

      case 16:
        emitU8(op_OPDSIZ);
        emitOp(op_MOV_Ev_Gv);
        emitEa(&oEa, ry);
        return;

      default:
        INTERNAL_ERROR("Unsupported operand size: %s", store_inst);
        return;
    }
  }

  if (auto const boollit = src.DynamicCast<BooleanLiteral>()) {
    emitOp(op_MOV_Eb_Ib);
    emitEa(&oEa, opext_MOV_Eb_Ib);
    emitU8(boollit->value());
    return;
  }

  if (auto const i8lit = src.DynamicCast<Int8Literal>()) {
    emitOp(op_MOV_Eb_Ib);
    emitEa(&oEa, opext_MOV_Eb_Ib);
    emitU8(i8lit->value());
    return;
  }

  if (auto const u8lit = src.DynamicCast<UInt8Literal>()) {
    emitOp(op_MOV_Eb_Ib);
    emitEa(&oEa, opext_MOV_Eb_Ib);
    emitU8(u8lit->value());
    return;
  }

  if (auto const charlit = src.DynamicCast<CharLiteral>()) {
    emitU8(op_OPDSIZ);
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    EmitU16(charlit->value());
    return;
  }

  if (auto const i16lit = src.DynamicCast<Int16Literal>()) {
    emitU8(op_OPDSIZ);
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    EmitU16(i16lit->value());
    return;
  }

  if (auto const u16lit = src.DynamicCast<UInt16Literal>()) {
    emitU8(op_OPDSIZ);
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    EmitU16(u16lit->value());
    return;
  }

  if (auto const f32lit = src.DynamicCast<Float32Literal>()) {
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    EmitU32(f32lit->ToUInt32());
    return;
  }

  if (auto const i32lit = src.DynamicCast<Int32Literal>()) {
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    EmitU32(i32lit->value());
    return;
  }

  if (auto const u32lit = src.DynamicCast<UInt32Literal>()) {
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    EmitU32(u32lit->value());
    return;
  }

  if (auto const f64lit = src.DynamicCast<Float64Literal>()) {
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    EmitU64(f64lit->ToUInt64());
    return;
  }

  if (auto const i64lit = src.DynamicCast<Int64Literal>()) {
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    EmitU64(i64lit->value());
    return;
  }

  if (auto const u64lit = src.DynamicCast<UInt64Literal>()) {
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    EmitU64(u64lit->value());
    return;
  }

  if (auto const funlit = src.DynamicCast<FunLit>()) {
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    emitIz(funlit);
    return;
  }

  if (auto const strlit = src.DynamicCast<StringLiteral>()) {
    emitOp(op_MOV_Ev_Iz);
    emitEa(&oEa, opext_MOV_Ev_Iz);
    emitIz(strlit);
    return;
  }

  INTERNAL_ERROR("Unsupported source operand: %s", store_inst);
}

DefProcI_(Swap)
{
  emitOp(op_XCHG_Ev_Gv);
  emitU8(ModRm(
      Mod_Reg,
      mapGpr(pI->GetOutput()),
      mapGpr(pI->GetSx()->StaticCast<Physical>())));
}

DefProcI_(UnBox) {
#if 0
  if (pI->output_type() == *Ty_Float32) {
      Reg const rd = mapFpr(pI->GetOutput());
      Reg const rx = mapGpr(pI->GetSx());

      emitOp(op_MOVSS_Vss_Wss);
      emitDisp(rd, rx, offsetof(Layout_single_float, m_flt) - Record::Tag);
      return;
  }

  if (pI->output_type() == *Ty_Float64) {
      Reg const rd = mapFpr(pI->GetOutput());
      Reg const rx = mapGpr(pI->GetSx());

      emitOp(op_MOVSD_Vsd_Wsd);
      emitDisp(rd, rx, offsetof(Layout_double_float, m_dbl) - Record::Tag);
      return;
  }
#endif
  processUnexpected(pI);
}

DefProcI_(UpVarBase) {
  auto const ofs = ComputeFixedPathOffset(
      pI->GetBB()->GetFunction(),
      pI->GetSx()->StaticCast<Function>());

  emitOp(op_LEA_Gv_M);
  emitDisp(mapGpr(pI->GetOutput()), $sp, ofs);
}

DefProcI_(VarDef) {
  auto const pVar = pI->GetSx()->StaticCast<Variable>();

  switch (pVar->GetStorage()) {
  case Variable::Storage_Closed:
  case Variable::Storage_Literal:
      break;

  case Variable::Storage_Stack:
      if (!pVar->HasLocation()) {
          INTERNAL_ERROR("VarDef: Variable isn't allocated: %s", *pI);
      }
      break;

  case Variable::Storage_Register:
  default:
      COMPILER_INTERNAL_ERROR();
      break;
  }
}

DefProcI_(VarRef) {
  VarDefI const* pVarDefI = pI->GetQx()->GetDefI()->StaticCast<VarDefI>();
  Variable* const pVar = pVarDefI->GetSx()->StaticCast<Variable>();
  ASSERT(Variable::Storage_Stack == pVar->GetStorage());

  if (!pVar->HasLocation()) {
      COMPILER_INTERNAL_ERROR();
      return;
  }

  emitOp(op_LEA_Gv_M);
  Ea oEa($sp, pVar->GetLocation());
  emitEa(&oEa, mapGpr(pI->GetOutput()));
}

DefProcI_(x86CvtFloat) {
  if (pI->output_type() == *Ty_Float32) {
      // float32 <= float64
      emitVsWs(op_CVTSD2SS_Vss_Wsd, pI->GetOutput(), pI->GetSx());
      return;
  }

  if (pI->output_type() == *Ty_Float64) {
      // float64 <= float32
      emitVsWs(op_CVTSS2SD_Vsd_Wss, pI->GetOutput(), pI->GetSx());
      return;
  }

  processUnexpected(pI);
}

DefProcI_(x86CvtInt)
{
  if (pI->output_type() == *Ty_Float32) {
      // float32 <= int
      emitVsEv(op_CVTSI2SS_Vss_Ed, pI->GetOutput(), pI->GetSx());
      return;
  }

  if (pI->output_type() == *Ty_Float64) {
      // float64 <= int
      emitVsEv(op_CVTSI2SD_Vsd_Ed, pI->GetOutput(), pI->GetSx());
      return;
  }

  processUnexpected(pI);
}

DefProcI_(x86Encode) {
  if (pI->output_type() == *Ty_Float32) {
      // Copy   uint32  %r1 <= u32
      // Encode float32 %f2 <= %r1
      Reg const rd = mapFpr(pI->GetOutput()->StaticCast<Physical>());
      Reg const rx = mapGpr(pI->GetSx()->StaticCast<Physical>());
      emitOp(op_MOVD_Vdq_Ed);
      emitU8(ModRm(Mod_Reg, rd, rx));
      return;
  }

  if (pI->output_type() == *Ty_Float64) {
      // Copy   uint32  %r1 <= u32
      // Encode float64 %f2 <= %r1
      Reg const rd = mapFpr(pI->GetOutput()->StaticCast<Physical>());
      Reg const rx = mapGpr(pI->GetSx()->StaticCast<Physical>());

      emitOp(op_MOVD_Vdq_Ed);
      emitU8(ModRm(Mod_Reg, rd, rx));

      emitOp(op_PSLLQ_Udq_Ib);
      emitU8(ModRm(Mod_Reg, opext_PSLLQ_Udq_Ib, rd));
      emitU8(32);
      return;
  }

  processUnexpected(pI);
}

DefProcI_(x86Lea)
{
  if (nullptr != pI->GetQd())
  {
      return;
  }

  Ea oEa = GetEa(pI);
  emitOp(op_LEA_Gv_M);
  emitEa(&oEa, mapGpr(pI->GetOutput()));
}

DefProcI_(x86Test) {
  auto const pSx = pI->GetSx();
  auto const pSy = pI->GetSy();

  if (auto const pRy = pSy->DynamicCast<Physical>()) {
      emitOp(op_TEST_Ev_Gv);
      emitEv(mapGpr(pRy), pSx);
      return;
  }

  auto const iIb = getIb(pSy);

  if ((iIb & ~0xFF) == 0) {
      if (auto const pRx = pSx->DynamicCast<Physical>()) {
          int iGpr = mapGpr(pRx) & 7;
          if (0 == iGpr) {
              emitOp(op_TEST_AL_Ib);
              emitU8(static_cast<uint8>(iIb));
              return;
          }

          if (iGpr >= 4) {
              emitOp(op_TEST_Ev_Iz);
              emitEv(opext_TEST_Ev_Iz, pSx);
              emitU32(static_cast<uint32>(iIb));
              return;
          }
      }

      emitOp(op_TEST_Eb_Ib);
      emitEv(opext_TEST_Eb_Ib, pSx);
      emitU8(static_cast<uint8>(iIb));
      return;
  }

  if (auto const pIy = pSy->DynamicCast<Immediate>()) {
      if (auto const pRx = pSx->DynamicCast<Physical>()) {
          if (mapGpr(pRx) == 0) {
              emitOp(op_TEST_eAX_Iz);
              emitIz(pIy);
              return;
          }
      }

      emitOp(op_TEST_Ev_Iz);
      emitEv(opext_TEST_Ev_Iz, pSx);
      emitIz(pIy);
      return;
  }

  COMPILER_INTERNAL_ERROR();
}

DefProcI_(x86Zero) {
  if (pI->output_type() == *Ty_Float32) {
      Reg const rd = mapFpr(pI->GetOutput());
      emitOp(op_XORPS_Vps_Wps);
      emitU8(ModRm(Mod_Reg, rd, rd));
      return;
  }

  if (pI->output_type() == *Ty_Float64) {
      Reg const rd = mapFpr(pI->GetOutput());
      emitOp(op_XORPD_Vpd_Wpd);
      emitU8(ModRm(Mod_Reg, rd, rd));
      return;
  }

  processUnexpected(pI);
}

// ctor
X86AsmTask::X86AsmTask(Session& session, Module& module)
  : Base("X86AsmTask", session, module) {}

// [S]
void X86AsmTask::Start() {
  if (!s_Thread) {
    s_Thread = new Om::Thread();
  }

  X86AsmTasklet tasklet(session(), module());
  tasklet.Run();
}

} // Cg
} // Il
