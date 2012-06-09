// @(#)$Id$
//
// Evita Il - IR - Instructions - CopyI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_CopyI_h)
#define INCLUDE_Il_Ir_Instructions_CopyI_h

#include "./CopyInstruction.h"

namespace Il {
namespace Cg {

// CopyI ty %rd <= %rx
// CopyI
class CopyI : public CgInstruction_<CopyI, Op_Copy, CopyInstruction> {
  public: CopyI() : Base(Ty_Void, Void) {}

  public: CopyI(const Type& outy, const Output& out, const Output& src)
      : Base(outy, out) {
    AppendOperand(src);
  }

  DISALLOW_COPY_AND_ASSIGN(CopyI);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_CopyI_h)
