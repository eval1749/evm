#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - OpenInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./OpenInstruction.h"

#include "./CloseI.h"
#include "./UseI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
OpenInstruction::OpenInstruction(
    Op const eOp,
    const Type* const pType,
    const Output* const pOutput) :
        Base(eOp, pType, pOutput) {}

// [I]
bool OpenInstruction::IsNextClose() const {
    if (auto const pCloseI = GetNext()->DynamicCast<CloseI>()) {
        return pCloseI->GetSx() == GetOutput();
    }
    return false;
} // IsNextClose

bool OpenInstruction::IsUseless() const
{
    if (this->IsNextClose())
    {
        // No instruction between Open and Close.
        return true;
    }

    foreach (FrameReg::EnumUser, oEnum, GetOutput()->StaticCast<FrameReg>())
    {
        Instruction* const pI = oEnum.GetI();
        if (pI->Is<CloseI>())
        {
            continue;
        }

        if (pI->Is<UseI>())
        {
            continue;
        }

        return false;
    } // for each use
    return true;
} // OpenInstruction::IsUseless

// [R]
void OpenInstruction::Realize()
{
    Instruction::Realize();

    GetBB()->GetFunction()->AddFrameReg(
        GetOutput()->StaticCast<FrameReg>() );
} // OpenInstruction::Realize

// [U]
void OpenInstruction::Unrealize()
{
    GetBB()->GetFunction()->RemoveFrameReg(
        GetOutput()->StaticCast<FrameReg>() );

    Instruction::Unrealize();
} // OpenInstruction::Unrealize

} // Ir
} // Il
