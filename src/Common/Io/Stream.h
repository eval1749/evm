// @(#)$Id$
//
// Common Io Stream
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Io_Stream_h)
#define INCLUDE_Common_Io_Stream_h

#include "../Object.h"

namespace Common {
namespace Io {

class Stream : public WithCastable_<Stream, Object> {
  CASTABLE_CLASS(Stream);

  // ctor
  protected: Stream() {}
  public: virtual ~Stream() {}

  // [C]
  public: virtual void Close() {}

  // [F]
  public: virtual void Flush() {}

  // [R]
  public: virtual int Read(void*, int) = 0;

  // [W]
  public: virtual int Write(const void*, int) = 0;

  DISALLOW_COPY_AND_ASSIGN(Stream);
}; // Stream

} // Io
} // Common

#endif // !defined(INCLUDE_Common_Io_Stream_h)
