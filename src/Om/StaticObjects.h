// @(#)$Id$
//
// Evita Om - Memory Manager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Om_StaticObjects_h)
#define INCLUDE_Om_StaticObjects_h

#include "./Layout.h"
#include "./MemoryManager.h"

namespace Om {

// TODO 2011-01-30 Move LISP_BASE to platform.
// Exe Start 0x00400000
const intptr_t LISP_BASE = 0x0F000000;

struct StaticBino {
    Mm::Area m_oArea;

    Om::Layout::Float32 m_zero32;
    Om::Layout::Float64 m_zero64;

    Om::Layout::Char mv_char[Arch::CharCodeLimit];

    DISALLOW_COPY_AND_ASSIGN(StaticBino);
}; // StaticBino

#define ROUNDUP(x, y) ( (x + y - 1) / y * y )

const intptr_t BINO_BASE = LISP_BASE;
const intptr_t CHAR_BASE = BINO_BASE + offsetof(StaticBino, mv_char);
const intptr_t BINO_AREA_SIZE = ROUNDUP(sizeof(StaticBino), Arch::AllocUnit);
const intptr_t RECO_BASE = BINO_BASE + BINO_AREA_SIZE;

const intptr_t TYPE_BASE = RECO_BASE;

enum TypeIndex {
    #define DEFCLASS(mp_name, mp_base, mp_meta) \
        TypeIndex_ ## mp_name,

    #include "./StaticClass.inc"
    TypeIndex_Limit,
}; // TypeIndex

#define DEFCLASS(mp_name, mp_base, mp_meta) \
    mp_meta* const Ty_ ## mp_name = reinterpret_cast<mp_meta*>( \
        TYPE_BASE + sizeof(Om::Layout::Type) * TypeIndex_ ## mp_name);

#include "./StaticClass.inc"

const intptr_t STATIC_OBJECT_END =
    TYPE_BASE
    + TypeIndex_Limit * sizeof(Om::Layout::Type);

} // Om

#endif // !defined(INCLUDE_Om_StaticObjects_h)
