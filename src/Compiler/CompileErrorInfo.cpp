#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - CompileSession
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CompileErrorInfo.h"

namespace Compiler {

namespace {
static const char* const k_rgpszCompileError[] = {
  "NoError",

  #define DEFERROR(mp_cat, mp_name) \
      #mp_cat "_" #mp_name,

  #include "./CompileError.inc"
}; // k_rgpszCompileError

static const char* const k_rgpszVmError[] = {
  "NoError",

  #define DEFERROR(mp_cat, mp_name) \
      "Il_" #mp_cat "_" #mp_name,

  #include "../Il/VmError.inc"
}; // k_rgpszVmError

static String ComputeErrorName(int const err_code) {
  if (err_code > VmError_Base && err_code < VmError_Max_1) {
    return k_rgpszVmError[err_code - VmError_Base];
  }

  if (err_code > CompileError_Base && err_code < CompileError_Max_1) {
    return k_rgpszCompileError[err_code - CompileError_Base];
  }

  return String::Format("Unknown_Error_%d", err_code);
}
} // namespace

CompileErrorInfo::CompileErrorInfo(
    const SourceInfo& source_info,
    CompileError const error_code,
    const Collection_<Box>& params)
    : Base(source_info, error_code, params) {}

// [C]
ErrorInfo& CompileErrorInfo::Clone() const {
  return *new CompileErrorInfo(
      source_info(), 
      static_cast<CompileError>(error_code()), 
      params());
}

// [T]
String CompileErrorInfo::ToString() const {
  return String::Format("%s(%s:%s) %s %s",
    source_info().file_name(),
    source_info().line() + 1,
    source_info().column(),
    ComputeErrorName(error_code()),
    params());
}

} // Compiler
