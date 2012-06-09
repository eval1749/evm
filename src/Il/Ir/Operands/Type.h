// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Type_h)
#define INCLUDE_Il_Ir_Type_h

#include "./Operand.h"

#include "../../../Common/Collections.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

enum Subtype {
  Subtype_No,
  Subtype_Yes,
  Subtype_Unknown,
};

class Type 
    : public Operand_<Type, Operand>, 
      public RefCounted_<Type> {
  CASTABLE_CLASS(Type);

  public: typedef Array_<const Type*> Types;
  public: typedef Array_<const Type*> Array;
  public: typedef ArrayList_<const Type*> List;
  public: typedef Collection_<const Type*> Collection;
  public: typedef Queue_<const Type*> Queue;
  public: typedef HashSet_<const Type*> Set;
  public: typedef Stack_<const Type*> Stack;

  protected: Type();
  public: virtual ~Type();

  // properties
  public: virtual int bit_width() const { return 0; }

  // [A]
  public: static const Type& And(const Type&, const Type&);

  // [C]
  public: virtual const Type& ComputeBoundType() const { return *this; }
  public: virtual int ComputeHashCode() const = 0;
  public: virtual const Type& Construct(const TypeArgs&) const = 0;

  // [G]
  public: virtual RegClass GetRegClass() const { return RegClass_Gpr; }

  // [H]
  public: virtual bool HasSign() const { return false; }

  // [I]
  public: virtual bool IsBound() const { return true; }
  public: bool IsFloat() const;
  public: bool IsInt() const;
  public: virtual Subtype IsSubtypeOf(const Type&) const = 0;
  public: bool IsUInt() const;

  // [O]
  public: static const Type& Or(const Type&, const Type&);

  DISALLOW_COPY_AND_ASSIGN(Type);
}; // Type

inline int ComputeHashCode(const Type* const p) {
  ASSERT(p != nullptr);
  return p->ComputeHashCode();
}

template<class TSelf, class TParent = Type>
class Type_ : public WithCastable_<TSelf, TParent> {

    protected: typedef Type_<TSelf, TParent> Base;

    // ctor
    protected: Type_() {}

    public: virtual void Apply(Functor* const pFunctor) override {
        pFunctor->Process(static_cast<TSelf*>(this));
    } // Apply

  DISALLOW_COPY_AND_ASSIGN(Type_);
}; // Type_

template<class TSelf, class TParent, typename TParam1>
class Type2_ : public WithCastable1_<TSelf, TParent, TParam1> {

  protected: typedef Type2_<TSelf, TParent, TParam1> Base;

  // ctor
  protected: Type2_(TParam1 param) : WithCastable1_(param) {}

  public: virtual void Apply(Functor* const pFunctor) override {
    pFunctor->Process(static_cast<TSelf*>(this));
  } // Apply

  DISALLOW_COPY_AND_ASSIGN(Type2_);
}; // Type2_

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Type_h)
