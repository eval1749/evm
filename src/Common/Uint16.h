// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_UInt16_h)
#define INCLUDE_Common_UInt16_h

namespace Common {

class MemoryZone;

class UInt16 : public WithCastable_<UInt16, Object>, public Allocable {
  CASTABLE_CLASS(UInt16);

  private: uint16 const m_iValue;

  public: UInt16(uint16 iValue) :
      m_iValue(iValue) {}

  public: uint16 GetValue() const { return m_iValue; }

  DISALLOW_COPY_AND_ASSIGN(UInt16);
}; // UInt16

} // Common

#endif // !defined(INCLUDE_Common_UInt16_h)
