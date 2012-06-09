// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Float32_h)
#define INCLUDE_Common_Float32_h

namespace Common {

class Float32 : public Object_<Float32> {
  CASTABLE_CLASS(Float32)

  private: union Union {
    float32 m_fValue;
    uint32 m_nValue;
  }; // Union

  private: float32 const m_fValue;

  public: Float32(float32 fValue) :
      m_fValue(fValue) {}

  public: uint32 GetUInt() const {
      Union uni;
      uni.m_fValue = m_fValue;
      return uni.m_nValue;
  } // GetUInt

  public: float32 GetValue() const { return m_fValue; }

  DISALLOW_COPY_AND_ASSIGN(Float32);
}; // Float32

} // Common

#endif // !defined(INCLUDE_Common_Float32_h)
