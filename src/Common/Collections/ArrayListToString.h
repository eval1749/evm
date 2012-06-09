// @(#)$Id$
//
// Evita Common Collections_ArrayListToString
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_ArrayListToString_h)
#define INCLUDE_Common_Collections_ArrayListToString_h

#include "./ArrayList_.h"
#include "../StringBuilder.h"

namespace Common {
namespace Collections {

template<typename T, class A>
String ArrayList_<T, A>::ToString() const {
  StringBuilder builder;
  builder.Append('[');
  const char* comma = "";
  foreach (Enum, elems, *this) {
    builder.Append(comma);
    comma = ", ";
    builder.Append(Stringify(elems.Get()));
  }
  builder.Append(']');
  return builder.ToString();
}

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_ArrayListToString_h)
