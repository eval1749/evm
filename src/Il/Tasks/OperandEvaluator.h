// @(#)$Id$
//
// Evita Il - Tasks - OperandEvaluator
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_OperandEvaluator_h)
#define INCLUDE_Il_Tasks_OperandEvaluator_h

#include "./Tasklet.h"

#include "../Functor.h"

namespace Il {
namespace Tasks {

class OperandEvaluator : public Tasklet, public Functor {
  private: const Module& module_;
  private: const Operand* value_;

  // ctor
  public: OperandEvaluator(Session&, const Module&);
  public: virtual ~OperandEvaluator() {}

  // [E]
  public: const Operand& Eval(const Operand&);

  // [H]
  private: bool HasNonLocalExitPoint(const Function&);

  // [I]
  private: bool IsSsaVariable(const Variable&);

  // [P]
  private: virtual void Process(AddI*) override;
  private: virtual void Process(CastInstruction*) override;
  private: virtual void Process(EqI*) override;
  private: virtual void Process(IfI*) override;
  private: virtual void Process(LoadI*) override;
  private: virtual void Process(MulI*) override;
  private: virtual void Process(NeI*) override;
  private: virtual void Process(PhiI*) override;
  private: virtual void Process(SelectI*) override;

  // [R]
  public: const Operand& Resolve(const Operand&);

  // [S]
  private: void SetValue(const Operand&);

  DISALLOW_COPY_AND_ASSIGN(OperandEvaluator);
}; // OperandEvaluator

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_OperandEvaluator_h)
