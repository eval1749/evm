// @(#)$Id$
//
// Evita Common IntPtr
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_IntPtr_h)
#define INCLUDE_Common_IntPtr_h

#include "./Object.h"

namespace Common {

class IntPtr : public WithCastable_<IntPtr, Object>, public Allocable {
  CASTABLE_CLASS(IntPtr);

  private: intptr_t const value_;

  public: IntPtr(intptr_t value = 0) : value_(value) {}

  public: IntPtr(const void* pv)
    : value_(reinterpret_cast<intptr_t>(pv)) {
  }

  public: IntPtr(const IntPtr& box) : value_(box.value_) {}
  private: IntPtr& operator=(const IntPtr&);

  public: intptr_t value() const { return value_; }

  // [T]
  public: virtual String ToStringWithFormat(
      const char16*, FormatProvider&) const override;
}; // IntPtr

} // Common

#endif // !defined(INCLUDE_Common_IntPtr_h)
