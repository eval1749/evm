// @(#)$Id$
//
// Evita Il
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Functor_h)
#define INCLUDE_Il_Ir_Functor_h

namespace Il {
namespace Ir {

class Functor {
  protected: Functor() {}

  public: virtual void Process(IrObject*);
  public: virtual void Process(Instruction*);
  public: virtual void Process(Operand*);

  #define FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
    public: virtual void Process(mp_Type*);

  #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format)

  #include "./FunctorMethods.inc"

  DISALLOW_COPY_AND_ASSIGN(Functor);
}; // Functor

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Functor_h)
