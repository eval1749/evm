#include "precomp.h"
// @(#)$Id$
//
// Evita Common - CommonInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Boolean.h"

#include "./String.h"

namespace Common {

Boolean* Boolean::False;
Boolean* Boolean::True;

void LibExport Boolean::Init() {
  False = new Boolean(false);
  True = new Boolean(true);
} // Init

// [T]
String Boolean::ToString() const {
  return String(m_fValue ? "True" : "Flase");
} // ToString

} // Common
