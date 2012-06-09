// @(#)$Id$
//
// Evita Il - FunctionEditor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_FunctionEditor_h)
#define INCLUDE_Il_Tasks_FunctionEditor_h

#include "./FunctionTasklet.h"

namespace Il {
namespace Tasks {

class FunctionEditor : public FunctionTasklet {
  // ctor
  public: FunctionEditor(Session&);

  // [E]
  public: bool EliminateInfiniteLoop(Function&);

  // [R]
  public: void Renumber(Function&);
  public: void RemoveCriticalEdges(Function&);
  DISALLOW_COPY_AND_ASSIGN(FunctionEditor);
};

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_FunctionEditor_h)
