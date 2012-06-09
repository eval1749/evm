// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Compiler_Passes_AbstractCompilerTest_h)
#define INCLUDE_test_Compiler_Passes_AbstractCompilerTest_h

#include <ostream>

#include "Compiler/CompileSession.h"
#include "ErrorItem.h"
#include "../Il/IlTest.h"

namespace CompilerTest {

using namespace Compiler;

class AbstractCompilerTest : public ::testing::Test {
  private: CompileSession& session_;
  private: CompilationUnit& compilation_unit_;

  protected: AbstractCompilerTest();
  protected: virtual ~AbstractCompilerTest();

  // properties
  protected: CompilationUnit& compilation_unit() const {
    return compilation_unit_;
  }

  protected: CompileSession& session() const { return session_; }

  // [G]
  protected: ErrorList GetErrors() const;

  // [M]
  public: ErrorList MakeErrorTree(int, ...);
  public: ErrorList MakeErrorTree(const ErrorItem&);
  public: ErrorList MakeErrorTree(const ErrorItem&, const ErrorItem&);

  public: ErrorList MakeErrorTree(
      const ErrorItem&, const ErrorItem&, const ErrorItem&);

  DISALLOW_COPY_AND_ASSIGN(AbstractCompilerTest);
};

} // CompilerTest

#endif // !defined(INCLUDE_test_Compiler_Passes_AbstractCompilerTest_h)
