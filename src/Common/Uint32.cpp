#include "precomp.h"
// @(#)$Id$
//
// Evita Common - CommonInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./UInt32.h"

#include "./String.h"

namespace Common {

// [T]
String UInt32::ToString() const {
  char sz[100];
  ::wsprintfA(sz, "%u", value_);
  return String(sz);
} // ToString

String UInt32::ToStringWithFormat(
    const char16* const format,
    FormatProvider&) const {
  ASSERT(format != nullptr);
  char sz[100];
  switch (*format) {
    case 'X': case 'x':
      ::wsprintfA(sz, "%x", value_);
      break;

    default:
      ::wsprintfA(sz, "%u", value_);
      break;
  } // switch

  return String(sz);
} // ToString

} // Common
