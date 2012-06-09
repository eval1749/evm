#include "precomp.h"
// @(#)$Id$
//
// Evita Executor - ExecuteErrorInfo
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ExecuteErrorInfo.h"

namespace Executor {

namespace {
static const char* const ExecuteErrorNames[] = {
  "NoError",
  #define DEFERROR(category, name) "Ee_" #category "_" #name,
  #include "./ExecuteError.inc"
};
} // namespace

static String ExecuteErrorName(int const err_code) {
  if (err_code > 0 && err_code < ExecuteError_Max_1) {
    return ExecuteErrorNames[err_code];
  }
  return String::Format("Ee_Unknown_%d", err_code);
}

ExecuteErrorInfo::ExecuteErrorInfo(
    const SourceInfo& source_info,
    ExecuteError const error_code,
    const Collection_<Box>& params)
    : Base(source_info, error_code, params) {}

ErrorInfo& ExecuteErrorInfo::Clone() const {
  return *new ExecuteErrorInfo(
      source_info(), static_cast<ExecuteError>(error_code()), params());
}

String ExecuteErrorInfo::ToString() const {
  return String::Format("%s(%s:%s) %s %s",
    source_info().file_name(),
    source_info().line() + 1,
    source_info().column(),
    ExecuteErrorName(error_code()),
    params());
}

} // Executor

