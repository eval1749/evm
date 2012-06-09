// @(#)$Id$
//
// Evita Il - IR - OperandBox
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_OperandBox_h)
#define INCLUDE_Il_Ir_OperandBox_h

#include "../IrObject.h"

namespace Il {
namespace Ir {

class OperandBox :
      public IrObject_<OperandBox, IrObject>,
      public ChildItem_<OperandBox, Instruction>,
      public DoubleLinkedItem_<OperandBox, Operand>,
      public LocalObject,
      public WorkListItem_<OperandBox> {

  CASTABLE_CLASS(OperandBox);

  private: Operand* m_pOperand;
  public:  Physical* m_pPhysical;

  // ctor
  public: explicit OperandBox(const Operand*);

  protected: OperandBox() :
      m_pOperand(nullptr),
      m_pPhysical(nullptr) {}

  public: Instruction& instruction() const { return *m_pParent; }

  // [G]
  public: Instruction* GetI() const { return m_pParent; }
  public: Operand* GetOperand() const { return m_pOperand; }
  public: Register* GetRx() const;

  // [I]
  protected: void init(const Operand* const p) {
    m_pOperand = const_cast<Operand*>(p);
    m_pPhysical = nullptr;
  } // init

  // [R]
  public: void Replace(const Operand&);

  // [S]
  public: void SetOperand(const Operand&);
  public: void SetOperand(const Operand* p) { SetOperand(*p); }

  DISALLOW_COPY_AND_ASSIGN(OperandBox);
}; // OperandBox

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_OperandBox_h)
