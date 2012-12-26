#include "precomp.h"
// @(#)$Id$
//
// Evita Common - Formatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./DebugHelper.h"

#include "./Box.h"

namespace Common {
namespace DebugHelper{

void __declspec(noreturn) __fastcall CheckFailed(
    Box a,
    Box b,
    const char* str_a,
    const char* str_b,
    const char* str_op,
    const char* filename,
    int const linenum,
    const char* fname) {
  ::OutputDebugStringW(
      String::Format("%s(%d): %s: %s %s %s\r\n",
          filename, linenum, fname,
          String::Format("%s %s %s", str_a, str_op, str_b),
          String::Format("%s=%s", str_a, a),
          String::Format("%s=%s", str_b, b)).value());
  ::Debugger::Fail(
      "Check faield: %ls\n"
      "\n"
      "File: %s\n"
      "Line: %d\n"
      "Function: %s\n"
      "\n"
      "Actual: %s = %ls\n"
      "Expect: %s = %ls\n",
      String::Format("%s %s %s", str_a, str_op, str_b).value(),
      filename,
      linenum,
      fname,
      str_a, a.ToString().value(),
      str_b, b.ToString().value());
}

void __fastcall CheckTrue(
    bool value,
    const char* msg,
    const char* filename,
    int const linenum,
    const char* fname) {
  if (value) return;

  ::OutputDebugStringW(
      String::Format("%s(%d): %s: %s\r\n",
      filename, linenum, fname,
      msg).value());

  ::Debugger::Fail(
      "Check failed: %s\n"
      "\n"
      "File: %s\n"
      "Line: %d\n"
      "Function: %s\n",
      msg,
      filename,
      linenum,
      fname);
}

} // DebugHelper
} // Common
