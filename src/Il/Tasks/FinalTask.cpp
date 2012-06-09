#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - FinalTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FinalTask.h"

#include "../Ir.h"

namespace Il {
namespace Tasks {

// ctor
FinalTask::FinalTask(Session& session, Module& module) 
    : Base("FinalTask", session, module) {}

} // Tasks
} // Il
