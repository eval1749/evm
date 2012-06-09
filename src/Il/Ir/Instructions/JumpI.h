// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_JumpI_h)
#define INCLUDE_Il_Ir_Instructions_JumpI_h

#include "./LastInstruction.h"

namespace Il {
namespace Ir {

class JumpI : public Instruction_<JumpI, Op_Jump, LastInstruction> {
    // ctor
    public: JumpI();
    public: JumpI(const BBlock&);
    public: JumpI(const BBlock*);

    // [G]
    public: Label* GetLabel() const;
    public: BBlock* GetTargetBB() const;

    // [R]
    public: virtual void Realize() override;

    // [U]
    public: virtual void Unrealize() override;

    DISALLOW_COPY_AND_ASSIGN(JumpI);
}; // JumpI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_JumpI_h)
