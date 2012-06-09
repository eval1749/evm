// @(#)$Id$
//
// Evita Compiler - Functor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Functor_h)
#define INCLUDE_Compiler_Functor_h

#include "./IrDefs.h"

namespace Compiler {

class Functor {
  #define FUNCTOR_METHOD_(mp_name) \
    public: virtual void Process(mp_name* const) {}

  #include "./FunctorMethods.inc"

  protected: Functor();
  public: virtual ~Functor();

  DISALLOW_COPY_AND_ASSIGN(Functor);
}; // Functor

} // Compiler

#endif // !defined(INCLUDE_Compiler_Functor_h)
