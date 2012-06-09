// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_IrBoolOutput_h)
#define INCLUDE_Il_IrBoolOutput_h

#include "./SsaOutput.h"

namespace Il {
namespace Ir {

/// <summary>
///   Internal representation BoolOutputean output. There are two global
//    static variable "False" and "True".
/// </summary>
class BoolOutput : public Operand_<BoolOutput, SsaOutput> {
  CASTABLE_CLASS(BoolOutput)

  // ctor
  public: BoolOutput();

  // [G]
  public: virtual char16 GetPrefixChar() const override { return 'b'; }
  public: virtual const Type& GetTy() const override;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(BoolOutput);
}; // BoolOutput

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_IrBoolOutput_h)
