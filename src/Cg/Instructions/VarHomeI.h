// @(#)$Id$
//
// Evita Il - IR - Instructions - VarHomeI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_VarHomeI_h)
#define INCLUDE_Il_Ir_Instructions_VarHomeI_h

namespace Il {
namespace Cg {

// VarHomeI ty %md <= %qx %ry
class VarHomeI : public CgInstruction_<VarHomeI, Op_VarHome> {
  // ctor
  public: VarHomeI() : Base(Ty_Void, Void) {}

  public: VarHomeI(
      const Type& outy,
      const Output& out,
      const Output& src0,
      const Operand& src1)
      : Base(outy, out) {
    AppendOperand(src0);
    AppendOperand(src1);
  }

  DISALLOW_COPY_AND_ASSIGN(VarHomeI);
}; // VarHomeI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_VarHomeI_h)
