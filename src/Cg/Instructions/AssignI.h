// @(#)$Id$
//
// Evita Il - IR - Instructions - AssignI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_AssignI_h)
#define INCLUDE_Il_Ir_Instructions_AssignI_h

#include "./CopyInstruction.h"

namespace Il {
namespace Cg {

// AssignI
class AssignI : public CgInstruction_<AssignI, Op_Assign, CopyInstruction> {
  public: AssignI() : Base(Ty_Void, Void) {}

  public: AssignI(const Type& outy, const Register& out, const Operand& src)
      : Base(outy, out) { AppendOperand(src); }

  DISALLOW_COPY_AND_ASSIGN(AssignI);
}; // AssignI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_AssignI_h)
