// @(#)$Id$
//
// Evita Il - IR - Operand
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Operand_h)
#define INCLUDE_Il_Ir_Operand_h

#include "../../Functor.h"
#include "../IrObject.h"

namespace Il {
namespace Ir {

// Class Operand is a abstract base class for all objects appeared in IR.
class Operand
    : public IrObject_<Operand, IrObject>,
      public LocalObject {
  CASTABLE_CLASS(Operand);

  public: typedef DoubleLinkedList_<OperandBox, Operand> Users;

  // ctor
  protected: Operand();

  // operator
  public: bool operator==(const Operand& r) const { return Equals(r); }
  public: bool operator!=(const Operand& r) const { return !operator==(r); }

  // properties
  public: virtual Instruction* def_inst() const { return nullptr; }
  public: const Type& type() const { return GetTy(); }

  // [C]
  public: virtual int ComputeHashCode() const {
    return static_cast<int>(reinterpret_cast<intptr_t>(this));
  }

  // [E]
  public: virtual bool Equals(const Operand& another) const {
    return this == &another;
  }

  // [G]
  public: virtual const Type& GetTy() const { CAN_NOT_HAPPEN(); }

  // [I]
  public: virtual bool IsFalse() const { return false; }
  public: virtual bool IsTrue()  const { return true; }

  // [R]
  public: virtual void Realize(OperandBox*) {}

  // [U]
  public: virtual void Unrealize(OperandBox*) {}

  DISALLOW_COPY_AND_ASSIGN(Operand);
};

template<class TSelf, class TBase = Operand>
class Operand_ : public WithCastable_<TSelf, TBase> {

  protected: typedef Operand_<TSelf, TBase> Base;

  protected: Operand_() {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(Operand_);
};

template<class TSelf, class TBase, typename T1>
class Operand1_ : public WithCastable1_<TSelf, TBase, T1> {

  protected: typedef Operand1_<TSelf, TBase, T1> Base;

  protected: Operand1_(T1 param)
      : WithCastable1_(param) {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(Operand1_);
};

template<class TSelf, class TBase, typename T1, typename T2>
class Operand2_ : public WithCastable2_<TSelf, TBase, T1, T2> {
  protected: Operand2_(T1 p1, T2 p2) : WithCastable2_(p1, p2) {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(Operand2_);
};

template<class TSelf, class TBase, typename T1, typename T2, typename T3>
class Operand3_ : public WithCastable3_<TSelf, TBase, T1, T2, T3> {
  protected: Operand3_(T1 p1, T2 p2, T3 p3)
      : WithCastable3_(p1, p2, p3) {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(Operand3_);
};

inline int ComputeHashCode(const Operand* const p) {
  ASSERT(p != nullptr);
  return p->ComputeHashCode();
}

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Operand_h)
