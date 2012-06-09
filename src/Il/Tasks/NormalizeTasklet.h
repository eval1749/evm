// @(#)$Id$
//
// Evita Il - Tasks - NormalizeTasklet
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_NormalizeTasklet_h)
#define INCLUDE_Il_Tasks_NormalizeTasklet_h

#include "./EditInstructionTasklet.h"

#include "./OperandEvaluator.h"

namespace Il {
namespace Tasks {

class NormalizeTasklet :
  public EditInstructionTasklet,
  public Functor {

  private: OperandEvaluator m_oEvaluator;

  // ctor
  public: NormalizeTasklet(Session&, const Module&);

  // [E]
  protected: virtual void EditInstruction(Instruction*) override;

  // [G]
  protected: Field* GetField(Operand* const pSx);

  // [P]
  protected: virtual void Process(ArithmeticInstruction*) override;

  protected: virtual void Process(BranchI*) override;

  protected: virtual void Process(CallI*) override;

  protected: virtual void Process(EqI*) override;

  protected: virtual void Process(FieldPtrI*) override;

  protected: virtual void Process(IfI*) override;

  protected: virtual void Process(LoadI*) override;

  protected: virtual void Process(MulI*) override;
  protected: virtual void Process(NeI*) override;

  protected: virtual void Process(PhiI*) override;

  protected: virtual void Process(SelectI*) override;
  protected: virtual void Process(StaticCastI*) override;
  protected: virtual void Process(StoreI*) override;

  protected: virtual void Process(ValuesI*) override;
  protected: virtual void Process(VarDefI*) override;

  // [U]
  protected: void UpdateOutputType(Instruction&, const Type&);
  protected: void UpdateReturnType(Function*);

  DISALLOW_COPY_AND_ASSIGN(NormalizeTasklet);
}; // NormalizeTasklet

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_NormalizeTasklet_h)
