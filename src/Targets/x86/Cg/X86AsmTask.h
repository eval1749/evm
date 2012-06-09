// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_X86_X86AsmTask_h)
#define INCLUDE_Il_Targets_X86_X86AsmTask_h

#include "../../../Il/Tasks/FunctionTask.h"
#include "./X86Functor.h"

namespace Il {
namespace Cg {

using namespace Il::Tasks;

class X86AsmTask : public ModuleTask_<X86AsmTask> {
  // ctor
  public: X86AsmTask(Session&, Module&);

  // [S]
  public: virtual void Start() override;

  DISALLOW_COPY_AND_ASSIGN(X86AsmTask);
}; // X86AsmTask

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_X86_X86AsmTask_h)
