#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - SourceBuffer
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./SourceBuffer.h"

namespace Compiler {

// ctor
SourceBuffer::SourceBuffer() : state_(STATE_NORMAL) {}

// [A]
void SourceBuffer::Add(const char16* text, size_t text_len) {
  ASSERT(text != nullptr);
  auto text_end = text + text_len;
  for (auto p = text; p < text_end; p++) {
    auto const ch = *p;
    switch (state_) {
      case STATE_CR:
        switch (ch) {
          case '\n':
            FinishLine();
            state_ = STATE_NORMAL;
            break;

          case '\r':
            line_buffer_.Append('\r');
            break;

          default:
            line_buffer_.Append('\r');
            state_ = STATE_NORMAL;
            break;
        }
        break;

      case STATE_NORMAL:
        switch (ch) {
          case '\n':
            FinishLine();
            break;

          case '\r':
           state_ = STATE_CR;
           break;

          default:
            line_buffer_.Append(ch);
        }
        break;

      default:
        CAN_NOT_HAPPEN();
    }
  }
}

// [F]
void SourceBuffer::FinishLine() {
  lines_.Add(line_buffer_.ToString());
  line_buffer_.Clear();
}

void SourceBuffer::Finish() {
  FinishLine();
  state_ = STATE_NORMAL;
}

// [G]
String SourceBuffer::GetLine(int i) const {
  return i < 0 || i >= lines_.Count() ? String() : lines_.Get(i);
}

} // Compiler
