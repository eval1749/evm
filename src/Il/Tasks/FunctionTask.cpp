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

FunctionTask::FunctionTask(
    const String& name,
    Session& session,
    Module& module)
  : ModuleTask(name, session, module) {}

// [P]
void FunctionTask::ProcessBBlockDefault(BBlock& bblock, Functor& functor) {
  DEBUG_FORMAT("Process %s %s", name(), bblock);
  for (auto& inst: bblock.instructions())
    inst.Apply(&functor);
}

void FunctionTask::ProcessFunctionDefault(Function& fun, Functor& functor) {
  DEBUG_FORMAT("Process %s %s", name(), fun);
  for (auto& bblock: fun.bblocks())
    bblock.Apply(&functor);
} // Process Function

// [S]
void FunctionTask::Start() {
  for (auto& fn: module().functions())
    ProcessFunction(fn);
} // Start

} // Tasks
} // Il
