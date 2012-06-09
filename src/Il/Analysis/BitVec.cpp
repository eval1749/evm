#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Data Flow Analysis
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./BitVec.h"

namespace Il {
namespace Ir {

BitVec::BitVec(int const count)
    : capacity_(CountWord(count)),
      count_(count),
      bits_(capacity_ ? new uint32[capacity_] : nullptr) {
  ASSERT(count >= 0);
  ASSERT(count_ == 0 || capacity_ > 0);
}

BitVec::~BitVec() {
  delete[] bits_;
}

// [A]
BitVec& BitVec::Adjust(int const new_count) {
  auto const new_capacity = CountWord(new_count);
  if (new_capacity <= capacity_) {
    count_ = new_count;
    return *this;
  }

  auto const old_bits = bits_;
  bits_ = new uint32[new_capacity];
  ::ZeroMemory(bits_, new_capacity);
  ::CopyMemory(bits_, old_bits, capacity_);
  capacity_ = new_capacity;
  count_ = new_count;
  delete[] old_bits;

  return *this;
} // Adjust

BitVec& BitVec::AndC2(const BitVec& that) {
  class AndC2 {
    public: static uint32 Compute(uint32 const a, uint32 const b) {
      return a & ~b;
    }
  };

  return Compute<AndC2>(that);
}

// [C]
template<class Operator> BitVec& BitVec::Compute(const BitVec& that) {
  ASSERT(Count() == that.Count());
  auto const num_words = CountWord(count_);
  for (int w = 0; w < num_words; ++w) {
    bits_[w] = Operator::Compute(bits_[w], that.bits_[w]);
  }
  return *this;
}

BitVec& BitVec::Copy(const BitVec& that) {
  ::CopyMemory(bits_, that.bits_, CountWord(count_) * BitsInWord / 8);
  return *this;
}

// [E]
bool BitVec::Equals(const BitVec& that) const {
  if (Count() != that.Count()) {
      return false;
  }

  auto const num_words = CountWord(count_);
  for (int i = 0; i < num_words - 1; ++i) {
    if (bits_[i] != that.bits_[i]) {
      return false;
    }
  }

  auto const k = count_ % BitsInWord;
  auto const m = k ? (1 << k) - 1 : static_cast<int>(-1);

  return (bits_[num_words - 1] & m)
            == (that.bits_[num_words - 1] & m);
}

// [F]
BitVec& BitVec::FillOne() {
  auto const num_words = CountWord(count_);
  for (int w = 0; w < num_words; ++w) {
    bits_[w] = static_cast<uint32>(-1);
  } // for w

  auto const k = count_ % BitsInWord;
  auto const m = k ? (1 << k) - 1 : static_cast<int>(-1);
  bits_[num_words - 1] &= ~m;
  return *this;
}

BitVec& BitVec::FillZero() {
  auto const num_words = CountWord(count_);
  for (int w = 0; w < num_words; ++w) {
    bits_[w] = 0;
  } // for w
  return *this;
} // FillZero

int BitVec::FindOne() const {
  for (int k = 0; k < count_; ++k) {
    if (IsOne(k)) {
        return k;
    }
  } // for
  return -1;
}

int BitVec::FindLastOne() const {
  int k = count_;
  while (k > 0) {
    k -= 1;
    if (IsOne(k)) {
      return k;
   }
  }

  return -1;
}

// [H]
int BitVec::Hash() const {
  auto const num_words = CountWord(count_);
  auto hash_code = count_;
  for (int i = 0; i < num_words; ++i) {
    hash_code = RotateLeft(hash_code, 5);
    hash_code ^= bits_[i];
 } // for
 return hash_code < 0 ? -hash_code : hash_code;
}

// [I]
BitVec& BitVec::Ior(const BitVec& that) {
  class Ior {
    public: static uint32 Compute(uint32 const a, uint32 const b) {
      return a | b;
    }
  };
  return Compute<Ior>(that);
}

bool BitVec::IsOne(int const k) const {
  return !IsZero(k);
}

bool BitVec::IsZero(int const k) const {
  ASSERT(k < count_);
  auto const w = k / BitsInWord;
  auto const b = k % BitsInWord;
  return (bits_[w] & (1 << b)) == 0;
}

// [S]
void BitVec::SetOne(int const k) {
  ASSERT(k < count_);
  auto const w = k / BitsInWord;
  auto const b = k % BitsInWord;
  bits_[w] |= 1 << b;
} // SetOne

void BitVec::SetZero(int const k) {
  auto const w = k / BitsInWord;
  auto const b = k % BitsInWord;
  bits_[w] &= ~(1 << b);
} // SetZero

} // Ir
} // Il
