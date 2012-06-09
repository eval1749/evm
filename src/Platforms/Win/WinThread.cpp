#include "precomp.h"
// @(#)$Id$
//
// Evita Om - Windows Mm
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./WinThread.h"

namespace Om {

DWORD s_dwTlsIndex;

// ctor
PlatformThread::PlatformThread() {
    ::TlsSetValue(s_dwTlsIndex, this);
} // PlatformThread

// [G]
Thread* PlatformThread::Get() {
    return reinterpret_cast<Thread*>(::TlsGetValue(s_dwTlsIndex));
} // Get

// [S]
void PlatformThread::StaticInit() {
    s_dwTlsIndex = ::TlsAlloc();
    if (s_dwTlsIndex == TLS_OUT_OF_INDEXES) {
        auto const dwError = ::GetLastError();
        Debugger::Fail("TlsAlloc %d", dwError);
    } // if
} // StaticInit

} // Om
