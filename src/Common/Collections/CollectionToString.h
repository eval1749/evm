// @(#)$Id$
//
// Evita Common Collections_CollectionToString
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_CollectionToString_h)
#define INCLUDE_Common_Collections_CollectionToString_h

#include "./Collection_.h"
#include "../StringBuilder.h"

namespace Common {
namespace Collections {

template<typename T, class A>
String Collection_<T, A>::ToString() const {
  StringBuilder builder;
  builder.Append('{');
  const char* comma = "";
  foreach (Enum, elems, *this) {
    builder.Append(comma);
    comma = ", ";
    builder.Append(Stringify(elems.Get()));
  }
  builder.Append('}');
  return builder.ToString();
}

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_CollectionToString_h)
