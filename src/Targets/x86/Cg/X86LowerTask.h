// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_X86_X86LowerTask_h)
#define INCLUDE_Il_Targets_X86_X86LowerTask_h

#include "../../../Il/Tasks/FunctionTask.h"
#include "./X86Functor.h"

namespace Il {
namespace Cg {

using namespace Il::Tasks;

class X86LowerTask
    : public FunctionTask_<X86LowerTask, FunctionTask, X86Functor> {
  // ctor
  // TODO(yosi) 2012-03-24 X86LowerTask should take CgSession.
  public: X86LowerTask(Session&, Module&);
  public: virtual ~X86LowerTask() {}

  // [P]
  private: virtual void ProcessFunction(Function&);

  DISALLOW_COPY_AND_ASSIGN(X86LowerTask);
}; // X86LowerTask

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_X86_X86LowerTask_h)
