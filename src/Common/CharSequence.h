// @(#)$Id$
//
// Evita CharSequence
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_CharSequence_h)
#define INCLUDE_Common_CharSequence_h

namespace Common {

class CharSequence {
  public: class Enum {
      private: const char16* m_pwchRunner;
      private: const char16* const m_pwchEnd;

      public: Enum(CharSequence& rCharSeq) :
          m_pwchEnd(rCharSeq.m_pwchEnd),
          m_pwchRunner(rCharSeq.m_pwchStart) {}

      private: Enum& operator =(Enum&);

      public: bool AtEnd() const {
          return m_pwchRunner >= m_pwchEnd;
      } // AtEnd

      public: char16 Get() const {
          ASSERT(!this->AtEnd());
          return *m_pwchRunner;
      } // Get

      public: const char16* GetPtr() const {
          return m_pwchRunner;
      } // GetPtr

      public: void Next() {
          ASSERT(!this->AtEnd());
          m_pwchRunner++;
      } // Next
  }; // Enum

  private: const char16* const m_pwchEnd;
  private: const char16* const m_pwchStart;

  public: CharSequence(
      const char16* const pwchStart,
      const char16* const pwchEnd) :
          m_pwchEnd(pwchEnd),
          m_pwchStart(pwchStart) {}

  DISALLOW_COPY_AND_ASSIGN(CharSequence);
}; // CharSequence

} // Common

#endif // !defined(INCLUDE_Common_CharSequence_h)
