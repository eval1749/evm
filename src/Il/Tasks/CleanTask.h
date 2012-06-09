// @(#)$Id$
//
// Evita Il - Ir - CleanTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_CleanTask_h)
#define INCLUDE_Il_Tasks_CleanTask_h

#include "./FunctionTask.h"
#include "./NormalizeTasklet.h"
#include "./WithChangeCount.h"

namespace Il {
namespace Tasks {

class CleanTask :
    public FunctionTask_<CleanTask>,
    public WithChangeCount {

  private: NormalizeTasklet normalizer_;

  // ctor
  public: CleanTask(Session&, Module&);

  // [F]
  private: void FoldBranch(const BranchI&, const BBlock&);

  // [H]
  private: void HandleBranch(const BranchI&);
  private: void HandleJump(const JumpI&);
  private: void HandleSwitch(const SwitchI&);

  // [P]
  private: void ProcessBBlock(const BBlock&);
  private: virtual void ProcessFunction(Function&);

  // [R]
  private: void RemovePhis(const BBlock&, const BBlock&);

  // [T]
  private: bool TryCoalesce(const JumpI&);
  private: bool TryJumpPhiRet(const JumpI&);
  private: bool TryRemoveEmpty(const JumpI&);

    DISALLOW_COPY_AND_ASSIGN(CleanTask);
}; // CleanTask

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_CleanTask_h)
