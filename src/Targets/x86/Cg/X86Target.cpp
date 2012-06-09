#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Targets - X86
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86Defs.h"

#include "../../../Om.h"
#include "../../../Om/Frame.h"

namespace Il {
namespace Cg {

static const RegDesc k_rgoRegDesc[8 * 5] = {
  { RegClass_Gpr, 0x00,   8, 0, "AL" },
  { RegClass_Gpr, 0x01,   8, 1, "CL" },
  { RegClass_Gpr, 0x02,   8, 2, "DL" },
  { RegClass_Gpr, 0x03,   8, 3, "BL" },
  { RegClass_Gpr, 0x04,   8, 4, "AH" },
  { RegClass_Gpr, 0x05,   8, 5, "CH" },
  { RegClass_Gpr, 0x06,   8, 6, "DH" },
  { RegClass_Gpr, 0x07,   8, 7, "BH" },

  { RegClass_Gpr, 0x08,  16, 0, "AX" },
  { RegClass_Gpr, 0x09,  16, 1, "CX" },
  { RegClass_Gpr, 0x0A,  16, 2, "DX" },
  { RegClass_Gpr, 0x0B,  16, 3, "BX" },
  { RegClass_Gpr, 0x0C,  16, 4, "SP" },
  { RegClass_Gpr, 0x0D,  16, 5, "BP" },
  { RegClass_Gpr, 0x0E,  16, 6, "SI" },
  { RegClass_Gpr, 0x0F,  16, 7, "DI" },

  { RegClass_Gpr, 0x10,  32, 0, "EAX" },
  { RegClass_Gpr, 0x11,  32, 1, "ECX" },
  { RegClass_Gpr, 0x12,  32, 2, "EDX" },
  { RegClass_Gpr, 0x13,  32, 3, "EBX" },
  { RegClass_Gpr, 0x14,  32, 4, "ESP" },
  { RegClass_Gpr, 0x15,  32, 5, "EBP" },
  { RegClass_Gpr, 0x16,  32, 6, "ESI" },
  { RegClass_Gpr, 0x17,  32, 7, "EDI" },

  { RegClass_Fpr, 0x18,  64, 0, "MM0" },
  { RegClass_Fpr, 0x19,  64, 1, "MM1" },
  { RegClass_Fpr, 0x1A,  64, 2, "MM2" },
  { RegClass_Fpr, 0x1B,  64, 3, "MM3" },
  { RegClass_Fpr, 0x1C,  64, 4, "MM4" },
  { RegClass_Fpr, 0x1D,  64, 5, "MM5" },
  { RegClass_Fpr, 0x1E,  64, 6, "MM6" },
  { RegClass_Fpr, 0x1F,  64, 7, "MM7" },

  { RegClass_Fpr, 0x20, 128, 0, "XMM0" },
  { RegClass_Fpr, 0x21, 128, 1, "XMM1" },
  { RegClass_Fpr, 0x22, 128, 2, "XMM2" },
  { RegClass_Fpr, 0x23, 128, 3, "XMM3" },
  { RegClass_Fpr, 0x24, 128, 4, "XMM4" },
  { RegClass_Fpr, 0x25, 128, 5, "XMM5" },
  { RegClass_Fpr, 0x26, 128, 6, "XMM6" },
  { RegClass_Fpr, 0x27, 128, 7, "XMM7" },
};

#define defregset(mp_name, mp_class, mp_n) \
  static const RegDesc* s_rgp ## mp_name [] = {

#define endregset(mp_name, mp_class, mp_n) \
  }; \
  static RegSet s_o ## mp_name = \
      { RegClass_ ## mp_class, mp_n, s_rgp ## mp_name };

#define regentry(mp_reg) \
  &k_rgoRegDesc[$ ## mp_reg],

#include "./X86Isa.inc"

static RegSet s_oFprEmpty = {
  RegClass_Fpr, 0, nullptr,
};

static RegGroup s_oFprGroup = {
  "FPR",
  RegClass_Fpr,
  8,              // width
  nullptr,           // rn
  nullptr,           // rsp
  nullptr,
  &s_oFprAll,     // all
  &s_oFprAll,     // allocable
  &s_oFprAll,     // free
  &s_oFprAll,     // args
  &s_oFprEmpty,   // callee save
  &s_oFprEmpty,   // caller save
};

static RegSet s_oGprEmpty = {
  RegClass_Gpr, 0
};

static RegGroup s_oGprGroup = {
  "GPR",
  RegClass_Gpr,
  4,                      // width
  &k_rgoRegDesc[$rn],     // $rn
  &k_rgoRegDesc[$sp],     // $rsp
  &k_rgoRegDesc[$tcb],    // $tcb
  &s_oGprAll,             // all
  &s_oGprAllocable,       // allocable
  &s_oGprAllocable,       // free
  &s_oGprArgs,            // args
  &s_oGprEmpty,           // callee save
  &s_oGprAllocable,       // caller save
};

// TODO 2011-01-08 yosi@msn.com Move Arch to another place.
class Arch {
  public: static int const MultipleValuesLimit = 128;
};

class X86Target : public CgTarget {
  private: CgOutput* m_rgpArgReg[Arch::MultipleValuesLimit];
  private: Physical* m_rgpPhysical[lengthof(k_rgoRegDesc)];

  public: X86Target()
      : CgTarget(&s_oFprGroup, &s_oGprGroup) {
    ::ZeroMemory(m_rgpPhysical, sizeof(m_rgpPhysical));
    ::ZeroMemory(m_rgpArgReg,   sizeof(m_rgpArgReg));
  }

  public: virtual ~X86Target() {
    auto const end = m_rgpPhysical + ARRAYSIZE(m_rgpPhysical);
    for (auto p = m_rgpPhysical; p < end; p++) {
      delete *p;
    }
  }

  public: virtual const Physical& physical(int name) const override {
    ASSERT(uint(name) < ARRAYSIZE(k_rgoRegDesc));
    return *const_cast<X86Target*>(this)->GetPhysical(&k_rgoRegDesc[name]);
  }

  public: virtual const Physical& thread_reg() const override {
    return physical($tcb);
  }

  // [C]
  public: virtual int ComputeFrameSize(const FrameReg& frame_reg) const override {
    auto& open_inst = *frame_reg.GetDefI();

    switch (frame_reg.GetFrameKind()) {
      case FrameReg::Kind_Catch:
        return sizeof(Om::CatchFrame)
            +  open_inst.CountOperands() * sizeof(void*);

      case FrameReg::Kind_Finally: {
        auto& args_inst = *open_inst.op1().StaticCast<Values>()->GetDefI();
        return sizeof(Om::FinallyFrame)
            +  args_inst.CountOperands() * sizeof(void*);
      }

      default:
        CAN_NOT_HAPPEN();
    }
  }

  // [G]
  public: virtual CgOutput* GetArgReg(uint nNth) override {
    auto const pArgRegs = s_oGprGroup.m_pArgs;
    if (nNth < static_cast<uint>(pArgRegs->m_c)) {
      return GetPhysical(pArgRegs->m_prgpReg[nNth]);
    }

    if (nNth >= lengthof(m_rgpArgReg)) {
      COMPILER_INTERNAL_ERROR();
      return new ThreadSlot(RegClass_Gpr, 0, sizeof(void*));
    }

    CgOutput* pMx = m_rgpArgReg[nNth];
    if (!pMx) {
      pMx = new ThreadSlot(
          RegClass_Gpr,
          offsetof(Thread, mv_value[nNth]),
          sizeof(void*));

      m_rgpArgReg[nNth] = pMx;
    }

    return pMx;
  }

  public: virtual Physical* GetPhysical(const RegDesc* pRegDesc) override {
    auto nIndex = pRegDesc->m_nId;
    Physical* pRx = m_rgpPhysical[nIndex];
    if (pRx == nullptr) {
      pRx = new X86Register(*pRegDesc);
      m_rgpPhysical[nIndex] = pRx;
    }
    return pRx;
  }

  DISALLOW_COPY_AND_ASSIGN(X86Target);
};

void LibExport CgTargetInit() {
  CgTarget::Set(*new X86Target());
}

} // Cg
} // Il
