#include "precomp.h"
// @(#)$Id$
//
// Evita Test Helper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include <ostream>

namespace Common {

static ::std::ostream& PrintChar16Ptr(::std::ostream& os, const char16* str) {
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

::std::ostream& operator<<(::std::ostream& os, const Object& obj) {
  return PrintChar16Ptr(os, obj.ToString().value());
}

::std::ostream& operator<<(::std::ostream& os, const String& obj) {
  return PrintChar16Ptr(os, obj.ToString().value());
}
} // Common
