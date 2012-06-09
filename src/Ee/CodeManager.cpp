#include "precomp.h"
// @(#)$Id$
//
// Evita Ee - CodeManager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CodeManager.h"

namespace Ee {

bool CodeManager::CodeDescBox::operator ==(const Address& r) const {
  return r.value_ >= code_min() && r.value_ < code_max();
}

bool CodeManager::CodeDescBox::operator ==(
    const CodeManager::CodeDescBox& r) const {
  return code_min() == r.code_min();
}

bool CodeManager::CodeDescBox::operator <(const Address& r) const {
  return code_min() < r.value_;
}

bool CodeManager::CodeDescBox::operator <(
    const CodeManager::CodeDescBox& r) const {
  return code_min() < r.code_min();
}

uintptr_t CodeManager::CodeDescBox::code_max() const {
  return code_min() + code_desc_->code_size();
}

uintptr_t CodeManager::CodeDescBox::code_min() const {
  return reinterpret_cast<uintptr_t>(code_desc_->codes());
}

CodeManager::CodeHeap::Block::Block(Block* const prev, size_t size)
    : curr_(sizeof(Block)),
      end_(static_cast<int32>(size)),
      next_(nullptr),
      prev_(prev) {}

void* CodeManager::CodeHeap::Block::Alloc(size_t size) {
  auto const alloc_size = ROUNDUP(int32(size), kAllocUnit);
  auto const next = curr_ + alloc_size;
  if (next > end_) {
    return nullptr;
  }

  auto const offset = curr_;
  curr_ = next;
  return reinterpret_cast<char*>(this) + offset;
}

CodeManager::CodeHeap::CodeHeap() {
  head_ = tail_ = new(new char[sizeof(Block)]) Block(nullptr, 0);
}

CodeManager::CodeHeap::~CodeHeap() {
  auto runner = head_;
  while (runner) {
    auto const next = runner->next_;
    if (!::VirtualFree(runner, runner->end_, MEM_DECOMMIT)) {
      auto const error = ::GetLastError();
      Debugger::Fail("VirtualFree: err=%d %p+%d byte\n", 
          error, runner, runner->end_);
    }
    runner = next;
  }
}

// ctor
CodeManager::CodeManager() {
  DEBUG_PRINTF("%p\n", this);
}

CodeManager::~CodeManager() {
  DEBUG_PRINTF("%p\n", this);
}

// [F]
CodeDesc* CodeManager::Find(const Address addr) const {
  ScopedLock lock(code_desc_map_lock_);
  return code_desc_map_.Find(addr);
}

} // Ee
