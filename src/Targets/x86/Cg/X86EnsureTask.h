// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_X86_X86EnsureTask_h)
#define INCLUDE_Il_Targets_X86_X86EnsureTask_h

#include "../../../Il/Tasks/FunctionTask.h"
#include "./X86Functor.h"

namespace Il {
namespace Cg {

using namespace Il::Tasks;

// o Rewrite relational instructions, e.g. Eq, Gt, Lt, and so on to
//   x86Cmp + x86Bool.
// o Rewrite binary operation using Assing and Copy for assigning same
//   register to output and first operand.
//      ADD a = b + c => Assign d = b; ADD e = d + c; Copy a = e
class X86EnsureTask
    : public FunctionTask_<X86EnsureTask, FunctionTask, X86Functor> {
  // ctor
  public: X86EnsureTask(Session&, Module&);
  public: virtual ~X86EnsureTask() {}

  // [P]
  private: virtual void ProcessFunction(Function&);

  DISALLOW_COPY_AND_ASSIGN(X86EnsureTask);
}; // X86EnsureTask

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_X86_X86EnsureTask_h)
