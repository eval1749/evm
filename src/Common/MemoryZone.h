// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_MemoryZone_h)
#define INCLUDE_Common_MemoryZone_h

namespace Common {

class MemoryZone {
    public: MemoryZone() {}
    public: virtual ~MemoryZone() {}

    public: virtual void* Alloc(size_t const cb) = 0;
    public: virtual void Free(void* const pv) = 0;
}; // MemoryZone

} // Common

void* operator new[](size_t cb, Common::MemoryZone* pMemoryZone);

#endif // !defined(INCLUDE_Common_MemoryZone_h)
