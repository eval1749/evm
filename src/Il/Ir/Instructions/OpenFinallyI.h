// @(#)$Id$
//
// Evita Il - IR - Instructions - OpenFinallyI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_OpenFinallyI_h)
#define INCLUDE_Il_Ir_Instructions_OpenFinallyI_h

#include "./OpenInstruction.h"

namespace Il {
namespace Ir {

// OpenFinallyI ty %fd <= fn %vy
class OpenFinallyI :
    public Instruction_<OpenFinallyI, Op_OpenFinally, OpenInstruction> {

    // ctor
    public: OpenFinallyI();

    public: OpenFinallyI(
        FrameReg* const pFd,
        Function* const pFn,
        Values* const pVy);

    // [I]
    public: virtual bool IsUseless() const override;

    DISALLOW_COPY_AND_ASSIGN(OpenFinallyI);
}; // OpenFinallyI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_OpenFinallyI_h)
