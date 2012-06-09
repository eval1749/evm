// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Float64_h)
#define INCLUDE_Common_Float64_h

namespace Common {

class Float64 : public WithCastable_<Float64, Object> {
  CASTABLE_CLASS(Float64)

  private: union Union {
    float64 m_fValue;
    uint64 m_nValue;
  }; // Union

  private: float64 const m_fValue;

  public: Float64(float64 fValue) :
      m_fValue(fValue) {}

  public: uint64 GetUInt() const {
      Union uni;
      uni.m_fValue = m_fValue;
      return uni.m_nValue;
  } // GetUInt

  public: float64 GetValue() const { return m_fValue; }

  DISALLOW_COPY_AND_ASSIGN(Float64);
}; // Float64

} // Common

#endif // !defined(INCLUDE_Common_Float64_h)
