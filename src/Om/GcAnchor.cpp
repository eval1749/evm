#include "precomp.h"
// @(#)$Id$
//
// Evita Om - GcAnchor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./GcAnchor.h"

namespace Om {

static int s_cLocks;
static GcAnchor::List* s_pGcAnchors;

// [G]
GcAnchor::List* GcAnchor::Get() {
    ASSERT(nullptr != s_pGcAnchors);
    return s_pGcAnchors;
} // Get

// [L]
void GcAnchor::Lock() {
    s_cLocks++;
} // Lock

// [S]
void GcAnchor::StaticInit() {
    s_pGcAnchors = new GcAnchor::List();
} // StaticInit


// [L]
void GcAnchor::Unlock() {
    ASSERT(s_cLocks > 0);
    s_cLocks--;
} // Unlock

} // Om
