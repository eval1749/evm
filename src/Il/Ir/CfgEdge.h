// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_CfgEdge_h)
#define INCLUDE_Il_Ir_CfgEdge_h

#include "./Edge.h"
#include "./IrObject.h"

namespace Il {
namespace Ir {

class CfgEdge :
    public Edge_<CfgEdge, BBlock>,
    public WithCastable_<CfgEdge, IrObject>,
    public WorkListItem_<CfgEdge> {

    CASTABLE_CLASS(CfgEdge)

    private: typedef Edge_<CfgEdge, BBlock> Edge;

    // ctor
    public: CfgEdge(
        BBlock* pFrom = nullptr,
        BBlock* pTo   = nullptr,
        Kind    eKind = Kind_Normal) :
            Edge(pFrom, pTo, eKind) {}

    DISALLOW_COPY_AND_ASSIGN(CfgEdge);
}; // CfgEdge

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_CfgEdge_h)
