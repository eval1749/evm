// @(#)$Id$
//
// Evita Il - IR - FunctionTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_FunctionTask_h)
#define INCLUDE_Il_Tasks_FunctionTask_h

#include "./ModuleTask.h"

#include "../Functor.h"

namespace Il {
namespace Tasks {

using namespace Il::Ir;

class FunctionTask : public ModuleTask {
  // ctor
  protected: FunctionTask(const String&, Session&, Module&);

  // [P]
  protected: void ProcessBBlockDefault(BBlock&, Functor&);
  protected: void ProcessFunctionDefault(Function&, Functor&);

  protected: virtual void ProcessFunction(Function&) = 0;

  // [S]
  public: virtual void Start() override;

  DISALLOW_COPY_AND_ASSIGN(FunctionTask);
}; // FunctionTask

template<class TSelf, class TParent = FunctionTask, class TFunctor = Functor>
class FunctionTask_ : public TParent, protected TFunctor {
  protected: typedef FunctionTask_<TSelf, TParent, TFunctor> Base;

  protected: FunctionTask_(
      const String& name,
      Session& session,
      Module& module)
      : TParent(name, session, module) {}

  public: static ModuleTask& Create(Session& session, Module& module) {
    return *new TSelf(session, module);
  }

  protected: virtual void Process(BBlock* bblock) override {
    ProcessBBlockDefault(*bblock, *this);
  }

  protected: virtual void Process(Function* fn) override {
    ProcessFunctionDefault(*fn, *this);
  }

  DISALLOW_COPY_AND_ASSIGN(FunctionTask_);
}; // FunctionTask_

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_FunctionTask_h)
