//////////////////////////////////////////////////////////////////////////////
//
// Common Debug Utility
// z_debug.h
//
// Copyright (C) 2007 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// @(#)$Id: //proj/evedit2/mainline/tinycl/z_debug.h#3 $
//
#if !defined(INCLUDE_common_debugger_h)
#define INCLUDE_common_debugger_h

#if _DEBUG
    #define ASSERT(mp_expr) \
    { \
        Debugger::Assert( \
            __FILE__, __LINE__, __FUNCTION__, #mp_expr, mp_expr ); \
        __assume(mp_expr); \
    } // ASSERT

    #define DEBUG_PRINTF(mp_fmt, ...) \
        CPRINTF(mp_fmt, __VA_ARGS__)
#else
    #define ASSERT(mp_expr) __assume(mp_expr)
    #define DEBUG_PRINTF(mp_fmt, ...) __noop(__VA_ARGS__)
#endif

#define CAN_NOT_HAPPEN() \
    Debugger::CanNotHappen(__FILE__, __LINE__, __FUNCTION__)

#define CASSERT(mp_expr) \
    typedef char CASSERT_name(__COUNTER__)[(mp_expr) ? 1 : -1]

#define CASSERT_name(x) CASSERT_conc(macro_CASSERT, x)
#define CASSERT_conc(x,y) x ## y

#define CPRINTF(mp_fmt, ...) \
    { \
        Debugger::Printf("%u: %hs(%d): ", \
            ::GetTickCount(), __FUNCTION__, __LINE__); \
        Debugger::Printf(mp_fmt, __VA_ARGS__); \
    } // CPRIHNTF

namespace Debugger {
void __fastcall Assert(const char*, int, const char*, const char*, bool);
void __declspec(noreturn) __fastcall CanNotHappen(
    const char*, int, const char*);
void __declspec(noreturn) __fastcall Fail(const char*, ...);
void __fastcall Printf(const char*, ...);
} // Debugger

#endif //!defined(INCLUDE_common_debugger_h)
