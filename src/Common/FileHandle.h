// @(#)$Id$
//
// Evita Common - FileCommon
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_FileHandle_h)
#define INCLUDE_FileHandle_h

namespace Common {

//////////////////////////////////////////////////////////////////////
//
// FileHandle
//
class FileHandle {
    public: HANDLE h;

    // ctor
    public: FileHandle(HANDLE const hFile = INVALID_HANDLE_VALUE) {
        this->h = hFile;
    } // FileHandle

    public: ~FileHandle() {
        this->Release();
    } // ~FileHandle

    public: operator HANDLE() const { return this->h; }

    public: FileHandle& operator =(HANDLE const hHandle) {
        this->Attach(hHandle);
        return *this;
    } // operator =

    // [A]
    public: void Attach(HANDLE const hHandle) {
        this->Release();
        this->h  = hHandle;
    } // Attach

    // [D]
    public: HANDLE Detach() {
        auto const h1 = this->h;
        this->h = INVALID_HANDLE_VALUE;
        return h1;
    } // Detach

    // [I]
    public: bool IsValid() const {
        return this->h != INVALID_HANDLE_VALUE;
    } // IsValie

    // [R]
    public: void Release() {
        if (this->IsValid()) {
            ::CloseHandle(this->h);
            this->h = INVALID_HANDLE_VALUE;
        }
    } // Release
}; // FileHandle

} // Common

#endif //!defined(INCLUDE_FileHandle_h)
