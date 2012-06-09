// @(#)$Id$
//
// Common Io StringWriter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Io_StringWriter_h)
#define INCLUDE_Common_Io_StringWriter_h

#include "./TextWriter.h"

namespace Common {

class StringBuilder;

namespace Io {

class StringWriter : public WithCastable_<StringWriter, TextWriter> {
  CASTABLE_CLASS(StringWriter);

  private: StringBuilder& builder_;

  // ctor
  public: StringWriter();
  public: virtual ~StringWriter();

  // [C]
  public: virtual void Close() override {}

  // [F]
  public: virtual void Flush() override {}

  // [W]
  protected: virtual void WriteStringImpl(const char16*, size_t) override;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(StringWriter);
}; // StringWriter

} // Io
} // Common

#endif // !defined(INCLUDE_Common_Io_StringWriter_h)
