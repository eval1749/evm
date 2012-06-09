// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_GlobalMemoryZone_h)
#define INCLUDE_Common_GlobalMemoryZone_h

#include "./MemoryZone.h"

namespace Common {

class GlobalMemoryZone : public MemoryZone {
  private: static GlobalMemoryZone* s_pGlobalMemoryZone;

  private: HANDLE const heap_handle_;

  private: GlobalMemoryZone();
  private: ~GlobalMemoryZone() { CAN_NOT_HAPPEN(); }

  // [A]
  public: virtual void* Alloc(size_t const cb) override;

  // [F]
  public: virtual void Free(void* const pv) override;

  // [G]
  public: static MemoryZone& Get() {
    return *s_pGlobalMemoryZone;
  } // Get

  // [I]
  public: static void Init();

  DISALLOW_COPY_AND_ASSIGN(GlobalMemoryZone);
}; // GlobalMemoryZone

} // Common

#endif // !defined(INCLUDE_Common_GlobalMemoryZone_h)
