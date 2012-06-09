#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ExitI.h"

#include "../CfgEdge.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
ExitI::ExitI() : Base(Ty_Void, Void) {}

// [R]
void ExitI::Realize() {
    auto const pFunction = this->GetBBlock()->GetFunction();

    pFunction->GetEntryBB()->AddEdge(this->GetBBlock())
        ->SetEdgeKind(CfgEdge::Kind_Pseudo);
} // Realize

} // Ir
} // Il
