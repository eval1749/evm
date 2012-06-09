// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_UpVarBaseI_h)
#define INCLUDE_Il_Ir_Instructions_UpVarBaseI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class UpVarBaseI :
        public Instruction_<UpVarBaseI, Op_UpVarBase> {

    // ctor
    public: UpVarBaseI();
    public: UpVarBaseI(Register* const pRd, Function* const pOwner);

    DISALLOW_COPY_AND_ASSIGN(UpVarBaseI);
}; // UpVarBaseI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_UpVarBaseI_h)
