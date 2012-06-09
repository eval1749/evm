// @(#)$Id$
//
// Common Io HandleStream
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Io_HandleStream_h)
#define INCLUDE_Common_Io_HandleStream_h

#include "./Stream.h"

namespace Common {
namespace Io {

class HandleStream : public WithCastable_<HandleStream, Stream> {
  CASTABLE_CLASS(HandleStream)

  private: HANDLE handle_;

  public: explicit HandleStream(HANDLE const handle) : handle_(handle) {}

  public: virtual ~HandleStream() {
      this->Close();
  }

  // [C]
  public: virtual void Close() override {
   if (handle_ != INVALID_HANDLE_VALUE) {
     ::CloseHandle(handle_);
     handle_ = INVALID_HANDLE_VALUE;
   }
  }

  // [R]
  public: virtual int Read(
      void* const pvBuffer,
      int const cbBuffer) override {

      ASSERT(nullptr != pvBuffer);
      ASSERT(cbBuffer >= 0);

      DWORD cbRead;
      auto fSucceeded = ::ReadFile(
          handle_,
          pvBuffer,
          cbBuffer,
          &cbRead,
          nullptr);
      if (!fSucceeded) {
        #ifdef _DEBUG
          auto const dwError = ::GetLastError();
          DEBUG_PRINTF("ReadFile: %d", dwError);
        #endif
        return 0;
      }

      return cbRead;
  }

  // [W]
  public: virtual int Write(
    const void* const pvBuffer,
    int const cbWrite) override {

    ASSERT(nullptr != pvBuffer);
    ASSERT(cbWrite >= 0);

    DWORD cbWritten;
    auto fSucceeded = ::WriteFile(
        handle_,
        pvBuffer,
        cbWrite,
        &cbWritten,
        nullptr);
    if (!fSucceeded) {
      #ifdef _DEBUG
        auto const dwError = ::GetLastError();
        DEBUG_PRINTF("WriteFile: %d", dwError);
      #endif
      return 0;
    }

    return cbWritten;
  }

  DISALLOW_COPY_AND_ASSIGN(HandleStream);
}; // HandleStream

} // Io
} // Common

#endif // !defined(INCLUDE_Common_Io_HandleStream_h)
