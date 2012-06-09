// @(#)$Id$
//
// Evita Il - IR - Instructions - EntryI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_EntryI_h)
#define INCLUDE_Il_Ir_Instructions_EntryI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class EntryI :
    public Instruction_<EntryI, Op_Entry> {

    // ctor
    public: EntryI();

    DISALLOW_COPY_AND_ASSIGN(EntryI);
}; // EntryI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_EntryI_h)
