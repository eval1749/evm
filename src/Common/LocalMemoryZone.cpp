#include "precomp.h"
// @(#)$Id$
//
// Evita Common - String
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LocalMemoryZone.h"

namespace Common {

LocalMemoryZone::LocalMemoryZone()
  : heap_(::HeapCreate(HEAP_NO_SERIALIZE, 0, 0)),
    max_(0),
    min_(reinterpret_cast<void*>(intptr_t(-1))) {}

LocalMemoryZone::~LocalMemoryZone() {
  if (heap_) {
    ::HeapDestroy(heap_);
  }
}

void* LocalMemoryZone::Alloc(size_t const cb) {
  void* pv = ::HeapAlloc(heap_, HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, cb);
  if (min_ > pv) min_ = pv;
  if (max_ < pv) max_ = pv;
  return pv;
}

void LocalMemoryZone::Free(void* const pv) {
  ASSERT(pv != nullptr);
}

bool LocalMemoryZone::Has(const void* const pv) const {
  return pv >= min_ && pv <= max_;
}

} // Common
