// @(#)$Id$
//
// Evita Common
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_String_h)
#define INCLUDE_Common_String_h

#include "./Object.h"
#include "./Collections/Collection_.h"
#include "./Collections/Iterator_.h"

namespace Common {

class Box;
class MemoryZone;
using Collections::Collection_;

class String : public WithCastable_<String, Object> {
  CASTABLE_CLASS(String);

  private: class CharData : public RefCounted_<CharData> {
    private: const char16* const  chars_;
    private: size_t const length_;

    public: CharData(size_t);
    public: CharData(const char16*, size_t);
    public: CharData(const char* str8, size_t);
    public: ~CharData();
    public: const char16* chars() const { return chars_; }
    public: size_t length() const { return length_; }

    DISALLOW_COPY_AND_ASSIGN(CharData);
  };

  public: class EnumChar : public EnumArray_<char16> {
    public: EnumChar(const String& r)
        : EnumArray_<char16>(r.chars_->chars(), r.chars_->length()) {}

    DISALLOW_COPY_AND_ASSIGN(EnumChar);
  }; // EnumChar

  public: class EnumCharFromEnd {
    private: const char16* const m_pwchStart;
    private: const char16* m_pwch;

    public: EnumCharFromEnd(const String& r)
        : m_pwchStart(r.chars_->chars()),
          m_pwch(r.chars_->chars() + r.chars_->length() - 1) {
    }

    public: bool AtEnd() const { return m_pwch < m_pwchStart; }
    public: char16 Get() const { ASSERT(!AtEnd()); return *m_pwch; }
    public: void Next() { ASSERT(!AtEnd()); m_pwch--; }

    DISALLOW_COPY_AND_ASSIGN(EnumCharFromEnd);
  }; // EnumCharFromEnd

  public: class Iterator {
    public: typedef char16 ElementType;
    protected: const String* string_;
    protected: size_t index_;

    public: Iterator() : string_(nullptr), index_(0) {}
    public: Iterator(const String& s, size_t n) : string_(&s), index_(n) {}

    public: Iterator(const Iterator& it)
        : string_(it.string_), index_(it.index_) {}

    public: char16 operator *() { return (*string_)[index_]; }
    public: Iterator operator ++() { ++index_; return *this; }
    public: Iterator operator --() { --index_; return *this; }

    public: Iterator& operator +=(int n) { index_ += n; return *this; }
    public: Iterator& operator -=(int n) { index_ -= n; return *this; }

    public: Iterator operator +(int n) const {
      return Iterator(*string_, index_ + n);
    }

    public: Iterator operator -(int n) const {
      return Iterator(*string_, index_ - n);
    }

    public: bool operator ==(const Iterator& r) const {
      //DCHECK_EQ(string_, r.string_);
      return index_ == r.index_;
    }

    public: bool operator !=(const Iterator& r) const {
      return !operator ==(r);
    }
  };

  private: static CharData* kEmptyCharData;
  private: CharData* chars_;

  private: String(CharData*);
  public: String();
  public: String(char);
  public: explicit String(int); // for not found value of HashMap.
  public: String(const char*);
  public: String(const char*, size_t);
  public: String(const char16*);
  public: String(const char16*, size_t);
  public: String(const String&);
  public: virtual ~String();

  // operator
  public: bool operator==(const String& r) const { return Equals(r); }
  public: bool operator!=(const String& r) const { return !operator==(r); }
  public: bool operator<(const String& r) const  { return !operator>=(r); }
  public: bool operator<=(const String&) const;
  public: bool operator>(const String& r) const  { return !operator<=(r); }
  public: bool operator>=(const String&) const;
  public: String& operator=(const String&);
  public: char16 operator[](size_t) const;

  // properties
  public: Iterator begin() const { return Iterator(*this, 0); }
  public: Iterator end() const { return Iterator(*this, size()); }
  public: size_t length() const { return chars_->length(); }
  public: size_t size() const { return chars_->length(); }
  public: const char16* value() const { return chars_->chars(); }

  // [C]
  public: int CompareTo(const String&) const;

  public: static int LibExport ComputeHashCode(const char16*, const char16*);

  public: static int LibExport ComputeHashCode(const char16* const pwsz) {
    return ComputeHashCode(pwsz, pwsz + ::lstrlenW(pwsz));
  }

  public: static int  LibExport ComputeHashCode(const String& r) {
    return ComputeHashCode(r.GetString(), r.GetString() + r.GetLength());
  }

  public: int ComputeHashCode() const {
    return ComputeHashCode(*this);
  }

  // [C]
  public: String Concat(const char16* const pwsz) const {
    ASSERT(pwsz != nullptr);
    return Concat(pwsz, size_t(::lstrlenW(pwsz)));
  }

  public: String Concat(const char16* pwch, size_t cwch) const;

  // [E]
  public: bool EndsWith(const String&) const;
  public: bool Equals(const String&) const;

  // [F]
  public: static String LibExport Format(
      const String&, const Collection_<Box>&);

  DECLARE_VARDIC_METHODS(
    public: static String LibExport Format, const String&, Box);

  // [G]
  public: size_t GetLength() const { return chars_->length(); }
  public: const char16* GetString() const { return chars_->chars(); }

  // [I]
  public: int IndexOf(char16) const;
  public: static void LibExport Init();
  public: bool IsEmpty() const { return chars_->length() == 0; }

  // [L]
  public: int LastIndexOf(char16 wch) const;

  // [S]
  public: String Substring(int) const;
  public: String Substring(int, int) const;

  // [T]
  public: virtual String ToString() const override;
}; // String

inline int ComputeHashCode(const String& str) {
  return str.ComputeHashCode();
}

static_assert(sizeof(String) == sizeof(void*) * 2,
    "sizeof(String) should be sizeof(void*)x2.");

}

#endif // !defined(INCLUDE_Common_String_h)
