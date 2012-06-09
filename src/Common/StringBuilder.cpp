#include "precomp.h"
// @(#)$Id$
//
// Evita Common StringBuilder
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./StringBuilder.h"

#include "./String.h"

namespace Common {

StringBuilder::StringBuilder()
    : m_nCapacity(100),
      m_cwch(0) {
  m_prgwch = new char16[m_nCapacity];
  m_prgwch[0] = 0;
}

StringBuilder::~StringBuilder() {
  delete[] m_prgwch;
}

// [A]
void StringBuilder::Append(const String& str) {
  Append(str.value(), str.length());
}

void StringBuilder::Append(const char16* const pwch, size_t const cwch) {
  ASSERT(pwch != nullptr);

  if (cwch == 0) {
    return;
  }

  if (m_cwch + cwch + 1 >= m_nCapacity) {
    auto const prgwchOld = m_prgwch;
    auto const nOldCapacity = m_nCapacity;
    m_nCapacity = (m_nCapacity + cwch) * 3 / 2;
    m_prgwch = new char16[m_nCapacity];

    ::CopyMemory(
        m_prgwch,
        prgwchOld,
        sizeof(char16) * nOldCapacity);

    delete[] prgwchOld;
  }

  ::CopyMemory(
      m_prgwch + m_cwch,
      pwch,
      sizeof(char16) * cwch);

  m_cwch += cwch;

  m_prgwch[m_cwch] = 0;
}

void StringBuilder::Append(const char* const start, size_t const cch) {
  ASSERT(start != nullptr);
  if (cch == 0) {
    return;
  }

  auto const end = start + cch;
  for (auto pch = start; pch < end; pch++) {
    Append(*pch);
  }
}

// [T]
String StringBuilder::ToString() const {
  return String(m_prgwch, m_cwch);
}

} // Common
