// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Int8_h)
#define INCLUDE_Common_Int8_h

namespace Common {

class MemoryZone;

class Int8 : public WithCastable_<Int8, Object> {
  CASTABLE_CLASS(Int8)

  private: int8 const m_iValue;

  public: Int8(int8 iValue) :
      m_iValue(iValue) {}

  public: int8 GetValue() const { return m_iValue; }

  DISALLOW_COPY_AND_ASSIGN(Int8);
}; // Int8

} // Common

#endif // !defined(INCLUDE_Common_Int8_h)
