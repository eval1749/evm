// @(#)$Id$
//
// Evita Common StringBuilder
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_StringBuilder_h)
#define INCLUDE_Common_StringBuilder_h

#include "./Object.h"

#include "./Collections/Array_.h"
#include "./Collections/ArrayList_.h"
#include "./Collections/Collection_.h"
#include "./String.h"

namespace Common {

class Box;

class StringBuilder : public Object_<StringBuilder> {
  CASTABLE_CLASS(StringBuilder);

  private: size_t m_nCapacity;
  private: char16* m_prgwch;
  private: size_t m_cwch;

  public: StringBuilder();
  public: virtual ~StringBuilder();

  // [A]
  public: void Append(char16 const wch) {
    Append(&wch, 1);
  }

  public: void Append(const char16* const pwsz) {
    Append(pwsz, size_t(::lstrlenW(pwsz)));
  }

  public: void Append(const char* const psz) {
    Append(psz, size_t(::lstrlenA(psz)));
  }

  public: void Append(const String&);
  public: void Append(const char16*, size_t);
  public: void Append(const char*, size_t);

  public: void AppendFormat(const String&, const Collection_<Box>&);
  DECLARE_VARDIC_METHODS(public: void AppendFormat, const String&, Box);

  // [C]
  public: void Clear() {
    m_cwch = 0;
    m_prgwch[0] = 0;
  }

  // [G]
  public: size_t GetLength() const { return m_cwch; }

  public: const char16* GetString() const {
    ASSERT(m_prgwch[m_cwch] == 0);
    return m_prgwch;
  }

  // [I]
  public: bool IsEmpty() const { return m_cwch == 0; }

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(StringBuilder);
}; // StringBuilder

}

#endif // !defined(INCLUDE_Common_StringBuilder_h)
