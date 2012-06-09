// @(#)$Id$
//
// Evita Il - Ir - FinalTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_FinalTask_h)
#define INCLUDE_Il_Tasks_FinalTask_h

#include "./FunctionTask.h"
#include "./NormalizeTasklet.h"
#include "./WithChangeCount.h"

namespace Il {
namespace Tasks {

// FinalTask does nothing. This task provides for dump point.
class FinalTask
    : public FunctionTask_<FinalTask>,
      public WithChangeCount {

  // ctor
  public: FinalTask(Session&, Module&);

  // [P]
  private: virtual void ProcessFunction(Function&) override {}

  DISALLOW_COPY_AND_ASSIGN(FinalTask);
}; // FinalTask

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_FinalTask_h)
