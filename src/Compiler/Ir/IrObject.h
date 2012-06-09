// @(#)$Id$
//
// Evita Compiler - IR
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_IrObject_h)
#define INCLUDE_evc_IrObject_h

namespace Compiler {

class IrObject : public WithCastable_<IrObject, Object>, public Allocable {
  CASTABLE_CLASS(IrObject)
  protected: IrObject() {}
  DISALLOW_COPY_AND_ASSIGN(IrObject);
}; // IrObject


template<class TSelf, class TParent = IrObject>
class IrObject_ : public WithCastable_<TSelf, TParent> {
  protected: typedef IrObject_<TSelf, TParent> Base;
protected: IrObject_() {}
  DISALLOW_COPY_AND_ASSIGN(IrObject_);
}; // IrObject_

} // Compiler

#endif // !defined(INCLUDE_evc_IrObject_h)
