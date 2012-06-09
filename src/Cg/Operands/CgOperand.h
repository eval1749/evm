// @(#)$Id$
//
// Evita Il - Cg - Operands
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_Operands_CgOperand_h)
#define INCLUDE_Il_Cg_Operands_CgOperand_h

#include "../CgFunctor.h"

namespace Il {
namespace Cg {

/// <summary>
///   Represents base class for output operand used during CG.
/// </summary>
class CgImmediate : public Operand_<CgImmediate, Immediate> {
  CASTABLE_CLASS(CgImmediate)
  protected: CgImmediate() {}
  private: virtual void Apply(Functor*) override { CAN_NOT_HAPPEN(); }
  public: virtual void Apply(CgFunctor*) { CAN_NOT_HAPPEN(); }
  DISALLOW_COPY_AND_ASSIGN(CgImmediate);
}; // CgImmediate

/// <summary>
///   Represents base class for output operand used during CG.
/// </summary>
class CgOutput : public Operand_<CgOutput, Output> {
  CASTABLE_CLASS(CgOutput)
  protected: CgOutput() {}
  private: virtual void Apply(Functor*) override { CAN_NOT_HAPPEN(); }
  public: virtual void Apply(CgFunctor*) { CAN_NOT_HAPPEN(); }
  DISALLOW_COPY_AND_ASSIGN(CgOutput);
}; // CgOutput

/// <summary>
///   Represents base class for output operand used during CG.
/// </summary>
class CgPseudoOutput : public Operand_<CgPseudoOutput, PseudoOutput> {
  CASTABLE_CLASS(CgPseudoOutput)
  protected: CgPseudoOutput() {}
  private: virtual void Apply(Functor*) override { CAN_NOT_HAPPEN(); }
  public: virtual void Apply(CgFunctor*) { CAN_NOT_HAPPEN(); }
  DISALLOW_COPY_AND_ASSIGN(CgPseudoOutput);
}; // CgPseudoOutput

template<class TSelf, class TBase>
class CgOperand_ : public Operand_<TSelf, TBase> {
  protected: typedef CgOperand_<TSelf, TBase> Base;

  public: CgOperand_() {}

  public: virtual void Apply(CgFunctor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(CgOperand_);
};

template<class TSelf, class TBase, typename T1>
class CgOperand1_ : public Operand1_<TSelf, TBase, T1> {
  public: CgOperand1_(T1 p1) : Operand1_(p1) {}

  public: virtual void Apply(CgFunctor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(CgOperand1_);
};

template<class TSelf, class TBase, typename T1, typename T2>
class CgOperand2_ : public Operand2_<TSelf, TBase, T1, T2> {
  public: CgOperand2_(T1 p1, T2 p2) : Operand2_(p1, p2) {}

  public: virtual void Apply(CgFunctor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(CgOperand2_);
};

template<class TSelf, class TBase, typename T1, typename T2, typename T3>
class CgOperand3_ : public Operand3_<TSelf, TBase, T1, T2, T3> {
  public: CgOperand3_(T1 p1, T2 p2, T3 p3) : Operand3_(p1, p2, p3) {}

  public: virtual void Apply(CgFunctor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(CgOperand3_);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_Operands_CgOperand_h)
