// @(#)$Id$
//
// Evita Il - IR - Instructions - UpVarRefI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_UpVarRefI_h)
#define INCLUDE_Il_Ir_Instructions_UpVarRefI_h

namespace Il {
namespace Cg {

/// <summary>
///     UPVARREF instruction. Reference of up level variable.
///   <para>
///     UPVARREF ty %rd &lt;= %rx %qy
///   </para>
/// </summary>
class UpVarRefI : public CgInstruction_<UpVarRefI, Op_UpVarRef> {
  // ctor
  public: UpVarRefI() : Base(Ty_Void, Void) {}

  public: UpVarRefI(
      const PointerType& outy,
      const Register& out,
      const Output& src0,
      const PseudoOutput& src1)
      : Base(outy, out) {
    AppendOperand(src0);
    AppendOperand(src1);
  }

  DISALLOW_COPY_AND_ASSIGN(UpVarRefI);
}; // UpVarRefI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_UpVarRefI_h)
