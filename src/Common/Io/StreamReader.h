// @(#)$Id$
//
// Evita Common - StreamReader
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Io_StreamReader_h)
#define INCLUDE_Common_Io_StreamReader_h

#include "./TextReader.h"

namespace Common {
namespace Io {

class StreamReader {
    public: static int const k_nBuffer = 1024 * 8;

    private: uint m_cb;;
    private: uint m_cwch;
    private: uint const m_nCodePage;
    private: HANDLE const m_h;
    private: char16* m_pwch;
    private: char m_rgb[k_nBuffer];
    private: char16 m_rgwch[k_nBuffer];

    public: StreamReader(
        HANDLE const hInput,
        uint const nCodePage);

    private: bool fill();

    public: virtual uint Read(
        char16* const pwchBuffer,
        uint const cwchBuffer);

    DISALLOW_COPY_AND_ASSIGN(StreamReader);
}; // StreamReader

} // Io
} // Common

#endif // !defined(INCLUDE_Common_Io_StreamReader_h)
