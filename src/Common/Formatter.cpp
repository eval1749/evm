#include "precomp.h"
// @(#)$Id$
//
// Evita Common - Formatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Formatter.h"
#include "./Collections/Array_.h"
#include "./Collections/ArrayToString.h"
#include "./Collections/ArrayListToString.h"
#include "./Collections/CollectionToString.h"
#include "./Io/StringWriter.h"
#include "./StringBuilder.h"

namespace Common {

using Collections::CollectionV_;
using Io::StringWriter;

String LibExport String::Format(
    const String& format,
    const Collection_<Box>& params) {
  Common::Io::StringWriter writer;
  writer.Write(format, params);
  return writer.ToString();
}

void StringBuilder::AppendFormat(
    const String& fmt,
    const Collection_<Box>& params) {
  StringWriter writer;
  writer.Write(fmt, params);
  Append(writer.ToString());
}

#define REPEATED_MACRO(n) \
  String LibExport String::Format(const String& fmt, PARAMS_DECL_##n(Box)) { \
    return String::Format(fmt, CollectionV_<Box>(PARAMS_##n)); \
  } \
  void StringBuilder::AppendFormat(const String& fmt, PARAMS_DECL_##n(Box)) { \
    StringWriter writer; \
    writer.Write(fmt, PARAMS_##n); \
    Append(writer.ToString()); \
  }

REPEAT_MACRO_9

} // Common
