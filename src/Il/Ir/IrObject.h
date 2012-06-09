// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_IrObject_h)
#define INCLUDE_Il_Ir_IrObject_h

#include "../Functor.h"
#include "./WithSourceInfo.h"

namespace Il {
namespace Ir {

class IrObject
    : public Allocable,
      public WithCastable_<IrObject, Object> {
  CASTABLE_CLASS(IrObject);

  protected: IrObject() {}
  public: virtual ~IrObject() {}

  // [A]
  public: virtual void Apply(Functor* const functor);

  DISALLOW_COPY_AND_ASSIGN(IrObject);
};

template<class TSelf, class TParent = IrObject>
class IrObject_ : public WithCastable_<TSelf, TParent> {

  protected: typedef IrObject_<TSelf, TParent> Base;

  protected: IrObject_() {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(IrObject_);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_IrObject_h)
