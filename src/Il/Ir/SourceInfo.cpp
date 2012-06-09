#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - SourceInfo
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./SourceInfo.h"

namespace Il {
namespace Ir {

// ctor
SourceInfo::SourceInfo() : column_(0), line_(0) {}

SourceInfo::SourceInfo(
    const String& file_name,
    int const line,
    int const column)
    : column_(column),
      line_(line),
      name_(file_name) {}

// operators
bool SourceInfo::operator==(const SourceInfo& r) const {
  if (this == &r) return true;
  return
    column_ == r.column_
    && line_ == r.line_
    && name_ == r.name_;
}

bool SourceInfo::operator!=(const SourceInfo& r) const {
  return !operator==(r);
}

bool SourceInfo::operator<(const SourceInfo& r) const {
  return !operator>=(r);
}

bool SourceInfo::operator<=(const SourceInfo& r) const {
  if (this == &r) return true;
  if (auto const diff = name_.CompareTo(r.name_)) {
    return diff < 0;
  }
  if (auto const diff = line_ - r.line_) { return diff < 0; }
  return column_ <= r.column_;
}

bool SourceInfo::operator>(const SourceInfo& r) const {
  return !operator<=(r);
}

bool SourceInfo::operator>=(const SourceInfo& r) const {
  if (this == &r) return true;
  if (auto const diff = name_.CompareTo(r.name_)) { 
    return diff > 0;
  }
  if (auto const diff = line_ - r.line_) { return diff > 0; }
  return column_ >= r.column_;
}


// [C]
int SourceInfo::ComputeHashCode() const {
  auto hash_code = column();
  hash_code = Common::ComputeHashCode(line(), hash_code);
  hash_code = Common::ComputeHashCode(name_.ComputeHashCode(), hash_code);
  return hash_code;
}

// [E]
bool SourceInfo::Equals(const SourceInfo& that) const {
  return *this == that;
}

} // Ir
} // Il
