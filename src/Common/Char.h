// @(#)$Id$
//
// Evita Char
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Char_h)
#define INCLUDE_Common_Char_h

#include "./Object.h"

#include "./UnicodeCategory.h"

namespace Common {

class Char : public Object_<Char> {
  CASTABLE_CLASS(Char);

  public: static int const MaxValue = (1 << 16) - 1;
  public: static int const MinValue = 0;

  private: UnicodeCategory m_eUnicodeCategory;
  
  // ctor
  protected: Char() {}

  private: void* operator new(size_t, void* pv) { return pv; }

  // [G]
  public: char16 GetCode() const;

  public: UnicodeCategory LibExport GetUnicodeCategory() const {
    return m_eUnicodeCategory;
  } // GetUnicodeCategory

  public: static UnicodeCategory GetUnicodeCategory(char16 const wch) {
    return Char::New(wch)->GetUnicodeCategory();
  } // GetUnicodeCategory

  // [I]
  public: static void LibExport Init();

  // [N]
  public: static Char* LibExport New(uint const nCode);

  // [T]
  public: virtual String ToString() const;

  DISALLOW_COPY_AND_ASSIGN(Char);
}; // Char

} // Common

#endif // !defined(INCLUDE_Common_Char_h)
