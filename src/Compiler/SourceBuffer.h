// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_SourceBuffer_h)
#define INCLUDE_evc_SourceBuffer_h

namespace Compiler {

class SourceBuffer {
  private: enum State {
    STATE_CR,
    STATE_NORMAL,
  };

  private: StringBuilder line_buffer_;
  private: ArrayList_<String> lines_;
  private: State state_;

  public: SourceBuffer();

  public: void Add(const char16*, size_t);
  private: void FinishLine();
  public: void Finish();
  public: String GetLine(int) const;

  DISALLOW_COPY_AND_ASSIGN(SourceBuffer);
};

} // Compiler

#endif // !defined(INCLUDE_evc_SourceBuffer_h)
