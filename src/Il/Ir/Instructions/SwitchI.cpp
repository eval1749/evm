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
    AppendOperandBox(pBox);

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
  for (auto& case_box: case_boxes()) {
    if (case_box.operand() == *pLx)
      return &case_box;
  }
  return nullptr;
}

// [G]
BBlock* SwitchI::GetDefaultBB() const {
    return this->GetSy()->StaticCast<Label>()->GetBB();
} // GetDefaultBB

// [R]
// Note: Usually, SwtichI is realized before having cases.
void SwitchI::Realize() {
    this->Instruction::Realize();
    this->GetBB()->AddEdge(this->GetDefaultBB());

    for (const auto& case_box: case_boxes()) {
        auto const pTargetBB = case_box.GetBB();
        this->GetBB()->AddEdge(pTargetBB);
    } // for
} // Realize

void SwitchI::Unrealize() {
    this->GetBB()->RemoveOutEdge(this->GetDefaultBB());

    for (const auto& case_box: case_boxes()) {
        auto const pTargetBB = case_box.GetBB();
        this->GetBB()->RemoveOutEdge(pTargetBB);
    } // for

    this->Instruction::Unrealize();
} // Unrealize

} // Ir
} // Il
