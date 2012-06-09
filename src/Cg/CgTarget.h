// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Cg_CgTarget_h)
#define INCLUDE_evm_Cg_CgTarget_h

#include "../Il.h"

namespace Il {
namespace Cg {

class CgTarget {
  private: const RegGroup* m_pFprGroup;
  private: const RegGroup* m_pGprGroup;

  // ctor
  protected: CgTarget(const RegGroup* pFprGroup, const RegGroup* pGprGroup)
    : m_pFprGroup(pFprGroup),
      m_pGprGroup(pGprGroup) {}

  // properties
  public: virtual const Physical& physical(int) const = 0;
  public: virtual const Physical& thread_reg() const = 0;

  // [C]
  public: virtual int ComputeFrameSize(const FrameReg&) const = 0;

  // [G]
  public: static CgTarget& LibExport Get();
  public: const RegGroup* GetFprGroup() const { return m_pFprGroup; }
  public: const RegGroup* GetGprGroup() const { return m_pGprGroup; }
  public: virtual CgOutput* GetArgReg(uint) = 0;
  public: virtual Physical* GetPhysical(const RegDesc*) = 0;

  // [S]
  public: static void LibExport Set(CgTarget&);

  DISALLOW_COPY_AND_ASSIGN(CgTarget);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_evm_Cg_CgTarget_h)
