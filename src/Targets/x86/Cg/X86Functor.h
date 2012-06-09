// @(#)$Id$
//
// Evita Il
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_CgX86_X86Functor_h)
#define INCLUDE_evm_CgX86_X86Functor_h

#include "../../../Cg/CgFunctor.h"

#if !defined(NEED_CG_INSTRUCTIONS)
  #error "You must define NEED_CG_INSTRUCTIONS"
#endif

#if !defined(TARGET_X86)
  #error "You msut define TARGET_X86."
#endif

namespace Il {
namespace Cg {

using namespace Il::Ir;
class X86Instruction;

// X86Functor provides double-dispatching on derived class and IR objects.
class X86Functor : public CgFunctor {
  protected: X86Functor() {}

  public: virtual void Process(X86Instruction*);
  protected: virtual void Process(CgInstruction*) override;
  protected: virtual void Process(Instruction*) override;
  protected: virtual void Process(Operand*) override;

  #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format)

  #define DEFINE_TARGET_INSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    public: virtual void Process(mp_name ##I*);

  #define FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
    public: virtual void Process(mp_Type*) override;

  #include "../../../Il/FunctorMethods.inc"

  #define CG_FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
    public: virtual void Process(mp_Type*) override;

  #include "../../../Cg/CgFunctorMethods.inc"

  #define X86_FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
    public: virtual void Process(mp_Type*);

  #include "./X86FunctorMethods.inc"

  DISALLOW_COPY_AND_ASSIGN(X86Functor);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_evm_CgX86_X86Functor_h)
