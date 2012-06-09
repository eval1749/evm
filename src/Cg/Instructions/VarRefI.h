// @(#)$Id$
//
// Evita Il - IR - Instructions - VarRefI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_VarRefI_h)
#define INCLUDE_Il_Ir_Instructions_VarRefI_h

namespace Il {
namespace Cg {


/// <summary>
///     VARREF instruction. Generates reference of owned variable. X86 Ensure
///     pass introduces this instruction for passing reference of owned
///     variable to inner function.
///   <para>
///     VARREF ty %rd &lt;= %qx
///   </para>
/// </summary>
class VarRefI : public CgInstruction_<VarRefI, Op_VarRef> {
  // ctor
  public: VarRefI() : Base(Ty_Void, Void) {}

  public: VarRefI(
      const PointerType& outy,
      const Register& out,
      const PseudoOutput& src)
      : Base(outy, out) {
      AppendOperand(src);
  }

  DISALLOW_COPY_AND_ASSIGN(VarRefI);
}; // VarRefI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_VarRefI_h)
