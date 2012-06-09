// @(#)$Id$
//
// Evita Il - IR - Instructions - UpVarDefI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_UpVarDefI_h)
#define INCLUDE_Il_Ir_Instructions_UpVarDefI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class UpVarDefI :
    public Instruction_<UpVarDefI, Op_UpVarDef> {

    // ctor
    public: UpVarDefI();

    public: UpVarDefI(
        const Type&,
        PseudoOutput&,
        const Variable&);

    // [I]
    public: virtual bool IsUseless() const override;

    DISALLOW_COPY_AND_ASSIGN(UpVarDefI);
}; // UpVarDefI


} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_UpVarDefI_h)
