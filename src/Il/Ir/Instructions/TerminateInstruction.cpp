#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - TerminateInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TerminateInstruction.h"

#include "../BBlock.h"
#include "../CfgEdge.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

void TerminateInstruction::Realize() {
    this->Instruction::Realize();
    auto const pBBlock = this->GetBB();
    auto const pFunction = pBBlock->GetFunction();
    auto const pExitBB = pFunction->GetExitBB();
    pBBlock->AddEdge(pExitBB)->SetEdgeKind(CfgEdge::Kind_Exit);
} // TerminateInstruction::Realize

void TerminateInstruction::Unrealize() {
    auto const pBBlock = this->GetBB();
    auto const pFunction = pBBlock->GetFunction();
    auto const pExitBB = pFunction->GetExitBB();
    pBBlock->RemoveOutEdge(pExitBB);
    this->Instruction::Unrealize();
} // TerminateInstruction::Unrealize

} // Ir
} // Il
