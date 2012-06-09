#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Label
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LabelDef.h"

#include "./MethodDef.h"

namespace Compiler {

// ctor
LabelDef::LabelDef(
    MethodDef* const pOwner,
    const Name& name,
    BBlock* const pBBlock,
    const SourceInfo& source_info) :
      Base(*pOwner, name, source_info),
      is_defined_(false),
      m_pBBlock(pBBlock),
      m_pFrameReg(nullptr) {
  ASSERT(m_pBBlock != nullptr);
}

// [A]
void LabelDef::Add(
  BBlock* const pBBlock,
  FrameReg* const pFrameReg) {
  ASSERT(pBBlock != nullptr);
  m_oLabelRefs.Add(LabelRef(pBBlock, pFrameReg));
}

// [D]
void LabelDef::Define(FrameReg* const pFrameReg) {
  m_pFrameReg = pFrameReg;
  is_defined_ = true;
}

// [I]
bool LabelDef::IsDefined() const {
  return is_defined_;
}

/// <summary>
///   Return true if there is an incoming edge.
/// </summary>
bool LabelDef::IsUsed() const {
  return m_pBBlock->CountInEdges() > 0;
}

} // Compiler
