#include "precomp.h"
// @(#)$Id$
//
// Evita Cg X86 Register
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

namespace Il {
namespace Cg {

String StackSlot::ToString() const {
  return size() == 4
    ? String::Format("[ESP+%d]", offset())
    : String::Format("[ESP+%d:%d]", offset(), size());
}

String TttnLit::ToString() const {
  static const char* const sTttnNames[] =  {
    "O",    // 0
    "NO",   // 1
    "C",    // 2
    "NC",   // 3
    "E",    // 4
    "NE",   // 5
    "BE",   // 6
    "NBE",  // 7
    "S",    // 8
    "NS",   // 9
    "P",    // 10
    "NP",   // 11
    "L",    // 12
    "GE",   // 13
    "LE",   // 14
    "G",    // 15
  };
  static_assert(ARRAYSIZE(sTttnNames) == 16, "Bad sTttnNames");
  return uint(value_) < ARRAYSIZE(sTttnNames)
    ? sTttnNames[value_]
    : String::Format("TTTN(%d)", value_);
}

} // Cg
} // Il
