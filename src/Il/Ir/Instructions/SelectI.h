// @(#)$Id$
//
// Evita Il - IR - Instructions - SelectI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_SelectI_h)
#define INCLUDE_Il_Ir_Instructions_SelectI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   <example>
///     SELECT ty %rd &lt;= %vx iy
///   </example>
/// </summary>
class SelectI : public Instruction_<SelectI, Op_Select> {
  public: SelectI();
  public: SelectI(const Type&, const Register&, const Values&, int);
  DISALLOW_COPY_AND_ASSIGN(SelectI);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_SelectI_h)
