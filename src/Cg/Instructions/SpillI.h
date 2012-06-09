// @(#)$Id$
//
// Evita Il - IR - Instructions - SpillI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_SpillI_h)
#define INCLUDE_Il_Ir_Instructions_SpillI_h

#include "./CopyInstruction.h"

namespace Il {
namespace Cg {

// SpillI ty %md <= %rx
class SpillI : public CgInstruction_<SpillI, Op_Spill, CopyInstruction> {
  // ctor
  public: SpillI() : Base(Ty_Void, Void) {}

  public: SpillI(const Type& outy, const StackSlot& out, const Physical& src)
      : Base(outy, out) { AppendOperand(src); }

  DISALLOW_COPY_AND_ASSIGN(SpillI);
}; // SpillI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_SpillI_h)
