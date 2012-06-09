// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_VoidOutput_h)
#define INCLUDE_Il_Ir_VoidOutput_h

#include "./Output.h"

namespace Il {
namespace Ir {

/// <summary>
///   Void output. This is singletone class. You can use this via static
///   variable "Void".
/// </summary>
class VoidOutput : public Operand_<VoidOutput, Output> {
  CASTABLE_CLASS(VoidOutput)

  // ctor
  public: VoidOutput();

  // [G]
  public: virtual const Type& GetTy() const override;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(VoidOutput);
}; // VoidOutput

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_VoidOutput_h)
