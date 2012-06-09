// @(#)$Id$
//
// Evita Il - IR - Operands - Label
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Label_h)
#define INCLUDE_Il_Ir_Label_h

#include "./Immediate.h"

namespace Il {
namespace Ir {

class Label :
      public DoubleLinkedItem_<Label>,
      public Operand_<Label, Immediate> {

  CASTABLE_CLASS(Label)

  public: typedef DoubleLinkedList_<Label> List;

  private: BBlock* m_pBBlock;

  // ctor
  public: Label(BBlock* const pBBlick);

  // [E]
  public: virtual bool Equals(const Operand&) const override;

  // [G]
  public: BBlock* GetBB() const { return m_pBBlock; }

  // [S]
  public: BBlock* SetBB(BBlock* p) { return m_pBBlock = p; }

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(Label);
}; // Label

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Label_h)
