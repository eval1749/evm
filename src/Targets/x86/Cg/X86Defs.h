// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_X86Defs_h)
#define INCLUDE_Il_Cg_X86Defs_h

#include "../../../Il.h"
#include "../../../Cg.h"
#include "./X86Arch.h"

namespace Il {
namespace Cg {

class X86Operand;
class TttnLit;

#define DEFINE_TARGET_INSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
  class mp_name ## I;

#include "./Instructions/X86Instructions.inc"

} // Cg
} // Il

#include "./X86Functor.h"
#include "./X86Operands.h"
#include "./X86Instructions.h"

namespace Il {
namespace Cg {

using namespace Il::X86;

// TODO 2011-01-08 yosi@msn.com Remove Thread class. This is
// for porting.
struct Thread {
  int m_name;
  int mv_value[128];
}; // Thread

class X86Utility {
  // [G]
  protected: static Physical* getGpr(uint eReg) {
      CgTarget* pTarget = &CgTarget::Get();
      foreach (RegSet::Enum, oEnum, pTarget->GetGprGroup()->m_pAll) {
          const RegDesc* pDesc = oEnum.Get();
          if (pDesc->m_nId == eReg) {
              return pTarget->GetPhysical(pDesc);
          }
      } // for each reg
      COMPILER_INTERNAL_ERROR();
      return nullptr;
  } // getGpr

  protected: static Physical* getTcb() {
      CgTarget* pTarget = &CgTarget::Get();
      return pTarget->GetPhysical(pTarget->GetGprGroup()->m_pRtcb);
  } // getTcb
}; // X86UtilPass

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_X86Defs_h)
