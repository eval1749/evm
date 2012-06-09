#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - String
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Allocable.h"

#include "./GlobalMemoryZone.h"
#include "./MemoryZone.h"

namespace Common {

void Allocable::operator delete(void* pv, size_t cb) {
  if (pv == nullptr) return;
  auto const extra = reinterpret_cast<Extra*>(pv) - 1;
  auto& zone = extra->zone_;
  auto const footer = reinterpret_cast<int*>(reinterpret_cast<char*>(extra) + sizeof(Extra) + extra->size_);
  ASSERT(extra->size_ == cb);
  ASSERT(extra->live_ == Extra::kMagic);
  ASSERT(*footer == Extra::kMagic);
  *footer = 0;
  zone.Free(extra);
} // delete

// size_t is sizeof(T) instead of sizeof(T[]).
void Allocable::operator delete[](void* pv, size_t) {
  if (pv == nullptr) return;
  auto const extra = reinterpret_cast<Extra*>(pv) - 1;
  ASSERT(extra->live_ == Extra::kMagic);
  extra->live_ = 0;
  auto& zone = extra->zone_;

  auto const footer = reinterpret_cast<int*>(reinterpret_cast<char*>(extra) + sizeof(Extra) +  + extra->size_);
  ASSERT(*footer == Extra::kMagic);
  *footer = 0;

  zone.Free(extra);
} // delete

void* Allocable::operator new(size_t cb, MemoryZone& zone) {
  return Alloc(zone, cb);
} // new

void* Allocable::operator new[](size_t cb, MemoryZone& zone) {
  return Alloc(zone, cb);
} // new

void* Allocable::operator new(size_t cb, MemoryZone* const zone) {
  ASSERT(zone != nullptr);
  return Alloc(*zone, cb);
} // new

void* Allocable::operator new[](size_t cb, MemoryZone* const zone) {
  ASSERT(zone != nullptr);
  return Alloc(*zone, cb);
} // new

void* Allocable::operator new(size_t cb) {
  return Allocable::operator new(cb, GlobalMemoryZone::Get());
} // new

void* Allocable::operator new[](size_t cb) {
  return Allocable::operator new(cb, GlobalMemoryZone::Get());
} // new

void* Allocable::Alloc(MemoryZone& zone, size_t cb) {
  auto const extra = new(zone.Alloc(sizeof(Extra) + cb + sizeof(int))) Extra(zone, cb);
  auto const footer = reinterpret_cast<int*>(reinterpret_cast<char*>(extra) + sizeof(Extra) + cb);
  *footer = Extra::kMagic;
  return extra + 1;
} // Alloc

} // Common
