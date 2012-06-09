// @(#)$Id$
//
// Evita Il - Tasks - Cfg2SsaTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_Cfg2SsaTask_h)
#define INCLUDE_Il_Tasks_Cfg2SsaTask_h

#include "./FunctionTask.h"

namespace Il {
namespace Tasks {

class Cfg2SsaTask : public FunctionTask_<Cfg2SsaTask> {
  // ctor
  public: Cfg2SsaTask(Session&, Module&);

  // [P]
  protected: virtual void ProcessFunction(Function&) override;

  DISALLOW_COPY_AND_ASSIGN(Cfg2SsaTask);
}; // Cfg2SsaTask

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_Cfg2SsaTask_h)
