// @(#)$Id$
//
// Common Io MemoryStream
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Io_MemoryStream_h)
#define INCLUDE_Common_Io_MemoryStream_h

#include "./Stream.h"

namespace Common {
namespace Io {

class MemoryStream : public WithCastable_<MemoryStream, Stream> {
  CASTABLE_CLASS(MemoryStream);

  private: class Chunk {
    private: static int const kAlign = 16;
    private: static int const kMinChunkSize = kAlign * 2;

    private: uint8* const bytes_;
    private: int const capacity_;
    private: Chunk* next_;
    private: int size_;

    // ctor
    public: Chunk(int const capacity)
      : bytes_(new uint8[NormalizeCapaciy(capacity)]),
        capacity_(NormalizeCapaciy(capacity)),
        next_(nullptr),
        size_(0) {
    }

    public: ~Chunk() {
      delete[] bytes_;
    }

    // properties
    public: const uint8* bytes() const { return bytes_; }
    public: Chunk* next() const { return next_; }
    public: void set_next(Chunk* const next) { next_ = next; }

    // [A]
    public: int Add(const void* const pv, int const count) {
      ASSERT(pv != nullptr);
      ASSERT(count >= 0);
      auto const copy = min(count, capacity_ - size_);
      ::CopyMemory(bytes_ + size_, pv, copy);
      size_ += copy;
      return copy;
    }

    // [C]
    public: int Count() const { return size_; }

    // [N]
    private: static int NormalizeCapaciy(int const in) {
      return max(ROUNDUP(in, kAlign), kMinChunkSize);
    }

    DISALLOW_COPY_AND_ASSIGN(Chunk);
  };

  private: static int const kDefaultCapacity = 128;

  private: Chunk* first_chunk_;
  private: Chunk* last_chunk_;
  private: Chunk* read_chunk_;
  private: int read_offset_;

  public: MemoryStream(int = kDefaultCapacity);
  public: virtual ~MemoryStream();

  // [C]
  public: virtual void Close() {}

  // [F]
  public: virtual void Flush() {}

  // [R]
  public: virtual int Read(void*, int) override;

  // [W]
  public: virtual int Write(const void*, int) override;

  DISALLOW_COPY_AND_ASSIGN(MemoryStream);
}; // MemoryStream

} // Io
} // Common

#endif // !defined(INCLUDE_Common_Io_MemoryStream_h)
