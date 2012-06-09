#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - ErrorInfo
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ErrorInfo.h"

#include "./SourceInfo.h"

namespace Il {
namespace Ir {

// ctor
ErrorInfo::ErrorInfo(
    const SourceInfo& source_info,
    int const error_code,
    const Collection_<Box>& params)
    : error_code_(error_code),
      params_(params),
      source_info_(source_info) {}

// operators
bool ErrorInfo::operator==(const ErrorInfo& r) const {
  if (this == &r) return true;
  return error_code_ == r.error_code_
    && params_ == r.params_
    && source_info_ == r.source_info_;
}

bool ErrorInfo::operator!=(const ErrorInfo& r) const {
  return !operator==(r);
}

bool ErrorInfo::operator<(const ErrorInfo& r) const {
  return !operator>=(r);
}

bool ErrorInfo::operator<=(const ErrorInfo& r) const {
  if (this == &r) return true;
  return error_code_ <= r.error_code_
    && params_ <= r.params_
    && source_info_ <= r.source_info_;
}

bool ErrorInfo::operator>(const ErrorInfo& r) const {
  return !operator<=(r);
}

bool ErrorInfo::operator>=(const ErrorInfo& r) const {
  if (this == &r) return true;
  return error_code_ >= r.error_code_
    && params_ >= r.params_
    && source_info_ >= r.source_info_;
}

// [C]
int ErrorInfo::ComputeHashCode() const {
  int hash_code = error_code();

  hash_code = Common::ComputeHashCode(
    source_info().ComputeHashCode(), hash_code);

  foreach (EnumParam, params, *this) {
    auto const param = params.Get();
    hash_code = Common::ComputeHashCode(param.ComputeHashCode(), hash_code);
  }
  return hash_code;
}

} // Ir
} // Il
