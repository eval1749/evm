#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - SsaOutput
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./SsaOutput.h"

#include "../Instructions.h"

namespace Il {
namespace Ir {

// [G]
OperandBox* SsaOutput::GetFirstUser() const {
    return static_cast<const Users*>(this)->GetFirst();
} // GetFirstUser

OperandBox* SsaOutput::GetSingleUser() const {
    EnumUser oEnum(this);
    if (oEnum.AtEnd()) return nullptr;
    auto const pBox = oEnum.Get();
    oEnum.Next();
    if (!oEnum.AtEnd()) return nullptr;
    return pBox;
} // GetSingleUser

/// <summary>
///   Retreive type of SsaOutput.
/// </summary>
const Type& SsaOutput::GetTy() const {
  ASSERT(GetDefI() != nullptr);
  return GetDefI()->output_type();
}

// [H]
bool SsaOutput::HasUser() const {
    return !static_cast<const Users*>(this)->IsEmpty();
} // HasUser

// [R]
/// <summary>
///   Update Used-Def chain.
/// </summary>
void SsaOutput::Realize(OperandBox* const pBox) {
    ASSERT(nullptr != pBox->GetI());
    ASSERT(pBox->GetI()->IsRealized());
    static_cast<Users*>(this)->Append(pBox);
} // SsaOutput::Realize

// [U]
void SsaOutput::Unrealize(OperandBox* const pBox) {
    static_cast<Users*>(this)->Delete(pBox);
} // SsaOutput::Unrealize

} // Ir
} // Il
