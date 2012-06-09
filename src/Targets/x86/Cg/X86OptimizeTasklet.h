// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_X86_X86OptimizeTask_h)
#define INCLUDE_Il_Targets_X86_X86OptimizeTask_h

#include "../../../Il/Tasks/FunctionTask.h"
#include "../../../Il/Tasks/NormalizeTasklet.h"
#include "./X86Functor.h"

namespace Il {
namespace Cg {

using namespace Il::Tasks;

class X86NormalizeTasklet : public NormalizeTasklet, public X86Functor {
  protected: X86NormalizeTasklet(Session& session, const Module& module)
      : NormalizeTasklet(session, module) {}
  DISALLOW_COPY_AND_ASSIGN(X86NormalizeTasklet);
};

class X86OptimizeTasklet : public X86NormalizeTasklet {
  // ctor
  public: X86OptimizeTasklet(Session&, const Module&);
  public: virtual ~X86OptimizeTasklet() {}

  // [P]
  private: virtual void Process(x86LeaI* const pI) override;

  DISALLOW_COPY_AND_ASSIGN(X86OptimizeTasklet);
}; // X86OptimizeTasklet

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_X86_X86OptimizeTask_h)
