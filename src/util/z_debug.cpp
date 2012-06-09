#include "precomp.h"
//////////////////////////////////////////////////////////////////////////////
//
// Common Debug Utility
// z_debug.cpp
//
// Copyright (C) 2007 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// @(#)$Id: //proj/evedit2/mainline/tinycl/z_debug.cpp#1 $
//
#include "./z_debug.h"

namespace Debugger {

void __fastcall Assert(
    const char* pszFile,
    int iLine,
    const char* pszFunc,
    const char* pszExpr,
    bool fExpr ) {

    if (fExpr) {
        return;
    } // if

    char sz[1024];
    ::wsprintfA(sz,
        "Assertion Failed!\n"
        "  File: %hs\n"
        "  Line: %d\n"
        "  Function: %hs\n"
        "  Expression: %hs\n",
        pszFile,
        iLine,
        pszFunc,
        pszExpr );
    Fail(sz);
} // Assert

void __declspec(noreturn) __fastcall CanNotHappen(
    const char* pszFile,
    int iLine,
    const char* pszFunc ) {
    char sz[1024];
    ::wsprintfA(sz,
        "Can't happen!\n"
        "  File:     %hs\n"
        "  Line:     %d\n"
        "  Function: %hs\n",
        pszFile,
        iLine,
        pszFunc );
    Fail(sz);
} // CanNotHappen

void __declspec(noreturn) __fastcall Fail(const char* psz, ...) {
    va_list args;
    va_start(args, psz);
    char sz[1024];
    ::wvsprintfA(sz, psz, args);
    va_end(args);
    ::MessageBoxA(nullptr, sz, "Tiny Common Lisp", MB_ICONERROR);
    __debugbreak();
} // Fail

void __fastcall Printf(const char* psz, ...) {
  if (!::IsDebuggerPresent()) return;
  va_list args;
  va_start(args, psz);
  char sz[1024];
  ::wvsprintfA(sz, psz, args);
  va_end(args);
  ::OutputDebugStringA(sz);
}

} // Debugger
