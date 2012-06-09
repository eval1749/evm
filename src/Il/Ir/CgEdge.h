// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_CgEdge_h)
#define INCLUDE_Il_Ir_CgEdge_h

#include "./Edge.h"
#include "./IrObject.h"

namespace Il {
namespace Ir {

class Function;

class CgEdge :
        public LocalObject,
        public WithCastable_<CgEdge, IrObject>,
        public Edge_<CgEdge, Function> {

    CASTABLE_CLASS(CgEdge)

    private: typedef Edge_<CgEdge, Function> Edge;

    public: uint m_cUsers;

    public: CgEdge(
        Function*   pIn,
        Function*   pOut,
        Kind        eKind = Kind_Normal) :
            m_cUsers(0),
            Edge(pIn, pOut, eKind) {}

    DISALLOW_COPY_AND_ASSIGN(CgEdge);
}; // CgEdge

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_CgEdge_h)
