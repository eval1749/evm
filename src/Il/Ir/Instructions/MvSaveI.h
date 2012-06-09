// @(#)$Id$
//
// Evita Il - IR - Instructions - MvSaveI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_MvSaveI_h)
#define INCLUDE_Il_Ir_Instructions_MvSaveI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// MvSaveI ty %rd <= %vx
class MvSaveI :
    public Instruction_<MvSaveI, Op_MvSave> {

    // ctor
    public: MvSaveI();
    public: MvSaveI(Register* pRd, Values* pVx);
    public: MvSaveI(PseudoOutput* pRd, Values* pVx);

    DISALLOW_COPY_AND_ASSIGN(MvSaveI);
}; // MvSaveI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_MvSaveI_h)
