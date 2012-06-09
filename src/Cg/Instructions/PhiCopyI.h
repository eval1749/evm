// @(#)$Id$
//
// Evita Il - IR - Instructions - PhiCopyI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_PhiCopyI_h)
#define INCLUDE_Il_Ir_Instructions_PhiCopyI_h

#include "./CopyInstruction.h"

namespace Il {
namespace Cg {

// PhiCopyI ty %rd <= %rx
class PhiCopyI
    : public CgInstruction_<PhiCopyI, Op_PhiCopy, CopyInstruction> {
  public: PhiCopyI();
  public: PhiCopyI(const Type&, const CgOutput&, const Operand&);
  DISALLOW_COPY_AND_ASSIGN(PhiCopyI);
}; // PhiCopyI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_PhiCopyI_h)
