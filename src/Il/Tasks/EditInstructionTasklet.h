// @(#)$Id$
//
// Evita Il - Tasks - EditInstructionTasklet
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_EditInstructionTasklet_h)
#define INCLUDE_Il_Tasks_EditInstructionTasklet_h

#include "./EditTasklet.h"

namespace Il {
namespace Tasks {

class EditInstructionTasklet : public EditTasklet {
  private: WorkList_<Instruction> m_oWorkList;
  private: const Module& module_;

  // ctor
  protected: EditInstructionTasklet(
      const String&,
      Session& session,
      const Module&);

  protected: virtual ~EditInstructionTasklet();

  public: const Module& module() const { return module_; }

  // [A]
  public: void Add(Instruction&);
  public: void Add(Output&);
  public: void Add(SsaOutput&);

  public: void Add(Instruction* p) { Add(*p); }
  public: void Add(Output* p) { Add(*p); }
  public: void Add(SsaOutput* p) { Add(*p); }

  // [E]
  protected: virtual void EditInstruction(Instruction* const pI) = 0;

  // [M]
  public: void MakeEmpty();

  // [S]
  public: void Start();

  protected: void SwapOperands(
      Instruction&,
      int const iX = 0,
      int const iY = 1);

  DISALLOW_COPY_AND_ASSIGN(EditInstructionTasklet);
}; // EditInstructionTasklet

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_EditInstructionTasklet_h)
