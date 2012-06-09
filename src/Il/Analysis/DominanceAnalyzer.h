// @(#)$Id$
//
// Evita Il - DominanceAnalyzer
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_DominanceAnalyzer_h)
#define INCLUDE_Il_Tasks_DominanceAnalyzer_h

#include "../Tasks/FunctionTasklet.h"

namespace Il {
namespace Tasks {

class DominanceAnalyzer : public FunctionTasklet {
  // ctor
  public: DominanceAnalyzer(Session&);

  // [E]
  public: bool ComputeDominance(const Function&);
  public: bool ComputePostDominance(const Function&);
  DISALLOW_COPY_AND_ASSIGN(DominanceAnalyzer);
};

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_DominanceAnalyzer_h)
