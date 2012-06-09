// @(#)$Id$
//
// Evita Om - Memory Manager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Win_WinThread_h)
#define INCLUDE_Win_WinThread_h

namespace Om {

class PlatformThread {
    // ctor
    protected: PlatformThread();

    // [G]
    public: static Thread* Get();

    // [S]
    public: static void StaticInit();
}; // PlatformThread

} // Om

#endif // !defined(INCLUDE_Win_WinThread_h)
