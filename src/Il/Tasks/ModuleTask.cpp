#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - FunctionTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FunctionTask.h"

#include "../Ir/Module.h"
#include "../Ir/Operands/Function.h"

namespace Il {
namespace Tasks {

ModuleTask::ModuleTask(
    const String& name,
    Session& session,
    Module& module)
  : Task(name, session), module_(module) {}

} // Tasks
} // Il
