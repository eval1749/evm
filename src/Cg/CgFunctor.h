// @(#)$Id$
//
// Evita Il
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Cg_CgFunctor_h)
#define INCLUDE_evm_Cg_CgFunctor_h

#if !defined(NEED_CG_INSTRUCTIONS)
  #error "You must define NEED_CG_INSTRUCTIONS"
#endif

namespace Il {
namespace Cg {

using namespace Il::Ir;

class CgFunctor : public Functor{
  protected: CgFunctor() {}

  public: virtual void Process(CgInstruction*);
  protected: virtual void Process(Instruction*) override;
  protected: virtual void Process(Operand*) override;

  #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    public: virtual void Process(mp_name ##I*);

  #define FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
    public: virtual void Process(mp_Type*) override;

  #include "../Il/FunctorMethods.inc"

  #define CG_FUNCTOR_METHOD_2_(mp_Type, mp_Base) \
    public: virtual void Process(mp_Type*);

  #include "./CgFunctorMethods.inc"

  DISALLOW_COPY_AND_ASSIGN(CgFunctor);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_evm_Cg_CgFunctor_h)
