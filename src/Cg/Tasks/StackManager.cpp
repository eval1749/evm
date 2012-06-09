#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Cg - Stack Manager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./StackManager.h"

namespace Il {
namespace Cg {

StackManager::AllocationMap::AllocationMap()
    : capacity_(0),
      elements_(nullptr),
      length_(0) {}

StackManager::AllocationMap::~AllocationMap() {
  delete elements_;
}

void StackManager::AllocationMap::ExtendTo(int const new_length) {
  ASSERT(new_length >= length_);
  if (new_length > capacity_) {
    auto const old_elements = elements_;
    capacity_ = RoundUp(new_length, kAllocUnit);
    elements_ = new int[capacity_];
    ::CopyMemory(elements_, old_elements, sizeof(int) * length_);
  }

  length_ = new_length;
}

void StackManager::AllocationMap::Fill(
    int const start,
    int const end,
    int const value) {
  ASSERT(end <= length_);
  ASSERT(start <= length_);
  ASSERT(start <= end);
  for (auto i = start; i < end; i++) {
    elements_[i] = value;
  }
}

int StackManager::AllocationMap::Get(int const index) const {
  ASSERT(index < length_);
  return elements_[index];
}

void StackManager::AllocationMap::Reset() {
  ::ZeroMemory(elements_, sizeof(*elements_) * capacity_);
  length_ = 0;
}

void StackManager::AllocationMap::Set(int const index, int const value) {
  ASSERT(index < length_);
  elements_[index] = value;
}

StackManager::StackManager(CgSession& session) : session_(session) {
  DEBUG_PRINTF("%p\n", this);
}

StackManager::~StackManager() {
  DEBUG_PRINTF("%p\n", this);
}

// [A]
int StackManager::Allocate(int const size, int const align) {
  ASSERT(size > 0);
  ASSERT(align > 0);
  for (auto offset = 0; offset < map_.Count(); offset += align) {
    if (!map_.Get(offset)) {
      auto found = true;
      for (auto i = 0; i < size; i++) {
        if (map_.Get(offset + i) != 0) {
          found = false;
          break;
        }
      }

      if (found) {
        map_.Fill(offset, size, offset + 1);
        return offset;
      }
    }
  }

  auto const offset = RoundUp(map_.Count(), align);
  map_.ExtendTo(offset + size);
  map_.Fill(offset, offset + size, offset + 1);
  map_.Fill(offset + size, map_.Count(), 0);
  return offset;
}

bool StackManager::Assign(int const offset, int const size) {
  ASSERT(offset >= 0);
  ASSERT(size > 0);
  auto const marker = offset + 1;
  auto const end = offset + size;
  if (map_.Count() < end) {
    map_.ExtendTo(end);
  }
  for (int i = offset; i < end; i++) {
    if (map_.Get(i)) {
      return false;
    }
    map_.Set(i, marker);
  }
  return true;
}

void StackManager::Release(int const offset) {
  auto const marker = offset + 1;
  ASSERT(map_.Get(offset) == marker);
  auto i = offset;
  while (map_.Get(i) == marker) {
    map_.Set(i, 0);
  }
}

void StackManager::Reset() {
  map_.Reset();
}

} // Cg
} // Il
