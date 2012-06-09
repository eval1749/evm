// @(#)$Id$
//
// Evita Common - TextReader
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Io_TextReader_h)
#define INCLUDE_Common_Io_TextReader_h

namespace Common {
namespace Io {

class TextReader {
    public: abstract uint Read(
        char16* const pwchBuffer,
        uint const cwchBuffer) = 0;
}; // TextReader

} // Io
} // Common

#endif // !defined(INCLUDE_Common_Io_TextReader_h)
