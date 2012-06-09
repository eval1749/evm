#include "precomp.h"
// @(#)$Id$
//
// Evita Common Io StreamWriter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./StreamWriter.h"

#include "../String.h"

#include "./FileStream.h"

namespace Common {
namespace Io {

// ctor
StreamWriter::StreamWriter(const String& file_name)
    : char_count_(0),
      is_stream_owner_(true),
      stream_(
        *new FileStream(
            file_name,
            FileMode_Create,
            FileAccess_Write,
            FileShare_Read)) {}

StreamWriter::StreamWriter(Stream& stream)
    : char_count_(0),
      is_stream_owner_(false),
      stream_(stream) {}

StreamWriter::~StreamWriter() {
  if (is_stream_owner_) {
      Close();
      delete &stream_;
  } // if
} // ~StreamWriter

// [C]
void StreamWriter::Close() {
  Flush();

  if (is_stream_owner_) {
      stream_.Close();
  }
}

// [F]
void StreamWriter::Flush() {
  auto pch = chars_;

  while (char_count_ != 0) {
    auto written = stream_.Write(pch, char_count_);
    if (written == 0) {
      char_count_ = 0;
      return;
    }

    char_count_ -= written;
    pch += written;
  }
}

void StreamWriter::WriteStringImpl(
  const char16* const pwchIn,
  size_t const cwchIn) {

  auto pwchRunner = pwchIn;
  size_t cwchRunner = cwchIn;

  while (cwchRunner >= 1) {
    if (char_count_ + 10 > lengthof(chars_)) {
      Flush();
    }

    auto cwch = static_cast<int>(cwchRunner);

    for (;;) {
      auto cch = ::WideCharToMultiByte(
          CP_UTF8,
          0,
          pwchRunner,
          cwch,
          chars_ + char_count_,
          lengthof(chars_) - char_count_,
          nullptr,
          nullptr);
      if (cch >= 1) {
          char_count_  += cch;
          pwchRunner   += cwch;
          cwchRunner -= cwch;
          break;
      }

      auto dwError = ::GetLastError();
      if (ERROR_INSUFFICIENT_BUFFER != dwError) {
        // TODO 2010-12-27 yosi@msn.com Throw IoException.
        __debugbreak();
        #if 0
            error(Qplatform_error,
                Kcode,      MakeUInt(dwError),
                Koperation, Qwrite_string,
                Koperands,  list(stream));
        #endif
      } // if

      cwch -= 1;
    }
  }
}

} // Io
} // Common
