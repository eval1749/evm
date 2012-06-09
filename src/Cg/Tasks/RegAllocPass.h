// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_RegAllocPass_h)
#define INCLUDE_Il_Cg_RegAllocPass_h

#include "../../Il/Tasks/FunctionTask.h"
#include "../CgFunctor.h"

namespace Il {
namespace Cg {

using namespace Il::Ir;
using namespace Il::Tasks;

class RegAllocPass
    : public FunctionTask_<RegAllocPass, FunctionTask, CgFunctor> {

  private: CgSession& session_;

  // ctor
  public: RegAllocPass(Session&, Module&);

  // [P]
  protected: virtual void ProcessFunction(Function&) override;

  DISALLOW_COPY_AND_ASSIGN(RegAllocPass);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_RegAllocPass_h)
