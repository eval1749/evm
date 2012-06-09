// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Int16_h)
#define INCLUDE_Common_Int16_h

namespace Common {

class MemoryZone;

class Int16 : public WithCastable_<Int16, Object>, public Allocable {
  CASTABLE_CLASS(Int16)

  private: int16 const m_iValue;

  public: Int16(int16 iValue) :
      m_iValue(iValue) {}

  public: int16 GetValue() const { return m_iValue; }

  DISALLOW_COPY_AND_ASSIGN(Int16);
}; // Int16

} // Common

#endif // !defined(INCLUDE_Common_Int16_h)
