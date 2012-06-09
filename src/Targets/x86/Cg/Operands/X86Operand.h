// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_CgX86_Operands_X86Operand_h)
#define INCLUDE_evm_CgX86_Operands_X86Operand_h

namespace Il {
namespace Cg {

class X86Operand : public Operand_<X86Operand> {
  protected: X86Operand() {}
  private: virtual void Apply(Functor*) override { CAN_NOT_HAPPEN(); }
  public: virtual void Apply(X86Functor*) { CAN_NOT_HAPPEN(); }
  DISALLOW_COPY_AND_ASSIGN(X86Operand);
};

template<class TSelf, class TBase = X86Operand>
class X86Operand_ : public Operand_<TSelf, TBase> {
  public: X86Operand_() {}

  public: virtual void Apply(X86Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(X86Operand_);
};

}   // Cg
}  // Il
#endif // !defined(INCLUDE_evm_CgX86_Operands_X86Operand_h)
