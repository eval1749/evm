// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_CgX86_X86Register_h)
#define INCLUDE_evm_CgX86_X86Register_h

#include "./X86Operand.h"

namespace Il {
namespace Cg {

class X86Register
    : public CgOperand1_<X86Register, Physical, const RegDesc&> {
  CASTABLE_CLASS(X86Register);

  // ctor
  public: X86Register(const RegDesc&);

  // [A]
  private: virtual void Apply(CgFunctor*) override { CAN_NOT_HAPPEN(); }

  public: virtual void Apply(X86Functor* const functor) {
    functor->Process(this);
  }

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(X86Register);
}; // X86Register

}  // Cg
}  // Il

#endif // !defined(INCLUDE_evm_CgX86_X86Register_h)
