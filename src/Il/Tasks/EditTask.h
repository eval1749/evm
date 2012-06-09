// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_EditTask_h)
#define INCLUDE_Il_Tasks_EditTask_h

#include "Task.h"

#include "WithChangeCount.h"

namespace Il {
namespace Tasks {

using namespace Il::Ir;

class EditTask :
  public Task,
  public WithChangeCount {

  protected: EditTask(const String&, Session&);

  DISALLOW_COPY_AND_ASSIGN(EditTask);
}; // Task

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_EditTask_h)
