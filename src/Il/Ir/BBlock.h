// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_BBlock_h)
#define INCLUDE_Il_Ir_BBlock_h

#include "./CfgEdge.h"
#include "./GraphNode.h"
#include "./IrObject.h"

#include "../Functor.h"

#include "./Instructions/Instruction.h"
#include "./Operands/Label.h"

namespace Il {
namespace Tasks {
class FunctionEditor;
} // Tasks
} // Il

namespace Il {
namespace Ir {

class DataFlowData;
class LoopNode;

/// <summary>
///  Internal representation of basic block.
/// </summary>
class BBlock
    : public LocalObject,
      protected ChildList_<Instruction, BBlock>,
      public Extendable_<BBlock>,
      public GraphNode_<Function, BBlock, CfgEdge>,
      public IrObject_<BBlock, IrObject>,
      public WithIndex,
      public WorkListItem_<BBlock>,
      public WithWorkArea {

  CASTABLE_CLASS(BBlock);

  // for ctor
  friend class Module;

  // for ReplaceLabels, ReplaceEdgeTo, and ReplacePhiOperands
  friend class Il::Tasks::FunctionEditor;

  public: class EnumLabel : public Label::List::Enum {
      public: EnumLabel(const BBlock* const p) :
          Label::List::Enum(&p->m_oLabels) {}
  }; // EnumLabel

  public: typedef ChildList_<Instruction, BBlock> Instructions;

  private: int m_iName;
  private: Label::List m_oLabels;
  protected: DataFlowData* m_pDfData;
  private: LoopNode* m_pLoopNode;

  // ctor
  private: BBlock(int);
  public: virtual ~BBlock();

  // For DataFlowBB
  protected: BBlock() {}

  public: Label& label() const { return *m_oLabels.GetFirst(); }
  public: int name() const { return m_iName; }

  // [A]
  public: void AppendI(const Instruction&);
  private: void AddLabel(Label&);

  // [C]
  // VerifyTask put BBlock into HashMap.
  public: virtual int ComputeHashCode() const {
      return static_cast<int>(reinterpret_cast<intptr_t>(this));
  } // ComputeHashCode

  // [D]
  public: bool DoesDominate(const BBlock&) const;

  // [E]
  class_Enum_(BBlock, I, Instructions)

  public: class EnumPhiI : public EnumI {
      public: EnumPhiI(const BBlock* pBBlock) : EnumI(pBBlock) {}
      public: bool AtEnd() const;
      public: PhiI* Get() const;
  }; // EnumPhiI

  // [G]
  public: Instruction* GetFirstI() const;
  public: Function* GetFunction() const { return GetGraphParent(); }
  public: Instruction* GetLastI() const;

  public: LoopNode* GetLoopNode() const { return m_pLoopNode; }

  public: BBlock* GetNext() const {
    return static_cast<const LayoutNode*>(this)->GetNext();
  }

  public: BBlock* GetPrev() const {
    return static_cast<const LayoutNode*>(this)->GetPrev();
  }

  // [I]
  public: void InsertAfterI(const Instruction&, const Instruction*);
  public: void InsertAfterI(const Instruction& inst, const Instruction&);
  public: void InsertBeforeI(const Instruction&, const Instruction*);
  public: void InsertBeforeI(const Instruction&, const Instruction&);
  public: bool IsExitBB() const;
  public: bool IsLiveIn(int) const;
  public: bool IsLiveOut(int) const;

  // [M]
  public: void MoveBeforeI(const Instruction&, const Instruction*);
  public: void MoveLabelsTo(BBlock&);

  // [P]
  private: Instructions* PrepareTraversal(Instructions* p) { return p; }

  // [R]
  public: void BBlock::RedirectEdgeFrom(
      BBlock* const pNewBB,
      BBlock* const pOldBB);

  public: void BBlock::RedirectEdgeTo(
      BBlock* const pNewBB,
      BBlock* const pOldBB);

  public: void RemoveI(const Instruction&);
  private: void RemoveLabel(Label&);
  public: void ReplaceI(const Instruction&, const Instruction&);

  private: void BBlock::ReplaceLabels(
      BBlock* const pNewBB,
      BBlock* const pOldBB);

  private: void BBlock::ReplacePhiOperands(
      BBlock* const pNewBB, 
      BBlock* const pOldBB);

  public: void Reset();

  // [S]
  public: void SetLiveIn(int);
  public: void SetLiveOut(int);
  public: void SetLoopNode(LoopNode* p) { m_pLoopNode = p; }
  public: void SetName(int const iName) { m_iName = iName; }
  public: BBlock* SplitBefore(Instruction* const pRefI);

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(BBlock);
}; // BBlock

inline int ComputeHashCode(const BBlock* const p) {
  ASSERT(p != nullptr);
  return p->ComputeHashCode();
}

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_BBlock_h)
