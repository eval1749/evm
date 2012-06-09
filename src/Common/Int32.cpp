#include "precomp.h"
// @(#)$Id$
//
// Evita Common - CommonInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Int32.h"

#include "./String.h"

namespace Common {

Int32* Int32::Zero;

void LibExport Int32::Init() {
  Zero = new Int32(0);
} // Init

// [T]
String Int32::ToString() const {
  char sz[100];
  ::wsprintfA(sz, "%d", value_);
  return String(sz);
} // ToString

String Int32::ToStringWithFormat(
    const char16* const format,
    FormatProvider&) const {
  ASSERT(format != nullptr);

  char sz[100];
  switch (*format) {
    case 'X': case 'x':
      ::wsprintfA(sz, "%x", value_);
      break;

    default:
      ::wsprintfA(sz, "%d", value_);
      break;
  } // switch

  return String(sz);
} // ToString

} // Common
