#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - SwitchI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./SwitchI.h"
#include "./SwitchOperandBox.h"

#include "../CfgEdge.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

SwitchI::EnumCase::EnumCase(const SwitchI* const pSwitchI) :
    Base(pSwitchI) {
    if (!this->AtEnd()) {
        this->Next();
    } // if

    if (!this->AtEnd()) {
        this->Next();
    } // if
} // EnumCase

SwitchOperandBox* SwitchI::EnumCase::Get() const {
    ASSERT(!this->AtEnd());
    return this->Base::GetBox()->StaticCast<SwitchOperandBox>();
} // Get

// ctor
SwitchI::SwitchI() :
    Base(Ty_Void, Void) {}

SwitchI::SwitchI(
    Operand* const pSx,
    Label* const pLabel) :
        Base(Ty_Void, Void) {
    this->AppendOperand(pSx);
    this->AppendOperand(pLabel);
} // SwitchI

// [A]
// Note: We take Operand instead of Literal for FASL and Parser.
void SwitchI::AddOperand(
    BBlock* const pTargetBB,
    Operand* const pLx) {

    ASSERT(pLx != nullptr);
    ASSERT(pTargetBB != nullptr);

    auto const pBox = new SwitchOperandBox(pTargetBB, pLx);
    this->AppendOperand(pBox);

    if (this->IsRealized()) {
        this->GetBB()->AddEdge(pTargetBB);
    } // if
} // AddOperand

// [F]
/// <summary>
///   Use Case:
///     <list>
///         <item>Parser: goto case statment.</item>
///         <item>Parser: switch statement.</item>
///     </list>
/// </summary>
SwitchOperandBox* SwitchI::FindOperand(Operand* const pLx) const {
    foreach (EnumCase, oEnum, this) {
        auto const pBox = oEnum.Get();
        if (*pBox->GetOperand() == *pLx) {
            return pBox;
        } // if
    } // for

    return nullptr;
} // FindOperand

// [G]
BBlock* SwitchI::GetDefaultBB() const {
    return this->GetSy()->StaticCast<Label>()->GetBB();
} // GetDefaultBB

// [R]
// Note: Usually, SwtichI is realized before having cases.
void SwitchI::Realize() {
    this->Instruction::Realize();
    this->GetBB()->AddEdge(this->GetDefaultBB());

    foreach (EnumCase, oEnum, this) {
        auto const pBox = oEnum.Get();
        auto const pTargetBB = pBox->GetBB();
        this->GetBB()->AddEdge(pTargetBB);
    } // for
} // Realize

void SwitchI::Unrealize() {
    this->GetBB()->RemoveOutEdge(this->GetDefaultBB());

    foreach (EnumCase, oEnum, this) {
        auto const pBox = oEnum.Get();
        auto const pTargetBB = pBox->GetBB();
        this->GetBB()->RemoveOutEdge(pTargetBB);
    } // for

    this->Instruction::Unrealize();
} // Unrealize

} // Ir
} // Il
