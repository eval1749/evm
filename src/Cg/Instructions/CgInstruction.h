// @(#)$Id$
//
// Evita Il - IR - Instructions - CgInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_Instructions_CgInstruction_h)
#define INCLUDE_Il_Cg_Instructions_CgInstruction_h

#include "../../Il/Ir/Instructions/Instruction.h"

namespace Il {
namespace Cg {

class CgInstruction : public InstructionFamily_<CgInstruction> {
  CASTABLE_CLASS(CgInstruction)

  // ctor
  protected: CgInstruction(Op opcode, const Type* outy, const Output* out)
    : Base(opcode, outy, out) {}

  protected: CgInstruction(Op opcode, const Type& outy, const Output& out)
    : Base(opcode, outy, out) {}

  private: virtual void Apply(Functor*) override { CAN_NOT_HAPPEN(); }

  public: virtual void Apply(CgFunctor* functor) {
    functor->Process(this);
  }

  DISALLOW_COPY_AND_ASSIGN(CgInstruction);
};

template<class TSelf, Op tOpcode, class TBase = CgInstruction>
class CgInstruction_ : public Instruction_<TSelf, tOpcode, TBase> {
  public: typedef CgInstruction_<TSelf, tOpcode, TBase> Base;

  protected: CgInstruction_(
      const Type* const pType,
      const Output* const pOutput)
      : Instruction_(pType, pOutput) {}

  protected: CgInstruction_(
      const Type& outy,
      const Output& out)
      : Instruction_(outy, out) {}

  // [A]
  public: virtual void Apply(CgFunctor* const functor) {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(CgInstruction_);
};

template<class TSelf, class TBase = CgInstruction>
class CgInstructionFamily_
    : public WithCastable3_<TSelf, TBase, Op, const Type*, const Output*> {

  public: typedef CgInstructionFamily_<TSelf, TBase> Base;

  protected: CgInstructionFamily_(
      Op const opcode,
      const Type* const outy,
      const Output* const out)
      : WithCastable3_(opcode, outy, out) {}

  protected: CgInstructionFamily_(
      Op const opcode,
      const Type& outy,
      const Output& out)
      : WithCastable3_(opcode, &outy, &out) {}

  public: virtual void Apply(CgFunctor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(CgInstructionFamily_);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_Instructions_CgInstruction_h)
