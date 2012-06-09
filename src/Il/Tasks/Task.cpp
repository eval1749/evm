#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Module
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Task.h"

namespace Il {
namespace Tasks {

Task::Task(const String& name, Session& session) 
    : Tasklet(name, session) {}

} // Tasks
} // Il
