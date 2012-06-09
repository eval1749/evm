// @(#)$Id$
//
// Evita Il - Fasl - BitReader
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Il_Fasl_BitReader_h)
#define INCLUDE_Evita_Il_Fasl_BitReader_h

#include "../../Common/Io.h"

namespace Il {
namespace Fasl {

using Common::Io::Stream;

class BitReader {
  private: int bit_position_;
  private: const uint8* buffer_;
  private: int buffer_index_;
  private: int buffer_len_;

  // Read bits
  private: int read_bit_mask_;
  private: int read_bits_;

  // Result bits accumulator
  private: int write_bits_;
  private: int write_bits_count;
  private: int write_bits_mask_;

  // ctor
  public: BitReader()
    : bit_position_(0),
      buffer_(nullptr),
      buffer_index_(0),
      buffer_len_(0),
      read_bit_mask_(0),
      read_bits_(0),
      write_bits_count(0),
      write_bits_(0),
      write_bits_mask_(0) {}

  // properties
  public: int bit_position() const { return bit_position_; }

  // [A]
  public: void Align() {
    ASSERT(write_bits_count == 0);
    if (read_bit_mask_) {
      read_bit_mask_ = 0;
    }
  }

  // [F]
  public: void Feed(const void* buffer, int const buffer_len) {
    ASSERT(buffer != nullptr);
    ASSERT(buffer_len >= 0);
    // We've finsihed process all bytes.
    ASSERT(buffer_index_ == buffer_len_);
    buffer_ = reinterpret_cast<const uint8*>(buffer);
    buffer_len_ = buffer_len;
    buffer_index_ = 0;
  }

  private: bool FillBits() {
    ASSERT(read_bit_mask_ == 0);

    if (buffer_index_ == buffer_len_) {
      return false;
    }

    ASSERT(buffer_index_ < buffer_len_);
    read_bits_ = buffer_[buffer_index_];
    read_bit_mask_ = 1 << 7;
    buffer_index_++;

    return true;
  }

  // [R]
  public: int Read(int const num_bits) {
    ASSERT(num_bits > 0 && num_bits <= 8);

    if (write_bits_count == 0) {
      write_bits_ = 0;
      write_bits_mask_ = 1 << (num_bits - 1);
    } // if

    while (write_bits_mask_ != 0) {
      if (read_bit_mask_ == 0) {
        if (!FillBits()) {
          return -1;
        }
      }

      if ((read_bits_ & read_bit_mask_) != 0) {
        write_bits_ |= write_bits_mask_;
      }

      read_bit_mask_ >>= 1;
      write_bits_mask_ >>= 1;
      ++write_bits_count;
      ++bit_position_;
    }

    if (write_bits_count < num_bits) {
      return -1;
    }

    write_bits_count = 0;
    return write_bits_;
  }

  DISALLOW_COPY_AND_ASSIGN(BitReader);
}; // BitReader

} // Fasl
} // Il

#endif // !defined(INCLUDE_Evita_Il_Fasl_BitReader_h)
