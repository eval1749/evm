// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_UInt32_h)
#define INCLUDE_Common_UInt32_h

#include "./Object.h"

namespace Common {

class MemoryZone;

class UInt32 : public WithCastable_<UInt32, Object>, public Allocable {
  CASTABLE_CLASS(UInt32);

  private: uint32 const value_;

  public: UInt32(uint32 value = 0) : value_(value) {}
  public: UInt32(const UInt32& box) : value_(box.value_) {}

  private: UInt32& operator=(const UInt32&);

  // properties
  public: uint32 value() const { return value_; }

  // [E]
  public: bool Equals(const UInt32& that) const {
    return value_ == that.value_;
  }

  // [T]
  public: virtual String ToString() const override;

  public: virtual String ToStringWithFormat(
      const char16*,
      FormatProvider&) const override;
}; // UInt32

} // Common

#endif // !defined(INCLUDE_Common_UInt32_h)
