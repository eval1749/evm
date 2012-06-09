#include "precomp.h"
// @(#)$Id$
//
// Evita Common Io StreamWriter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./StringWriter.h"

#include "../StringBuilder.h"

namespace Common {
namespace Io {

StringWriter::StringWriter() : builder_(*new StringBuilder()) {}

StringWriter::~StringWriter() {
  delete &builder_;
} // StringWriter

void StringWriter::WriteStringImpl(
    const char16* const pwch, size_t const cwch) {
  builder_.Append(pwch, cwch);
} // WriteString

String StringWriter::ToString() const {
  return builder_.ToString();
} // ToString

} // Io
} // Common
