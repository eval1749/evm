#include "precomp.h"
// @(#)$Id$
//
// Evita Common - CommonInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)


#include "./CommonDefs.h"

#include "./Boolean.h"
#include "./Char.h"
#include "./GlobalMemoryZone.h"
#include "./Int32.h"
#include "./String.h"

namespace Common {

void Init() {
    GlobalMemoryZone::Init();
    // String::Init should be called before all others initialization
    // for DCHECK.
    String::Init();

    Boolean::Init();
    Char::Init();
    Int32::Init();
} // Init

} // Common
