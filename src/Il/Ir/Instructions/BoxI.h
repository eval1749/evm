// @(#)$Id$
//
// Evita Il - IR - Instructions - BoxI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_BoxI_h)
#define INCLUDE_Il_Ir_Instructions_BoxI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   Pseudo instruction BOX.
///   <example>
///     BOX ty %rd &lt;= %sx
///   </example>
/// </summary>
class BoxI : public Instruction_<BoxI, Op_Box> {
  public: BoxI();

  public: BoxI(
      const Type&,
      const Output&,
      const Operand&);

  public: BoxI(
      const Type*,
      const Output*,
      const Operand*);

  DISALLOW_COPY_AND_ASSIGN(BoxI);
}; // BoxI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_BoxI_h)
