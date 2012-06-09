#include "precomp.h"

typedef unsigned char uint8;

#if NDEBUG

int __cdecl my_memcmp(const void* src1, const void* src2, size_t count)
{
    __assume(nullptr != src1);
    __assume(nullptr != src2);

    const uint8* s = reinterpret_cast<const uint8*>(src1);
    const uint8* p = reinterpret_cast<const uint8*>(src2);
    const uint8* e = s + count;
    while (s < e)
    {
        int diff = *s - *p;
        if (0 != diff) return diff;
        s++;
        p++;
    } // while
    return 0;
} // memcmp

void* __cdecl my_memset(void* dst, int c, size_t count)
{
    __assume(nullptr != dst);
    uint8* d = reinterpret_cast<uint8*>(dst);
    uint8* e = d + count;
    while (d < e) *d++ = static_cast<uint8>(c);
    return dst;
} // memset

extern "C" void* __fastcall myFillMemory(void* dst, size_t count, BYTE c)
{
    __assume(nullptr != dst);
    uint8* d = reinterpret_cast<uint8*>(dst);
    uint8* e = d + count;
    switch (count)
    {
    case 0:
        return dst;
    case 3:
        *d++ = c;
        // FALLTHROUGH
    case 2:
        *d++ = c;
        // FALLTHROUGH
    case 1:
        *d = c;
        break;
    default:
    {
        switch (reinterpret_cast<uint>(d) % 4)
        {
        case 3:
            *d++ =c;
            // FALLTHROUGH
        case 2:
            *d++ =c;
            // FALLTHROUGH
        case 1:
            *d++ = c;
            // FALLTHROUGH
        } // switch

        switch (reinterpret_cast<uint>(e) % 4)
        {
        case 3:
            *--e =c;
            // FALLTHROUGH
        case 2:
            *--e =c;
            // FALLTHROUGH
        case 1:
            *--e = c;
            // FALLTHROUGH
        } // switch

        uint32* d32 = reinterpret_cast<uint32*>(d);
        uint32* e32 = reinterpret_cast<uint32*>(e);
        uint32  c32 = (c << 24) | (c << 16) | (c << 8) | c;
        while (d32 < e32) *d32++ = c32;
        break;
    } // default
    } // switch
    return dst;
} // myFillMemory

#if 0
extern "C" void* __fastcall myZeroMemory(void* dst, size_t count)
{
    uint8* d = reinterpret_cast<uint8*>(dst);
    uint8* e = d + count;
    while (d < e) *d++ = 0;
    return dst;
} // myZeroMemory
#else
extern "C" void* __fastcall myZeroMemory(void* dst, size_t count)
{
    return myFillMemory(dst, count, 0);
} // myZeroMemory
#endif

#endif // NDEBUG

#if NDEBUG
void* __cdecl operator new(size_t cb)
    { return ::HeapAlloc(::GetProcessHeap(), 0, cb); }

void __cdecl operator delete(void* pv)
    { ::HeapFree(::GetProcessHeap(), 0, pv); }

extern "C"
{

// Note: For x64, we need to use /OPT:NOREF
extern const int _fltused = 0;

int __cdecl _purecall(void) { return 0; }

typedef unsigned char uint8;

void* __fastcall myCopyMemory(void* dst, const void* src, size_t count)
{
    const uint8* s = reinterpret_cast<const uint8*>(src);
    const uint8* e = s + count;
    uint8* d = reinterpret_cast<uint8*>(dst);
    while (s < e) *d++ = *s++;
    return dst;
} // memcpy

void* __fastcall myMoveMemory(void* dst, const void* src, size_t count)
{
    const uint8* s = reinterpret_cast<const uint8*>(src);
    const uint8* e = s + count;
    uint8* d = reinterpret_cast<uint8*>(dst);
    if (s < d && d < e)
    {
        d += count;
        while (e > s) *--d = *--e;
    }
    else
    {
        while (s < e) *d++ = *s++;
    }
    return dst;
} // myMoveMemory

// TODO 2007-07-28 yosi@msn.com We should remove dependency of LIBCMT.
// It seems GDI+ requires LIBCMT for floating point operation.(?)
#if 0
// See crt/src/crtexe.c
void __cdecl WinMainCRTStartup()
{
    int iRet = WinMain(::GetModuleHandle(nullptr), nullptr, nullptr, 0);
    ::ExitProcess(iRet);
} // WinMainCRTStartup
#endif

} // extern "C"
#endif // NDEBUG
