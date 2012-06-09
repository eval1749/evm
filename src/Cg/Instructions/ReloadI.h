// @(#)$Id$
//
// Evita Il - IR - Instructions - ReloadI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_ReloadI_h)
#define INCLUDE_Il_Ir_Instructions_ReloadI_h

#include "./CopyInstruction.h"

namespace Il {
namespace Cg {

// ReloadI ty %pd <= %mx
class ReloadI : public CgInstruction_<ReloadI, Op_Reload, CopyInstruction> {
  // ctor
  public: ReloadI() : Base(Ty_Void, Void) {}

  public: ReloadI(
      const Type& outy,
      const Physical& out,
      const StackSlot& src)
      : Base(outy, out) {
    AppendOperand(src);
  }

  DISALLOW_COPY_AND_ASSIGN(ReloadI);
}; // ReloadI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_ReloadI_h)
