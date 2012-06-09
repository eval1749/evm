// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_UInt64_h)
#define INCLUDE_Common_UInt64_h

namespace Common {

class UInt64 : public WithCastable_<UInt64, Object>, public Allocable {
  CASTABLE_CLASS(UInt64)

  private: uint64 const m_iValue;

  public: UInt64(uint64 iValue) :
      m_iValue(iValue) {}

  public: uint64 GetValue() const { return m_iValue; }

  DISALLOW_COPY_AND_ASSIGN(UInt64);
}; // UInt64

} // Common

#endif // !defined(INCLUDE_Common_UInt64_h)
