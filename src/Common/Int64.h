// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Int64_h)
#define INCLUDE_Common_Int64_h

namespace Common {

class MemoryZone;

class Int64 : public WithCastable_<Int64, Object>, public Allocable {
  CASTABLE_CLASS(Int64);

  static int64 const MaxValue =
    static_cast<int64>((static_cast<uint64>(1) << 63) - 1);

  static int64 const MinValue =
    static_cast<int64>(static_cast<uint64>(1) << 63);

  private: int64 const m_iValue;

  public: Int64(int64 iValue) : m_iValue(iValue) {}
  public: int64 GetValue() const { return m_iValue; }

  DISALLOW_COPY_AND_ASSIGN(Int64);
}; // Int64

} // Common

#endif // !defined(INCLUDE_Common_Int64_h)
