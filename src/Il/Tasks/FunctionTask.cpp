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
  foreach (BBlock::EnumI, insts, bblock) {
    insts.Get()->Apply(&functor);
  } // for bblock
} // Process BBlock

void FunctionTask::ProcessFunctionDefault(Function& fun, Functor& functor) {
  DEBUG_FORMAT("Process %s %s", name(), fun);
  foreach (Function::EnumBBlock, bblocks, fun) {
    bblocks.Get()->Apply(&functor);
  } // for bblock
} // Process Function

// [S]
void FunctionTask::Start() {
  foreach (Module::EnumFunction, oEnum, module()) {
    auto& fn = *oEnum.Get();
    ProcessFunction(fn);
  } // for
} // Start

} // Tasks
} // Il
