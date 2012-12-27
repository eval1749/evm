#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - AbstractParseTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ErrorItem.h"
#include "Compiler/CompileError.h"
#include "../Il/IlTest.h"

namespace CompilerTest {

using namespace Compiler;

namespace {
static const char* error_namev[] = {
  #define DEFERROR(mp_cat, mp_name) #mp_cat "_" #mp_name,
  #include "Compiler/CompileError.inc"
};

static String ComputeErrorCode(const ErrorInfo& error) {
  auto const error_code = error.error_code();
  auto const name_index = error_code - CompileError_Min;
  return name_index >= 0 && name_index < ARRAYSIZE(error_namev)
      ? String(error_namev[name_index])
      : String::Format("%d", error_code);
}
} // namespace

ErrorItem::ErrorItem(const String& code, int line, int column)
    : column_(column), code_(code), line_(line) {}

ErrorItem::ErrorItem(
    const String& code,
    int line,
    int column,
    Box a)
    : column_(column), code_(code), line_(line),
      params_(CollectionV_<Box>(a)) {}

ErrorItem::ErrorItem(
    const String& code,
    int line,
    int column,
    Box a,
    Box b)
    : column_(column), code_(code), line_(line),
      params_(CollectionV_<Box>(a, b)) {}

ErrorItem::ErrorItem(const ErrorItem& r)
    : column_(r.column_), code_(r.code_), line_(r.line_),
      params_(r.params_) {}

ErrorItem::ErrorItem(const ErrorInfo& error)
    : column_(error.source_info().column()),
      code_(ComputeErrorCode(error)),
      line_(error.source_info().line()),
      params_(error.params()) {}

ErrorItem::ErrorItem() : column_(0), line_(0) {}

ErrorItem& ErrorItem::operator =(const ErrorItem& r) {
  column_ = r.column_;
  code_ = r.code_;
  line_ = r.line_;
  params_ = r.params_;
  return *this;
}

bool ErrorItem::operator ==(const ErrorItem& r) const {
  if (column_ != r.column_ || code_ != r.code_ || line_ != r.line_) {
    return false;
  }

  if (params_.Count() != r.params_.Count()) {
    return false;
  }

  Collection_<Box>::Enum enum_this(params_);
  Collection_<Box>::Enum enum_that(r.params_);
  while (!enum_this.AtEnd() && !enum_that.AtEnd()) {
    if (enum_this.Get().ToString() != enum_that.Get().ToString()) {
      return false;
    }
    enum_this.Next();
    enum_that.Next();
  }
  return enum_this.AtEnd() && enum_that.AtEnd();
}

bool ErrorItem::operator !=(const ErrorItem& r) const {
  return !operator ==(r);
}


bool ErrorItem::operator ==(const ErrorList& list) const {
  return list == *this;
}

bool ErrorItem::operator !=(const ErrorList& r) const {
  return !operator ==(r);
}

String ErrorItem::ToString() const {
  return String::Format("(%s %d %d %s)", code_, line_, column_, params_);
}

::std::ostream& operator <<(::std::ostream& os, const ErrorItem& r) {
  return os << r.ToString();
}

ErrorList::ErrorList(const ErrorList& r) {
  for (auto& item: r.items_) {
    items_.Add(item);
  }
}

bool ErrorList::operator ==(const ErrorList& r) const {
  return items_ == r.items_;
}

bool ErrorList::operator ==(const ErrorItem& item) const {
  return items_.Count() == 1 && items_.Get(0) == item;
}

void ErrorList::Add(const ErrorItem& item) {
  items_.Add(item);
}

String ErrorList::ToString() const {
  StringBuilder builder;
  builder.Append("[");
  const char* space = "";
  for (auto& item: items_) {
    builder.Append(space);
    space = " ";
    builder.Append(item.ToString());
  }
  builder.Append("]");
  return builder.ToString();
}

::std::ostream& operator <<(::std::ostream& os, const ErrorList& r) {
  return os << r.ToString();
}

} // CompilerTest
