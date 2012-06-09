// @(#)$Id$
//
// Evita Common Boolean
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Boolean_h)
#define INCLUDE_Common_Boolean_h

#include "./Object.h"

namespace Common {

class Boolean : public WithCastable_<Boolean, Object> {
  CASTABLE_CLASS(Boolean);

  public: static Boolean* False;
  public: static Boolean* True;

  private: bool const m_fValue;

  // ctor
  private: Boolean(bool fValue) : m_fValue(fValue) {}
  protected: Boolean() : m_fValue(false) {}

  // [G]
  public: bool GetValue() const { return m_fValue; }

  // [I]
  public: static void LibExport Init();

  // [N]
#if 0
  public: Boolean* New(bool const fValue) {
    return fValue ? True : False;
  } // New
#endif

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(Boolean);
}; // Boolean

} // Common

#endif // !defined(INCLUDE_Common_Boolean_h)
