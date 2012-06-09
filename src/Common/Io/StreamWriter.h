// @(#)$Id$
//
// Common Io StreamWriter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Io_StreamWriter_h)
#define INCLUDE_Common_Io_StreamWriter_h

#include "./Stream.h"
#include "./TextWriter.h"

namespace Common {
namespace Io {

class Stream;

class StreamWriter : public WithCastable_<StreamWriter, TextWriter> {
  CASTABLE_CLASS(StreamWriter);

  private: enum { BufferSize = 4096 };

  private: int char_count_;
  private: char chars_[BufferSize];
  private: bool is_stream_owner_;
  private: Stream& stream_;

  // ctor
  public: StreamWriter(const String&);
  public: StreamWriter(Stream&);

  public: virtual ~StreamWriter();

  // [C]
  public: virtual void Close() override;

  // [F]
  public: virtual void Flush() override;

  // [W]
  protected: virtual void WriteStringImpl(const char16*, size_t) override;

  DISALLOW_COPY_AND_ASSIGN(StreamWriter);
}; // StreamWriter

} // Io
} // Common

#endif // !defined(INCLUDE_Common_Io_StreamWriter_h)
