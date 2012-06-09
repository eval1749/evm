#include "precomp.h"
// @(#)$Id$
//
// Evita Common - IntPtr
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./IntPtr.h"

#include "./String.h"

namespace Common {

String IntPtr::ToStringWithFormat(
    const char16* const format,
    FormatProvider&)  const {
  ASSERT(format != nullptr);
  char sz[100];
  switch (*format) {
    case 'X': case 'x':
      ::wsprintfA(sz, "%lx", value_);
      break;

    default:
      ::wsprintfA(sz, "%ld", value_);
      break;
  } // switch

  return String(sz);
} // ToString

} // Common
