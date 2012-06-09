// @(#)$Id$
//
// Evita Common Int32
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Int32_h)
#define INCLUDE_Common_Int32_h

#include "./Object.h"

namespace Common {

class Int32 : public WithCastable_<Int32, Object>, public Allocable {
  CASTABLE_CLASS(Int32);

  public: static Int32* Zero;

  public: static int32 const MaxValue = static_cast<int32>((1u << 31) - 1);
  public: static int32 const MinValue = static_cast<int32>(1u << 31);

  private: int32 const value_;

  public: Int32(int32 const value = 0) : value_(value) {}
  public: Int32(const Int32& box) : value_(box.value_) {}

  private: Int32& operator=(const Int32&);

  // properties
  public: int32 value() const { return value_; }

  // [E]
  public: bool Equals(const Int32& that) const {
    return value_ == that.value_;
  }

  // [I]
  public: static void LibExport Init();

  // [T]
  public: virtual String ToString() const override;

  public: virtual String ToStringWithFormat(
      const char16*, FormatProvider&) const override;
}; // Int32

} // Common

#endif // !defined(INCLUDE_Common_Int32_h)
