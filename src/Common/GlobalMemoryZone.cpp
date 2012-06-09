#include "precomp.h"
// @(#)$Id$
//
// Evita Common - String
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./GlobalMemoryZone.h"

using Common::MemoryZone;

void* operator new[](size_t cb, MemoryZone* pMemoryZone) {
  return pMemoryZone->Alloc(cb);
} // new

namespace Common {

GlobalMemoryZone* GlobalMemoryZone::s_pGlobalMemoryZone;

GlobalMemoryZone::GlobalMemoryZone() 
    : heap_handle_(::GetProcessHeap()) {
}

void* GlobalMemoryZone::Alloc(size_t const cb) {
  return ::HeapAlloc(heap_handle_, HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, cb);
} // Alloc

void GlobalMemoryZone::Free(void* const pv) {
  ASSERT(pv != nullptr);
  ::HeapFree(heap_handle_, HEAP_NO_SERIALIZE, pv);
} // Free

void GlobalMemoryZone::Init() {
  ASSERT(s_pGlobalMemoryZone == nullptr);
  s_pGlobalMemoryZone = new GlobalMemoryZone();
} // Init

} // Common
