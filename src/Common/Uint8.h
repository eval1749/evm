// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_UInt8_h)
#define INCLUDE_Common_UInt8_h

namespace Common {

class MemoryZone;

class UInt8 : public WithCastable_<UInt8, Object>, public Allocable {
  CASTABLE_CLASS(UInt8)

  private: uint8 const m_iValue;

  public: UInt8(uint8 iValue) :
      m_iValue(iValue) {}

  public: uint8 GetValue() const { return m_iValue; }

  DISALLOW_COPY_AND_ASSIGN(UInt8);
}; // UInt8

} // Common

#endif // !defined(INCLUDE_Common_UInt8_h)
