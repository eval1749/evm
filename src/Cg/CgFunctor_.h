// @(#)$Id$
//
// Evita Il
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_Functor_h)
#define INCLUDE_Il_Cg_Functor_h

#include "./CgDefs.h"

namespace Il {
namespace Cg {

template<class Base_ = Il::Ir::Functor>
class CgFunctor_ : public Base_ {
    #define DEFINE_FUNCTOR_METHOD_(mp_type) \
        public: virtual bool Process(mp_type* const p ## mp_type) \
            { ASSERT(nullptr != p ## mp_type); return true; };

    ////////////////////////////////////////////////////////////
    //
    // Operands
    //
    DEFINE_FUNCTOR_METHOD_(CgOutput)
    DEFINE_FUNCTOR_METHOD_(ClosedMarker)
    DEFINE_FUNCTOR_METHOD_(FunLit)
    DEFINE_FUNCTOR_METHOD_(MemSlot)
    DEFINE_FUNCTOR_METHOD_(Physical)
    DEFINE_FUNCTOR_METHOD_(StackSlot)
    DEFINE_FUNCTOR_METHOD_(ThreadSlot)
    DEFINE_FUNCTOR_METHOD_(VarHome)

    #undef DEFINE_FUNCTOR_METHOD_
}; // CgFunctor

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_Functor_h)
