//////////////////////////////////////////////////////////////////////////////
//
// VC Support
// vcsupport.h
//
// Copyright (C) 2007 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// @(#)$Id: //proj/evedit2/mainline/tinycl/vcsupport.h#1 $
//
#if !defined(INCLUDE_vcsupport_h)
#define INCLUDE_vcsupport_h

#if NDEBUG

#undef CopyMemory
#define CopyMemory(d, s, c) myCopyMemory(d, s, c)

#undef FillMemory
#define FillMemory(d, c, k) myFillMemory(d, c, k)

#undef ZeroMemory
#define ZeroMemory(d, k) myZeroMemory(d, k)

// When we use owr own version of ZeroMermoy, VC2005 uses internal memset
// and we can't compile my_memset.
//#undef ZeroMemory
//#define ZeroMemory(d, c) myZeroMemory(d, c)

// Our version of memxxx decrease EXE size about 8KB.
// Only references in LIBCMT are __chkstk and __alloca_probe.
#pragma function(memcmp)
#if _M_IX86
    #pragma function(memcpy)
    #pragma function(memset)
#endif // _M_IX86

#define my_memcmp     memcmp
#define my_memcpy     memcpy
#define my_memmove    memmove
#define my_memset     memset

//#define memmove(d, s, k) myMoveMemory(d, s, k)

extern "C"
{
    void* __fastcall myCopyMemory(void*, const void*, size_t);
    void* __fastcall myFillMemory(void*, size_t, BYTE);
    void* __fastcall myMoveMemory(void*, const void*, size_t);
    void* __fastcall myZeroMemory(void*, size_t);
} // extern "C"

#endif // NDEBUG

#endif //!defined(INCLUDE_vcsupport_h)
