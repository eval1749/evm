// @(#)$Id$
//
// Evita Il - IR - Instructions - SwapI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_SwapI_h)
#define INCLUDE_Il_Ir_Instructions_SwapI_h

#include "./CopyInstruction.h"

namespace Il {
namespace Cg {


/// <summary>
///   SWAP instruction. Register allocator pass intorduces this instruction
///   for breaking cyclin during serializing parallel copy.
///   <para>
///     SWAP %pd &lt;= %px
///   </para>
/// </summary>
class SwapI : public CgInstruction_<SwapI, Op_Swap> {
  // ctor
  public: SwapI() : Base(Ty_Void, Void) {}

  public: SwapI(
      const Type& outy,
      const Physical& r0,
      const Physical& r1)
      : Base(outy, r0) {
    AppendOperand(r1);
  }

  DISALLOW_COPY_AND_ASSIGN(SwapI);
}; // SwapI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_SwapI_h)
