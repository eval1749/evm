#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - CopyInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CopyInstruction.h"

namespace Il {
namespace Cg {

//ctor
CopyInstruction::CopyInstruction(
    Op const eOp,
    const Type* const pType,
    const Output* const pOutput)
    : Base(eOp, pType, pOutput) {}

CopyInstruction::CopyInstruction(
    Op const eOp,
    const Type& outy,
    const Output& out )
    : Base(eOp, outy, out) {}

// [I]
bool CopyInstruction::IsUseless() const {
    if (this->GetOutput() == this->GetSx()) {
        return true;
    }

    if (auto const* pPrevI = GetPrev()) {
        if (pPrevI->Is<CopyInstruction>()) {
            // Copy %r1 <= %r2
            // Copy %r2 <= %r1
            if (GetOutput() == pPrevI->GetSx()
                    && GetSx() == pPrevI->GetOutput()) {
                return true;
            }

            // Copy %r1 <= %r2
            // Copy %r1 <= %r2
            if (GetOutput() == pPrevI->GetOutput()
                    && GetSx() == pPrevI->GetSx()) {
                return true;
            }
        }
    } // if prev

    return false;
} // IsUseless

} // Cg
} // Il
