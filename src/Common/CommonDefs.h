// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Defs_h)
#define INCLUDE_Common_Defs_h

namespace Common {

class Boolean;
class Float32;
class Float64;
class Int32;
class Int64;
class IntPtr;
class MemoryZone;
class String;
class UInt32;
class UInt64;

inline bool IsNotEmpty(const char* const psz) {
    return nullptr != psz && 0 != *psz;
} // IsNotEmpty

inline bool IsNotEmpty(const char16* const pwsz) {
    return nullptr != pwsz && 0 != *pwsz;
} // IsNotEmpty

namespace Io {

class TextWriter;

} // Io

} // Common


#endif // !defined(INCLUDE_Common_Defs_h)
