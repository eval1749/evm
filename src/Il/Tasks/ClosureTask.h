// @(#)$Id$
//
// Evita Il - Tasks - ClosureTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_ClosureTask_h)
#define INCLUDE_Il_Tasks_ClosureTask_h

#include "./FunctionTask.h"

namespace Il {
namespace Tasks {

class ClosureTask : public FunctionTask_<ClosureTask> {
  // ctor
  public: ClosureTask(Session&, Module&);

  // [P]
  protected: virtual void ProcessFunction(Function&) override;

  // [S]
  // Note: We process Module rather than each function.
  public: virtual void Start() override;

  DISALLOW_COPY_AND_ASSIGN(ClosureTask);
}; // ClosureTask

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_ClosureTask_h)
