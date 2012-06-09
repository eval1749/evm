#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Function
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Label.h"

#include "../BBlock.h"

namespace Il {
namespace Ir {

// ctor
Label::Label(BBlock* const pBBlock) :
      m_pBBlock(pBBlock) {
  ASSERT(m_pBBlock != nullptr);
} // Label

// [E]
bool Label::Equals(const Operand& another) const { 
  if (auto const label = another.DynamicCast<const Label>()) {
    return GetBB() == label->GetBB();
  }
  return false;
} // Label::Equal

// [T]
String Label::ToString() const {
  return GetBB()->ToString();
}

} // Ir
} // Il
