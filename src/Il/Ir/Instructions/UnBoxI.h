// @(#)$Id$
//
// Evita Il - IR - Instructions - UnBoxI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_UnBoxI_h)
#define INCLUDE_Il_Ir_Instructions_UnBoxI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   Pseudo instruction BOX.
///   <example>
///     UNBOX ty %rd &lt;= %rx
///   </example>
/// </summary>
class UnBoxI : public Instruction_<UnBoxI, Op_UnBox> {
  // ctor
  public: UnBoxI();

  public: UnBoxI(
      Type* const pty,
      Output* const pRd,
      Register* const pRx);

    DISALLOW_COPY_AND_ASSIGN(UnBoxI);
}; // UnBoxI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_UnBoxI_h)
