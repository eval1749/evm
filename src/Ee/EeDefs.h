// @(#)$Id$
//
// Evita Ee - CodeManager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Ee_EeDefs_h)
#define INCLUDE_Evita_Ee_EeDefs_h

namespace Ee {

struct Address {
  uintptr_t value_;
  Address(const void* pv) : value_(reinterpret_cast<uintptr_t>(pv)) {}
};

static_assert(
  sizeof(Address) == sizeof(intptr_t),
  "sizeof(Address) must be sizeof(intptr_t)");

// Initialize Executor.
void Init();
void RuntimeInit();

} // Ee

#endif // !defined(INCLUDE_Evita_Ee_EeDefs_h)
