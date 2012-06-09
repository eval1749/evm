// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_SwitchOperandBox_h)
#define INCLUDE_Il_Ir_SwitchOperandBox_h

#include "./OperandBox.h"

namespace Il {
namespace Ir {

class SwitchOperandBox :
        public IrObject_<SwitchOperandBox, OperandBox> {

    CASTABLE_CLASS(SwitchOperandBox)

    private: Label* m_pLabel;

    // ctor
    public: SwitchOperandBox(BBlock* const pBB, Operand* const pLx);

    // [G]
    public: BBlock* GetBB() const;
    public: Label* GetLabel() const { return m_pLabel; }

    // [S]
    public: void SetBB(BBlock* pBBlock);

    DISALLOW_COPY_AND_ASSIGN(SwitchOperandBox);
}; // SwitchOperandBox

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_SwitchOperandBox_h)
