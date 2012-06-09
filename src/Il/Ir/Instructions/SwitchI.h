// @(#)$Id$
//
// Evita Il - IR - Instructions - SwitchI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_SwitchI_h)
#define INCLUDE_Il_Ir_Instructions_SwitchI_h

#include "./LastInstruction.h"

namespace Il {
namespace Ir {

class SwitchI :
    public Instruction_<SwitchI, Op_Switch, LastInstruction> {

    public: class EnumCase : public EnumOperand {
        private: typedef EnumOperand Base;
        public: EnumCase(const SwitchI* const p);
        public: SwitchOperandBox* Get() const;
    }; // EnumCase

    // ctor
    // ctor
    public: SwitchI();

    public: SwitchI(
        Operand* const pSx,
        Label* const pLabel);

    // [A]
    public: void AddOperand(
        BBlock* const pBB,
        Operand* const pLx);

    // [F]
    public: SwitchOperandBox* FindOperand(Operand* const pSx) const;

    // [G]
    public: BBlock* GetDefaultBB() const;

    // [R]
    public: virtual void Realize() override;

    // [U]
    public: virtual void Unrealize() override;

    DISALLOW_COPY_AND_ASSIGN(SwitchI);
}; // SwitchI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_SwitchI_h)
