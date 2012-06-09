// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_Task_h)
#define INCLUDE_Il_Tasks_Task_h

#include "./Tasklet.h"

namespace Il {
namespace Tasks {

using namespace Il::Ir;

class Task : public Tasklet {
  // ctor
  protected: Task(const String&, Session&);

  public: virtual ~Task() {}

  // [S]
  public: virtual void Start() = 0;

  DISALLOW_COPY_AND_ASSIGN(Task);
}; // Task

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_Task_h)
