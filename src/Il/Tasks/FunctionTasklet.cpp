#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Tasklet - FunctionTasklet
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FunctionTasklet.h"

namespace Il {
namespace Tasks {

// ctor
FunctionTasklet::FunctionTasklet(const String& name, Session& session)
    : Tasklet(name, session) {}

} // Tasks
} // Il
