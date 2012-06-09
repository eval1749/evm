#include "precomp.h"
// @(#)$Id$
//
// Evita Common - String
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./String.h"

#include "./DebugHelper.h"
#include "./GlobalMemoryZone.h"
#include "./Io/StringWriter.h"

namespace Common {

using Common::Io::StringWriter;

String::CharData* String::kEmptyCharData;

String::CharData::CharData(size_t const count)
    : chars_(new char16[count + 1]), length_(count) {
  const_cast<char16*>(chars_)[length_] = 0;
}

String::CharData::CharData(const char16* const chars, size_t count)
    : chars_(new char16[count + 1]), length_(count) {
  ::CopyMemory(const_cast<char16*>(chars_),
               chars, sizeof(char16) * length_);
  const_cast<char16*>(chars_)[length_] = 0;
  AddRef();
}

String::CharData::CharData(const char* str8, size_t count)
    : chars_(new char16[count + 1]), length_(count) {
  auto const end = str8 + count;
  auto dest = const_cast<char16*>(chars_);
  for (auto s = str8; s < end; s++) {
    *dest++ = *s;
  }
  *dest = 0;
}

String::CharData::~CharData() {
  ASSERT(this != kEmptyCharData);
  delete[] chars_;
}

// ctor
String::String() : chars_(kEmptyCharData) {
  chars_->AddRef();
}

String::String(CharData* chars) : chars_(chars) {
  ASSERT(chars_ != nullptr);
  chars_->AddRef();
}

String::String(char const ch)
    : chars_(new CharData(&ch, 1)) {
  chars_->AddRef();
}

String::String(const char* const str8)
    : chars_(new CharData(str8, ::lstrlenA(str8))) {
  chars_->AddRef();
}

String::String(const char* const str8, size_t const count)
    : chars_(new CharData(str8, count)) {
  chars_->AddRef();
}

String::String(const char16* const str16)
    : chars_(new CharData(str16, ::lstrlenW(str16))) {
  chars_->AddRef();
}

String::String(const char16* const str16, size_t count)
    : chars_(new CharData(str16, count)) {
  chars_->AddRef();
}

String::String(const String& that) : chars_(that.chars_) {
  chars_->AddRef();
}

String::String(int const value) : chars_(kEmptyCharData) {
  ASSERT(value == 0);
  chars_->AddRef();
}

String::~String() {
  chars_->Release();
}

String& String::operator=(const String& that) {
  chars_->Release();
  chars_ = that.chars_;
  chars_->AddRef();
  return *this;
}

bool String::operator<=(const String& r) const {
  if (this == &r) return true;
  return CompareTo(r) <= 0;
}

bool String::operator>=(const String& r) const {
  if (this == &r) return true;
  return CompareTo(r) >= 0;
}

char16 String::operator [](size_t const index) const {
  DCHECK_LT(index, size());
  return value()[index];
}

// [C]
int String::CompareTo(const String& r) const {
  EnumChar this_chars(*this);
  EnumChar that_chars(r);
  while (!that_chars.AtEnd() && !this_chars.AtEnd()) {
    if (auto const diff = *this_chars - *that_chars) {
      return diff < 0 ? -1 : 1;
    }
    that_chars.Next();
    this_chars.Next();
  }

  if (this_chars.AtEnd()) {
    return that_chars.AtEnd() ? 0 : -1;
  }

  return that_chars.AtEnd() ? 1 : -1;
}

int String::ComputeHashCode(
    const char16* const start,
    const char16* const end) {
  auto hash_code = 0;
  for (auto p = start; p < end; p++) {
     hash_code = Common::ComputeHashCode(*p, hash_code);
  }
  return hash_code;
}

String String::Concat(const char16* const pwch, size_t const cwch) const {
  auto chars = new CharData(length() + cwch);
  auto start = const_cast<char16*>(chars->chars());
  ::CopyMemory(start, value(), sizeof(char16) * length());
  ::CopyMemory(start + length(), pwch, sizeof(char16) * cwch);
  start[chars->length()] = 0;
  return String(chars);
}

// [E]
bool String::EndsWith(const String& pat) const {
  if (length() < pat.length()) {
    return false;
  }

  return ::memcmp(
      value() + length() - pat.length(),
      pat.value(),
      sizeof(char16) * pat.length()) == 0;
}

bool String::Equals(const String& pat) const {
  return length() == pat.length()
    && ::memcmp(value(), pat.value(), sizeof(char16) * length()) == 0;
}

// [I]
int String::IndexOf(char16 const ch) const {
  auto index = 0;
  foreach (EnumChar, chars, *this) {
    if (chars.Get() == ch) {
      return index;
    }
    ++index;
  }
  return -1;
}

void String::Init() {
  kEmptyCharData = new CharData(static_cast<char16*>(nullptr), 0);
  kEmptyCharData->AddRef();
}

// [L]
int String::LastIndexOf(char16 wch) const {
  auto index = 0;
  foreach (EnumCharFromEnd, oEnum, *this) {
    index++;
    if (oEnum.Get() == wch) {
      return length() - index;
    }
  }
  return -1;
}

String String::Substring(int const start, int const len) const {
  ASSERT(start >= 0);
  ASSERT(start <= static_cast<int>(length()));
  ASSERT(start + len <= static_cast<int>(length()));
  return String(value() + start, len);
}

String String::Substring(int const start) const {
  ASSERT(start >= 0);
  ASSERT(start <= static_cast<int>(length()));
  return String(value() + start, length() - start);
}

// [T]
String String::ToString() const {
  return *this;
}

}
