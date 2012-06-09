// @(#)$Id$
//
// Evita Il
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_X86_Functor_h)
#define INCLUDE_Il_Targets_X86_Functor_h

#include "./X86Defs.h"

#include "../../Cg/CgFunctor_.h"

namespace Il {
namespace Cg {

#define DEFINE_FUNCTOR_METHOD_(mp_type) \
    public: virtual bool Process(mp_type* const p ## mp_type) \
        { ASSERT(nullptr != p ## mp_type); return true; };

#define DEFINE_FUNCTOR_METHODS \
    DEFINE_FUNCTOR_METHOD_(TttnLit)

template<class TFunctor>
class X86Functor_ : public TFunctor {
    DEFINE_FUNCTOR_METHODS
}; // X86Functor_

template<typename TFunctor, typename P1>
class X86Functor1_ : public TFunctor {
    protected: typedef X86Functor1_<TFunctor, P1> Base;

    protected: X86Functor1_(P1 p1) :
        TFunctor(p1) {}

    DEFINE_FUNCTOR_METHODS
}; // X86Functor1_

#undef DEFINE_FUNCTOR_METHODS
#undef DEFINE_FUNCTOR_METHOD_

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_X86_Functor_h)
