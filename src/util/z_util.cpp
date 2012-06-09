#include "precomp.h"
//////////////////////////////////////////////////////////////////////////////
//
// evcl - listener - winapp - Edit Buffer
// listener/winapp/ed_buffer.cpp
//
// Copyright (C) 1996-2007 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// @(#)$Id: //proj/evedit2/mainline/tinycl/z_util.cpp#2 $
//
#include "./z_util.h"

/// <summary>
///  Pointer of the first character in string.
/// </summary>
/// <returns>
///   Pointer of the first character in string or nullptr if not found.
/// </returns>
char16* LibExport lstrchrW(const char16* pwsz, char16 wch) {
  while (0 != *pwsz) {
    if (wch == *pwsz) {
      return const_cast<char16*>(pwsz);
    } // if
    pwsz++;
  } // while
  return nullptr;
} // lstrchrW


/// <summary>
///  Pointer of the last character in string.
/// </summary>
/// <returns>
///   Pointer of the last character in string or nullptr if not found.
/// </returns>
char16* LibExport lstrrchrW(const char16* pwsz, char16 wch) {
  const char16* pFound = nullptr;
  while (*pwsz != 0) {
      if (wch == *pwsz) {
        pFound = pwsz;
      } // if
      pwsz++;
  } // while
  return const_cast<char16*>(pFound);
} // lstrchrW
