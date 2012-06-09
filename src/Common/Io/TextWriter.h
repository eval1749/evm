// @(#)$Id$
//
// Common Io TextWriter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Io_TextWriter_h)
#define INCLUDE_Common_Io_TextWriter_h

#include "../Box.h"
#include "../Collections/Collection_.h"

namespace Common {
namespace Io {

using Common::Collections::Collection_;

class TextWriter : public WithCastable_<TextWriter, Object> {
  CASTABLE_CLASS(TextWriter)

  protected: TextWriter() {}
  public: virtual ~TextWriter() {}

  // [C]
  public: virtual void Close() {}

  // [F]
  public: virtual void Flush() {}

  // [W]
  public: void Write(char);
  public: void Write(char16);
  public: void Write(const String&);
  public: void Write(const String&, const Collection_<Box>&);
  DECLARE_VARDIC_METHODS(public: void Write, const String&, Box);

  public: void WriteLine();
  public: void WriteLine(const String&);
  public: void WriteLine(const String&, const Collection_<Box>&);
  DECLARE_VARDIC_METHODS(public: void WriteLine, const String&, Box);

  protected: virtual void WriteStringImpl(const char16*, size_t) = 0;

  DISALLOW_COPY_AND_ASSIGN(TextWriter);
}; // TextWriter

} // Io
} // Common

#endif // !defined(INCLUDE_Common_Io_TextWriter_h)
