#include "precomp.h"
// @(#)$Id$
//
// Evita Common Io MemoryStream
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MemoryStream.h"

namespace Common {
namespace Io {

MemoryStream::MemoryStream(int const capacity)
    : first_chunk_(new Chunk(capacity)),
      last_chunk_(first_chunk_),
      read_chunk_(first_chunk_),
      read_offset_(0) {
}

MemoryStream::~MemoryStream() {
  auto runner = first_chunk_;
  while (runner) {
    auto next = runner->next();
    delete runner;
    runner = next;
  }
}

int MemoryStream::Read(void* const pv, int const count) {
  ASSERT(pv != nullptr);
  ASSERT(count >= 0);

  if (read_chunk_ == nullptr) {
    return 0;
  }

  auto const start = static_cast<uint8*>(pv);

  auto avail = read_chunk_->Count() - read_offset_;
  auto bytes = read_chunk_->bytes();
  auto dest = start;
  auto rest = count;

  while (rest > 0) {
    if (avail == 0) {
      read_offset_ = 0;
      read_chunk_ = read_chunk_->next();
      if (read_chunk_ == nullptr) {
        break;
      }
      avail = read_chunk_->Count();
      bytes = read_chunk_->bytes();
    }

    auto const copy = min(avail, rest);
    ::CopyMemory(dest, bytes + read_offset_, copy);
    avail -= copy;
    dest += copy;
    read_offset_ += copy;
    rest -= copy;
  }

  return static_cast<int>(dest - start);
}

int MemoryStream::Write(const void* const pv, int const count) {
  ASSERT(pv != nullptr);
  ASSERT(count >= 0);

  if (count == 0) {
    return 0;
  }

  auto runner = static_cast<const uint8*>(pv);
  auto rest = count;
  while (rest > 0) {
    auto const copy = last_chunk_->Add(runner, rest);
    runner += copy;
    rest -= copy;

    if (rest && copy == 0) {
      auto const chunk = new Chunk(rest);
      last_chunk_->set_next(chunk);
      last_chunk_ = chunk;
    }
  }

  return count;
}

}  // Io
} // Common
