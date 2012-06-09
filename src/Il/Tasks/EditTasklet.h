// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_EditTasklet_h)
#define INCLUDE_Il_Tasks_EditTasklet_h

#include "Tasklet.h"

#include "WithChangeCount.h"

namespace Il {
namespace Tasks {

using namespace Il::Ir;

// TODO 2011-01-05 yosi@msn.com Should we use WithChangeCount?
class EditTasklet : public Tasklet, public WithChangeCount {
  protected: EditTasklet(const String&, Session&);
  DISALLOW_COPY_AND_ASSIGN(EditTasklet);
}; // Tasklet

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_EditTasklet_h)
