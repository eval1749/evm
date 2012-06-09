#include "precomp.h"
// @(#)$Id$
//
// Common Io FileStream
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FileStream.h"

#include "../String.h"

namespace Common {
namespace Io {

static HANDLE createFile(
    const String& file_name,
    FileMode const file_mode,
    FileAccess const file_access,
    FileShare const file_share) {
  ASSERT(!file_name.IsEmpty());
  auto file_handle = ::CreateFileW(
      file_name.value(),
      file_access,
      file_share,
      nullptr,
      file_mode,
      0,
      nullptr);

  if (file_handle == INVALID_HANDLE_VALUE) {
    #if _DEBUG
      auto const error = ::GetLastError();
      DEBUG_PRINTF("CreateFile(%ls)->%d\n", file_name.value(), error);
    #endif
    __debugbreak();
  }

  return file_handle;
} // createFile

FileStream::FileStream(
    const String& file_name,
    FileMode const file_mode,
    FileAccess const file_access,
    FileShare const file_share)
    : Base(createFile(file_name, file_mode, file_access, file_share)),
      file_name_(file_name) {}

} // Io
} // Common
