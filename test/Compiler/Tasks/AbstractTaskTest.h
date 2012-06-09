// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Compiler_Tasks_AbstractTaskTest_h)
#define INCLUDE_test_Compiler_Tasks_AbstractTaskTest_h

#include "../Passes/AbstractPassTest.h"

namespace CompilerTest {

class AbstractTaskTest : public AbstractPassTest {
  protected: AbstractTaskTest(CompilePass::NewFunction stop)
      : AbstractPassTest(stop) {}

  // [F]
  protected: CallI* FindCall(const NameRefI&) const;
  protected: NameRefI* FindNameRef(const Method&, const NameRef&) const;

  // [N]
  protected: const NameRef& NewNameRef(const String&) const;
  protected: const NameRef& NewNameRef(const String&, const String&) const;
};

} // CompilerTest

#endif // !defined(INCLUDE_test_Compiler_Tasks_AbstractTaskTest_h)
