#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Operands - Variable
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Variable.h"

#include "./Function.h"
#include "./Class.h"
#include "./PrimitiveType.h"

#include "../Name.h"

namespace Il {
namespace Ir {

// ctor
Variable::Variable(const Name& name)
    : m_cUpRefs(0),
      m_eStorage(Storage_Closed),
      m_iLocation(-1),
      m_pDefI(nullptr),
      name_(name),
      type_(Ty_Object) {}

Variable::Variable() : name_(Name::Intern("?")) {}

// [H]
Variable* Variable::Get(Operand* const pSx) {
  if (auto const pRx = pSx->DynamicCast<Register>()) {
      return pRx->GetVariable();
  }
  return nullptr;
} // Variable::Get

Function* Variable::GetOwner() const {
  auto pBB = GetDefI()->GetBB();
  if (nullptr == pBB) {
      return nullptr;
  } // if
  return pBB->GetFunction();
} // Variable::GetOwner

Register* Variable::GetRd() const
  { return GetDefI()->GetRd(); }

// [R]
void Variable::Realize(OperandBox* const pBox) {
  ASSERT(nullptr != pBox);

  auto const pFun = pBox->GetI()->GetBB()->GetFunction();
  ASSERT(nullptr != pFun);

  if (this->GetOwner() == pFun) {
      pFun->m_oVarDefs.Append(pBox);

  } else {
      // Note: Up variables are listed in EntryBB
      pFun->m_oUpVarDefs.Append(pBox);
      m_cUpRefs += 1;
  }
} // Variable::Realize

// [S]
void Variable::SetTy(const Type& type) {
  ASSERT(&type != Ty_Void);
  type_ = &type;
}

// [T]
String Variable::ToString() const {
  return name_;
}

// [U]
void Variable::Unrealize(OperandBox* const pBox) {
  ASSERT(nullptr != pBox);

  auto const pFun = pBox->GetI()->GetBB()->GetFunction();
  ASSERT(nullptr != pFun);

  if (this->GetOwner() == pFun) {
      pFun->m_oVarDefs.Delete(pBox);

  } else {
      // Note: Up variables are listed in EntryBB
      pFun->m_oUpVarDefs.Delete(pBox);
      m_cUpRefs -= 1;
  }
} // Variable::Unrealize

} // Ir
} // Il
