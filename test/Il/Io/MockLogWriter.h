// @(#)$Id$
//
// Evita Lexer Checker
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Il_Tasks_MockLogWriter_h)
#define INCLUDE_test_Il_Tasks_MockLogWriter_h

#include "Common/Io/TextWriter.h"
#include "Il/Io/LogWriter.h"

namespace IlTest {

class NullTextWriter
      : public WithCastable_<NullTextWriter, Common::Io::TextWriter> {
  CASTABLE_CLASS(NullTextWriter);
  protected: virtual void WriteStringImpl(const char16*, size_t) override {}
};


class MockLogWriter : public Il::Io::LogWriter {
  private: NullTextWriter text_writer_;

  public: MockLogWriter();
};

} // IlTest

#endif // !defined(INCLUDE_test_Il_Tasks_MockLogWriter_h)
