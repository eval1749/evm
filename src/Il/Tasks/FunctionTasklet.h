// @(#)$Id$
//
// Evita Il - FunctionTasklet
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_FunctionTasklet_h)
#define INCLUDE_Il_Tasks_FunctionTasklet_h

#include "./Tasklet.h"

namespace Il {
namespace Tasks {

class FunctionTasklet : public Tasklet {
  // ctor
  protected: FunctionTasklet(const String&, Session&);
  DISALLOW_COPY_AND_ASSIGN(FunctionTasklet);
}; // FunctionTasklet

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_FunctionTasklet_h)
