#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Modifier
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

namespace Il {
namespace Ir {

String ModifiersToString(int modifiers) {
  StringBuilder builder;
  #define DEFMODIFIER(Name, name, bit_posn) \
    if ((modifiers & (1 << bit_posn)) != 0) { builder.Append(#name " ");  }

  #include "../../Il/Ir/Modifiers.inc"
  return builder.ToString();
}

} // Ir
} // Il
