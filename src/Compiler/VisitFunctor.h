// @(#)$Id$
//
// Evita Compiler - Functor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_VisitFunctor_h)
#define INCLUDE_Compiler_VisitFunctor_h

#include "./Functor.h"

namespace Compiler {

class VisitFunctor : public Functor {
  #define FUNCTOR_METHOD_(mp_name) \
    public: virtual void Process(mp_name* const) override;

  #include "./FunctorMethods.inc"

  protected: VisitFunctor() {}

  DISALLOW_COPY_AND_ASSIGN(VisitFunctor);
}; // VisitFunctor

} // Compiler

#endif // !defined(INCLUDE_Compiler_VisitFunctor_h)
