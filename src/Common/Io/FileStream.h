// @(#)$Id$
//
// Common Io FileStream
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Io_FileStream_h)
#define INCLUDE_Common_Io_FileStream_h

#include "./HandleStream.h"
#include "../String.h"

namespace Common {
namespace Io {

enum FileAccess {
    FileAccess_Read = 1,
    FileAccess_Write = 2,
    FileAccess_ReadWrite = FileAccess_Read | FileAccess_Write,
}; // FileAccess

enum FileMode {
    FileMode_Append = 6,
    FileMode_Create = 2,
    FileMode_CreateNew = 1,
    FileMode_Open = 3,
    FileMode_OpenOrCreate = 4,
    FileMode_Truncate = 5,
}; // FileMode

enum FileShare {
    FileShare_None = 0,
    FileShare_Read = 1,
    FileShare_Write = 2,
    FileShare_ReadWrite = FileShare_Read | FileShare_Write,
}; // FileShare

class FileStream : public WithCastable1_<FileStream, HandleStream, HANDLE> {
  CASTABLE_CLASS(FileStream);

  private: String file_name_;

  // ctor
  public: FileStream(
      const String&,
      FileMode,
      FileAccess,
      FileShare = FileShare_None);

  public: virtual ~FileStream() { Close(); }

  // [G]
  public: const String& file_name() const { return file_name_; }

  DISALLOW_COPY_AND_ASSIGN(FileStream);
}; // FileStream

} // Io
} // Common

#endif // !defined(INCLUDE_Common_Io_FileStream_h)
