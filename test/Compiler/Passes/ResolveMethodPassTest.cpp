#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - ResolveMethodPassTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractPassTest.h"

#include "Compiler/CompileSession.h"
#include "Compiler/Passes/ResolveMethodPass.h"
#include "Compiler/Syntax/Parser.h"

namespace CompilerTest {

using namespace Compiler;

class ResolveMethodPassTest : public AbstractPassTest {
  protected: ResolveMethodPassTest()
      : AbstractPassTest(ResolveMethodPass::Create) {
  }
};

TEST_F(ResolveMethodPassTest, Basic) {
  // Note: Parameter name isn't optional in C#.
  RUN_PARSER(
    "using Common;",
    "interface A {",
    "  void Foo();",
    "  void Foo(int x);",
    "  void Foo(String s);",
    "  int Bar(String s, int x);",
    "  String[] Bar(int[] v);",
    "}");

  RUN_PASSES();

  auto const iface_a = FindClass("A");
  ASSERT_TRUE(iface_a != nullptr);
  CHECK_METHOD(iface_a, Foo, *Ty_Void, Ty_Int32);
}

}  // CompilerTest
