#include "precomp.h"
// @(#)$Id$
//
// Evita Common - Object
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Object.h"

#include "./String.h"

namespace Common {

String Object::ToString() const {
  char sz[100];
  ::wsprintfA(sz, "%s@%p", GetKind(), this);
  return String(sz);
} // ToString

String Object::ToStringWithFormat(
    const char16* const format,
    FormatProvider&) const {
  ASSERT(format != nullptr);
  return ToString();
}

// Compute Adler32(RFC1950)
int LibExport ComputeHashCode(int const nDatum, int const nSeed) {
  auto const k_nBase = 65521; // largest prime small than 65536
  auto s1 = nSeed & 0xFFFF;
  auto s2 = (nSeed >> 16) & 0xFFFF;
  s1 = (s1 + nDatum) % k_nBase;
  s2 = (s2 + s1) % k_nBase;
  return ((s2 << 16) | s1) & (static_cast<uint>(-1) >> 1);
} // ComputeHashCode

NoReturn void LibExport Error(const char* const pszFormat, ...) {
  va_list args;
  va_start(args, pszFormat);
  char sz[100];
  ::wvsprintfA(sz, pszFormat, args);
  va_end(args);
  ::MessageBoxA(nullptr, sz, "Evita Virtual Machine", 0);
  __debugbreak();
} // Error

} // Common
