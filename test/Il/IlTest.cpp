#include "precomp.h"
// @(#)$Id$
//
// Evita Test Helper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./IlTest.h"

#include "Il.h"

::std::ostream& PrintChar16Ptr(::std::ostream& os, const char16* str) {
  for (auto s = str; *s; s++) {
    auto const ch = *s;
    if (ch > 0x7F) {
      char buf[10];
      ::wsprintfA(buf, "U+%04X", ch);
      os << buf;
    } else {
      os << static_cast<char>(ch);
    }
 }
 return os;
}

::std::ostream& operator<<(::std::ostream& os, const char16* s) {
  PrintChar16Ptr(os, s);
  return os;
}

::std::ostream& operator<<(::std::ostream& os, const BBlockSet& set) {
  os << "{";
  const char* comma = "";
  foreach (BBlockSet::Enum, members, set) {
    os << comma;
    comma = ", ";
    auto const member = *members;
    os << "B" << member->name();
  }
  os << "}";
  return os;
}

::std::ostream& PrintObject(::std::ostream& os, const Object& obj) {
  return PrintChar16Ptr(os, obj.ToString().value());
}

#define DEFINE_PRINTER2(ns1, ty) \
  namespace ns1 { \
  ::std::ostream& operator<<(::std::ostream& os, const ty& obj) { \
    return PrintObject(os, obj); \
  } }

#define DEFINE_PRINTER3(ns1, ns2, ty) \
  namespace ns1 { namespace ns2 { \
  ::std::ostream& operator<<(::std::ostream& os, const ty& obj) { \
    return PrintObject(os, obj); \
  } }}

#include "./Printers.inc"
