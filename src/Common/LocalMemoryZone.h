// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_LocalMemoryZone_h)
#define INCLUDE_Common_LocalMemoryZone_h

#include "./MemoryZOne.h"

namespace Common {

class LocalMemoryZone :
      public MemoryZone {

  private: HANDLE const heap_;
  private: void* max_;
  private: void* min_;

  public: LocalMemoryZone();
  public: virtual ~LocalMemoryZone();

  // [A]
  public: virtual void* Alloc(size_t const cb) override;

  // [F]
  public: virtual void Free(void* const pv) override;

  // [I]
  public: bool Has(const void*) const;

  DISALLOW_COPY_AND_ASSIGN(LocalMemoryZone);
}; // LocalMemoryZone

} // Common

#endif // !defined(INCLUDE_Common_LocalMemoryZone_h)
