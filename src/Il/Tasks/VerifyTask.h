// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_VerifyTask_h)
#define INCLUDE_Il_Tasks_VerifyTask_h

#include "./FunctionTask.h"

#include "../../Common/Collections.h"
#include "../../Common/LocalMemoryZone.h"

namespace Il {
namespace Tasks {

using namespace Common::Collections;
using namespace Il::Ir;

// Use LocalMemoryZone for BBlock HashMap.
class VerifyTask : public FunctionTask_<VerifyTask> {
  private: struct Entry;
  private: typedef ArrayList_<Entry*> EntryList;
  private: EntryList fail_list_;

  // ctor
  public: VerifyTask(Session&, Module&);
  public: virtual ~VerifyTask();

  // [A]
  protected: void Add(const Instruction&, String);
  protected: void Add(const Instruction&, String, Box);
  protected: void Add(const Instruction&, String, Box, Box);
  protected: void Add(const Instruction&, String, Box, Box, Box);
  protected: void Add(const Instruction&, String, Box, Box, Box, Box);

  // [P]
  protected: virtual void Process(ArithmeticInstruction*) override;

  protected: virtual void Process(BBlock* const pBBlock) override;

  protected: virtual void Process(BoxI*) override;

  protected: virtual void Process(CallI*) override;

  protected: virtual void Process(EltRefI*) override;
  protected: virtual void Process(EntryI*) override;

  protected: virtual void Process(FieldPtrI*) override;

  protected: virtual void Process(IfI*) override;
  protected: virtual void Process(Instruction*) override;

  protected: virtual void Process(LastInstruction*) override;
  protected: virtual void Process(LoadI*) override;

  protected: virtual void Process(NewI*) override;
  protected: virtual void Process(NewArrayI*) override;

  protected: virtual void Process(PhiI*) override;

  protected: virtual void Process(RelationalInstruction*) override;
  protected: virtual void Process(RetI*) override;

  protected: virtual void Process(ValuesI*) override;

  protected: virtual void Process(StoreI*) override;

  protected: virtual void ProcessFunction(Function&) override;

  // [V]
  private: bool Verify(const Instruction&);
  public: void VerifyFunction(const Function&);

  DISALLOW_COPY_AND_ASSIGN(VerifyTask);
}; // VerifyTask

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_VerifyTask_h)
