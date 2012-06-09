#include "precomp.h"
// @(#)$Id$
//
// Evita Om - MemoryManager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MemoryManager.h"

#include "./GcAnchor.h"

namespace Om {

size_t          Mm::sm_cbAllocSoFar;
Mm::Area*       Mm::sm_pEmptyArea;
Mm::Areas*      Mm::sm_pFreeAreas;
Mm::Area*       Mm::sm_pCommit;
Mm::Area*       Mm::sm_pEnd;
Mm::Area*       Mm::sm_pStart;

static uint8 s_rgbArea[sizeof(Mm::Area)];

// [A]
void* Mm::Area::Alloc(size_t const cbData) {
    auto const ofsNext = m_ofsFree + cbData;

    if (ofsNext > m_cbArea) {
        return nullptr;
    } // if

    auto const pv = reinterpret_cast<void*>(ToInt() + m_ofsFree);
    m_ofsFree = ofsNext;
    return pv;
} // Mm::Area::Alloc

// [S]
void Mm::StaticInit() {
    sm_pEmptyArea = new(s_rgbArea) Area();
    sm_pFreeAreas = new Areas();
    GcAnchor::StaticInit();
} // StaticInit

} // Om
