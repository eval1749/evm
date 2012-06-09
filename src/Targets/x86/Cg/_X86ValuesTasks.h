// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_X86_X86ValuesTask_h)
#define INCLUDE_Il_Targets_X86_X86ValuesTask_h

#include "../../../Il/Tasks/FunctionTask.h"
#include "./X86Functor.h"

namespace Il {
namespace Cg {

using namespace Il::Tasks;

class X86ValuesTask
    : public FunctionTask_<X86ValuesTask, FunctionTask, X86Functor> {
  // ctor
  public: X86ValuesTask(Session&, Module&);
  public: virtual ~X86ValuesTask() {}

  // [P]
  private: virtual void ProcessFunction(Function&);

  DISALLOW_COPY_AND_ASSIGN(X86ValuesTask);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_X86_X86ValuesTask_h)
