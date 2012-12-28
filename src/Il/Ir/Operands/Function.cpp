#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Operands - Function
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Function.h"

#include "../BBlock.h"
#include "../CfgEdge.h"
#include "../CgEdge.h"
#include "../Instructions.h"
#include "../Module.h"

#include "../../../Common/IntPtr.h"
#include "../../../Common/String.h"
#include "../../../Common/StringBuilder.h"

namespace Il {
namespace Ir {

Function::Function(
    Function* const pOuter,
    Flavor const eFlavor,
    const Name& name)
    : code_desc_(nullptr),
      m_cbFrame(0),
      flavor_(eFlavor),
      m_rgfFlag(0),
      m_pOuter(pOuter),
      m_pFunctionType(nullptr),
      name_(name) {}

void Function::set_code_desc(Ee::CodeDesc& r) {
  code_desc_ = &r;
}

// [A]
FrameReg* Function::AddFrameReg(FrameReg* const p) {
  return m_oFrameRegs.Append(p);
}

void Function::AppendBBlock(BBlock* const pBBlock) {
  ASSERT(pBBlock != nullptr);
  ASSERT(pBBlock->GetFunction() == nullptr);
  LayoutList::Append(pBBlock);
} // AppendBBlock

// [F]
PseudoOutput* Function::FindUpVar(const Variable* const pVar) const {
  ASSERT(pVar->GetOwner() != this);

  BBlock* pEntryBB = GetEntryBB();

  foreach (BBlock::EnumI, oEnum, pEntryBB) {
      auto const pUpVarDefI = oEnum.Get()->DynamicCast<UpVarDefI>();

      if (pUpVarDefI == nullptr) {
          continue;
      }

      if (pUpVarDefI->GetSx() == pVar) {
          return pUpVarDefI->GetQd();
      }
  } // for each insn

  return nullptr;
} // FindUpVar

// [G]
BBlock* Function::GetEntryBB() const {
  return static_cast<const LayoutList*>(this)->GetFirst();
} // GetEntryBB

BBlock* Function::GetExitBB() const {
  return static_cast<const LayoutList*>(this)->GetLast();
} // GetExitBB

PrologueI* Function::GetPrologueI() const {
  return GetStartBB()->GetFirstI()->StaticCast<PrologueI>();
} // GetPrologueI

Function* Function::GetSingleCaller() const {
  Function* pCaller = nullptr;
  foreach (Function::EnumInEdge, oEnum, this)
  {
      if (pCaller != nullptr) return nullptr;
      pCaller = oEnum.GetNode();
  } // for each inedge

  return pCaller;
} // GetSingleCaller

BBlock* Function::GetStartBB() const {
  return GetEntryBB()->GetLastI()->StaticCast<JumpI>()->
      GetLabel()->GetBB();
} // GetStartBB

// [H]
/// <summary>
///   Returns true if this function has nonlocal exit point
/// </summary>
bool Function::HasNonLocalExitPoint() const {
  foreach (EnumFrameReg, oEnum, this) {
      auto const pFd = oEnum.Get();

      auto const pOpenI = pFd->GetDefI();
      if (pOpenI == nullptr) {
          continue;
      }

      if (pOpenI->Is<OpenExitPointInstruction>()) {
          if (!pOpenI->IsUseless()) {
              return true;
          }
      }
  } // for each frame

  return false;
} // HasNonLocalExitPoint

bool Function::HasUpVar() const {
  return !m_oUpVarDefs.IsEmpty();
}

bool Function::HasUseSite() const {
  for (auto& user: users_) {
    if (!user.instruction().Is<OpenFinallyI>())
      return true;
  }
  return false;
}

// [I]

/// <summary>
///   Insert block after reference block.
/// </summary>
/// <param name="pBB">A block to be inserted</param>
/// <param name="pRefBB">A reference block</param>
/// <see cref="InsertBefore"/>
void Function::InsertAfter(const BBlock& bblock, const BBlock& ref_bblock) {
  LayoutList::InsertAfter(
      &const_cast<BBlock&>(bblock),
      &const_cast<BBlock&>(ref_bblock));
}

/// <summary>
///   Insert block before reference block.
/// </summary>
/// <param name="pBB">A block to be inserted</param>
/// <param name="pRefBB">A reference block</param>
/// <see cref="InsertAfter"/>
void Function::InsertBefore(const BBlock& bblock, const BBlock& ref_bblock) {
  LayoutList::InsertBefore(
      &const_cast<BBlock&>(bblock),
      &const_cast<BBlock&>(ref_bblock));
}

PseudoOutput* Function::InternUpVar(Variable* const pVar) {
  ASSERT(pVar->GetOwner() != this);

  if (auto const pQd = FindUpVar(pVar)) {
      return pQd;
  }

  auto const pQd = new PseudoOutput();
  auto const pEntryBB = GetEntryBB();

  pEntryBB->InsertBeforeI(
      *new UpVarDefI(pVar->GetDefI()->output_type(), *pQd, *pVar),
      pEntryBB->GetLastI());

  return pQd;
} // Function::InternUpVar

/// <summary>
///  Returns true if this function is closure.
/// </summary>
bool Function::IsClosure() const {
  if (!HasUpVar())
    return false;
  for (auto& user: users_) {
    if (!user.instruction().Is<OpenFinallyI>())
      return true;
  }
  return false;
} // IsClosure

// [M]
void Function::MakeVoid() {
  while (!bblocks().IsEmpty()) {
    auto const bblock = bblocks().GetFirst();
    RemoveBBlock(*bblock);
    delete bblock;
  }
}

// [R]
void Function::Realize(OperandBox* const pBox) {
  auto const pUserI = pBox->GetI();
  if (pUserI->Is<UpVarBaseI>()) {
      return;
  }

  if (pUserI->Is<CallI>()) {
      m_oCalls.Append(pBox);

  } else {
      users_.Append(pBox);
  }

  auto const pCaller = pBox->GetI()->GetBB()->GetFunction();

  auto pEdge = pCaller->FindEdgeTo(this);
  if (pEdge == nullptr) {
      pEdge = pCaller->AddEdge(this);
  }

  pEdge->m_cUsers += 1;
} // Realize

void Function::RemoveBBlock(BBlock& bblock) {
  while (auto const pI = bblock.GetFirstI()) {
      bblock.RemoveI(*pI);
  }
  LayoutList::Delete(&bblock);
}

// [S]
void Function::SetFunctionType(const FunctionType& fun_type) {
  m_pFunctionType = &fun_type;
} // SetFunctionType

// SetMethod may be called twice, Module::Clone and
// ConstructedClass::ConstructMethod
void Function::SetMethod(Method& method) {
  method_ = &method;
}

void Function::SetUp() {
  auto const pEntryBB = &module().NewBBlock();
  auto const pExitBB = &module().NewBBlock();
  auto const pStartBB = &module().NewBBlock();

  LayoutList::Append(pEntryBB);
  LayoutList::Append(pStartBB);
  LayoutList::Append(pExitBB);

  auto& zone = module().zone();
  pEntryBB->AppendI(*new(zone) EntryI());
  pEntryBB->AppendI(*new(zone) JumpI(pStartBB));
  pStartBB->AppendI(*new(zone) PrologueI(module().NewValues()));
  pExitBB->AppendI(*new(zone) ExitI());
} // Function

// [U]
void Function::Unrealize(OperandBox* const pBox) {
  auto const pUserI = pBox->GetI();
  if (pUserI->Is<UpVarBaseI>()) {
      return;
  }

  if (pUserI->Is<CallI>()) {
      m_oCalls.Delete(pBox);

  } else {
      users_.Delete(pBox);
  }

  auto const pCaller = pBox->GetI()->GetBB()->GetFunction();
  auto const pEdge = pCaller->FindEdgeTo(this);

  if (pEdge == nullptr) {
      COMPILER_INTERNAL_ERROR();
      return;
  }

  pEdge->m_cUsers -= 1;
  if (0 == pEdge->m_cUsers) {
      pCaller->RemoveEdge(pEdge);
  }
} // Unrealize

} // Ir
} // Il
