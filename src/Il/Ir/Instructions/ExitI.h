// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_ExitI_h)
#define INCLUDE_Il_Ir_Instructions_ExitI_h

#include "./LastInstruction.h"

namespace Il {
namespace Ir {

class ExitI :
        public Instruction_<ExitI, Op_Exit, LastInstruction> {

    // ctor
    public: ExitI();

    // [R]
    public: virtual void Realize() override;

    DISALLOW_COPY_AND_ASSIGN(ExitI);
}; // ExitI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_ExitI_h)
