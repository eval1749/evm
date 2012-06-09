// @(#)$Id$
//
// Evita Il - Fasl - BitWriter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Il_Fasl_BitWriter_h)
#define INCLUDE_Evita_Il_Fasl_BitWriter_h

#include "../../Common/Io.h"

namespace Il {
namespace Fasl {

using Common::Io::Stream;

class BitWriter {
  // Collect 8bit.
  private: class BitBuffer {
    private: int bits_;
    private: int num_bits_;

    public: BitBuffer() : bits_(0), num_bits_(0) {}

    // [A]
    public: void Add(int const bit) {
      ASSERT(bit == 0 || bit == 1);
      ASSERT(num_bits_ >= 0 && num_bits_ < 8);
      bits_ <<= 1;
      bits_ |= bit;
      num_bits_++;
    }

    // [F]
    public: void Fill() {
      if (num_bits_ < 8) {
        bits_ <<= 8 - num_bits_;
        num_bits_ = 8;
      }
    }

    public: void Flush() {
      bits_ = 0;
      num_bits_ = 0;
    }

    // [G]
    public: uint8 Get() const {
      ASSERT(num_bits_ == 8);
      return static_cast<uint8>(bits_);
    }

    // [I]
    public: bool IsEmpty() const { return num_bits_ == 0; }
    public: bool IsFull() const { return num_bits_ == 8; }

    DISALLOW_COPY_AND_ASSIGN(BitBuffer);
  };

  // Collect byte.
  class ByteBuffer {
    private: uint8* const bytes_;
    private: int const capacity_;
    private: int size_;

    public: ByteBuffer(int const capacity) :
        bytes_(new uint8[capacity]),
        capacity_(capacity),
        size_(0) {
    }

    public: ~ByteBuffer() { delete[] bytes_; }

    // [A]
    public: void Add(uint u32) {
      ASSERT((u32 & ~0xFF) == 0);
      ASSERT(!IsFull());
      bytes_[size_] = static_cast<uint8>(u32);
      size_++;
    }

    // [C]
    public: int Count() const { return size_; }

    // [F]
    public: void Flush(Stream& stream) {
      if (size_ > 0) {
        stream.Write(bytes_, size_);
        size_ = 0;
      }
    }

    // [I]
    public: bool IsFull() const {
      ASSERT(size_ >= 0 && size_ <= capacity_);
      return size_ == capacity_;
    }

    DISALLOW_COPY_AND_ASSIGN(ByteBuffer);
  };

  private: static int const kDefaultBufferSize = 8 * 1024;

  private: Stream& base_stream_;
  private: BitBuffer bit_buffer_;
  private: ByteBuffer byte_buffer_;
  private: int total_bits_;

  // ctor
  public: BitWriter(
      Stream& base_stream,
      int const capacity = kDefaultBufferSize)
      : base_stream_(base_stream),
        byte_buffer_(capacity),
        total_bits_(0) {
  }

  // properties
  public: int total_bits() const { return total_bits_; }

  // [F]
  public: void Flush() {
    if (!bit_buffer_.IsEmpty()) {
      bit_buffer_.Fill();
      FlushBits();
    } // if

    FlushBytes();
    base_stream_.Flush();
  }

  private: void FlushBits() {
    byte_buffer_.Add(bit_buffer_.Get());
    bit_buffer_.Flush();
    if (byte_buffer_.IsFull()) {
        FlushBytes();
    } // if
  }

  private: void FlushBytes() {
    ASSERT(bit_buffer_.IsEmpty());
    byte_buffer_.Flush(base_stream_);
  }

  // [W]
  public: void Write1(int const i32) { WriteBits(i32, 1); }
  public: void Write2(int const i32) { WriteBits(i32, 2); }
  public: void Write3(int const i32) { WriteBits(i32, 3); }
  public: void Write4(int const i32) { WriteBits(i32, 4); }
  public: void Write5(int const i32) { WriteBits(i32, 5); }
  public: void Write6(int const i32) { WriteBits(i32, 6); }
  public: void Write7(int const i32) { WriteBits(i32, 7); }
  public: void Write8(int const i32) { WriteBits(i32, 8); }

  private: void WriteBits(int const i32, int const num_bits) {
    ASSERT(i32 >= 0);
    ASSERT(num_bits > 0 && num_bits <= 8);
    auto runner = 1 << (num_bits - 1);
    while (runner != 0) {
      if ((i32 & runner) != 0) {
        bit_buffer_.Add(1);
      } else {
        bit_buffer_.Add(0);
      }

      total_bits_++;

      if (bit_buffer_.IsFull()) {
        FlushBits();
      } // if
      runner >>= 1;
    } // while
  }

  DISALLOW_COPY_AND_ASSIGN(BitWriter);
}; // BitWriter

} // Fasl
} // Il

#endif // !defined(INCLUDE_Evita_Il_Fasl_BitWriter_h)
