// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_CgX86_TttnLit_h)
#define INCLUDE_evm_CgX86_TttnLit_h

#include "./X86Operand.h"

namespace Il {
namespace Cg {

/// <summary>
///   Represents x86 TTTN.
/// </summary>
class TttnLit : public X86Operand_<TttnLit> {
  CASTABLE_CLASS(TtnLit);

  private: X86::Tttn const value_;

  // ctor
  public: TttnLit(X86::Tttn value) : value_(value) {}

  // properties
  public: X86::Tttn value() const { return value_; }

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(TttnLit);
}; // TttnLit

}  // Cg
}  // Il

#endif // !defined(INCLUDE_evm_CgX86_TttnLit_h)
