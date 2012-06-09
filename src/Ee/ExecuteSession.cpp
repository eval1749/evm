#include "precomp.h"
// @(#)$Id$
//
// Evita Executor - ExecuteSession
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ExecuteSession.h"

namespace Executor {

// ctor
ExecuteSession::ExecuteSession(const Il::Cg::CgTarget& target)
    : target_(target) {
  DEBUG_PRINTF("%p\n", this);
}

ExecuteSession::~ExecuteSession() {
  DEBUG_PRINTF("%p\n", this);
  while (!errors_.IsEmpty()) {
    delete errors_.Pop();
  }
}

// [A]
void ExecuteSession::AddErrorInfo(const ErrorInfo& error_info) {
  static bool s_continue = false;

  if (!s_continue && ::IsDebuggerPresent()) {
    __debugbreak();
    s_continue = true;
  } // if

  errors_.Append(&const_cast<ErrorInfo&>(error_info.Clone()));
} // AddError


void ExecuteSession::AddWarning(const ExecuteErrorInfo& error_info) {
  warnings_.Append(&const_cast<ExecuteErrorInfo&>(error_info));
} // AddWarning

} // Executor
