// @(#)$Id$
//
// Evita Il - IR - ModuleTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_ModuleTask_h)
#define INCLUDE_Il_Tasks_ModuleTask_h

#include "./Task.h"

#include "../Functor.h"

namespace Il {
namespace Tasks {

using namespace Il::Ir;

class ModuleTask : public Task {
  public: typedef ModuleTask& (*NewFunction)(
      Session& session,
      Module& module);

  private: Module& module_;

  // ctor
  protected: ModuleTask(const String&, Session&, Module&);

  // properties
  public: Module& module() const {
    return const_cast<ModuleTask*>(this)->module_;
  }

  DISALLOW_COPY_AND_ASSIGN(ModuleTask);
};

template<class TSelf, class TParent = ModuleTask>
class ModuleTask_ : public TParent {
  protected: typedef ModuleTask_<TSelf, TParent> Base;

  protected: ModuleTask_(
      const String& name,
      Session& session,
      Module& module)
      : TParent(name, session, module) {}

  public: static ModuleTask& Create(Session& session, Module& module) {
    return *new TSelf(session, module);
  }

  DISALLOW_COPY_AND_ASSIGN(ModuleTask_);
};

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_ModuleTask_h)
