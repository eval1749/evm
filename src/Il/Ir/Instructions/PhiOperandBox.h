// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_PhiOperandBox_h)
#define INCLUDE_Il_Ir_PhiOperandBox_h

#include "./OperandBox.h"

namespace Il {
namespace Ir {

class PhiOperandBox :
        public IrObject_<PhiOperandBox, OperandBox> {

    CASTABLE_CLASS(PhiOperandBox)

    private: BBlock* m_pBB;

    public: BBlock& bblock() const { return *m_pBB; }

    // ctor
    public: PhiOperandBox(BBlock* const pBB, Operand* const pSx);

    // [G]
    public: BBlock* GetBB() const { return m_pBB; }
    public: BBlock* GetBBlock() const { return m_pBB; }

    // [S]
    public: BBlock* SetBB(BBlock* p) { return m_pBB = p; }

    DISALLOW_COPY_AND_ASSIGN(PhiOperandBox);
}; // PhiOperandBox

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_PhiOperandBox_h)
