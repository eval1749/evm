// @(#)$Id$
//
// Evita Il - IR - Instructions - OpenCatchI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_OpenCatchI_h)
#define INCLUDE_Il_Ir_Instructions_OpenCatchI_h

#include "./OpenExitPointInstruction.h"

namespace Il {
namespace Ir {

// OpenCatchI ty %fd <=
class OpenCatchI :
    public Instruction_<OpenCatchI, Op_OpenCatch, OpenExitPointInstruction> {
    // ctor
    public: OpenCatchI();
    public: OpenCatchI(FrameReg* const pFd);

    // [I]
    public: virtual bool IsUseless() const override;

    DISALLOW_COPY_AND_ASSIGN(OpenCatchI);
}; // OpenCatchI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_OpenCatchI_h)
