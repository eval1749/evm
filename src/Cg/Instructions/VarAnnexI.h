// @(#)$Id$
//
// Evita Il - IR - Instructions - VarAnnexI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_VarAnnexI_h)
#define INCLUDE_Il_Ir_Instructions_VarAnnexI_h

namespace Il {
namespace Cg {

// VarAnnexI ty %md <= %sx %qy
class VarAnnexI : public CgInstruction_<VarAnnexI, Op_VarAnnex> {
  // ctor
  public: VarAnnexI() : Base(Ty_Void, Void) {}

  public: VarAnnexI(const StackSlot& out, const ClosedMarker& marker)
      : Base(*Ty_Object, out) {
    AppendOperand(marker);
  }

  DISALLOW_COPY_AND_ASSIGN(VarAnnexI);
}; // VarAnnexI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_VarAnnexI_h)
