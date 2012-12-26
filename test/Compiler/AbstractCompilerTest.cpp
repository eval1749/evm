#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - AbstractParseTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractCompilerTest.h"

namespace CompilerTest {

using namespace Compiler;

AbstractCompilerTest::AbstractCompilerTest()
    : session_(*new CompileSession("output")),
      compilation_unit_(*new CompilationUnit(session_, "unit_test")) {
  session_.Add(compilation_unit_);
}

AbstractCompilerTest::~AbstractCompilerTest() {
  DEBUG_PRINTF("%p\n", this);
  delete &session_;
}

// [G]
ErrorList AbstractCompilerTest::GetErrors() const {
  ErrorList error_list;
  for (auto& error: session().errors())
    error_list.Add(ErrorItem(error));
  return error_list;
}

// [M]
ErrorList AbstractCompilerTest::MakeErrorTree(int num_errors, ...) {
  ErrorList error_list;
  va_list args;
  va_start(args, num_errors);
  while (--num_errors >= 0) {
    auto const p = va_arg(args, ErrorItem*);
    error_list.Add(*p);
  }
  va_end(args);
  return error_list;
}

ErrorList AbstractCompilerTest::MakeErrorTree(const ErrorItem& e1) {
  return MakeErrorTree(1, &e1);
}
ErrorList AbstractCompilerTest::MakeErrorTree(
    const ErrorItem& e1, const ErrorItem& e2) {
  return MakeErrorTree(2, &e1, &e2);
}

ErrorList AbstractCompilerTest::MakeErrorTree(
    const ErrorItem& e1, const ErrorItem& e2, const ErrorItem& e3) {
  return MakeErrorTree(3, &e1, &e2, &e3);
}

} // CompilerTest
