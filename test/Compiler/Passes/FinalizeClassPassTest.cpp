#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - FinalizeClassPassTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractPassTest.h"

#include "Compiler/CompileSession.h"
#include "Compiler/Passes/FinalizeClassPass.h"
#include "Compiler/Syntax/Parser.h"

#include "../DomBuilder.h"

namespace CompilerTest {

using namespace Compiler;

class FinalizeClassPassTest : public AbstractPassTest {
  protected: FinalizeClassPassTest()
      : AbstractPassTest(FinalizeClassPass::Create) {
  }
};

TEST_F(FinalizeClassPassTest, Basic) {
  RUN_PARSER(
    "class A {}",
    "class B : A {"
    "  A a;",
    "  B b;",
    "}");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "A");
  CHECK_CLASS(class_b, class_a, "B");

  CHECK_FIELD(class_b, *class_a, field_a, "a");
  CHECK_FIELD(class_b, *class_b, field_b, "b");
}

TEST_F(FinalizeClassPassTest, AliasType) {
  RUN_PARSER(
      "namespace N1.N2 { class A {} }",
      "namespace N3 {",
      "  using R1 = N1;",
      "  using R2 = N1.N2;",
      "  class B {",
      "    N1.N2.A a;",  // refers to N1.N2.A
      "    R1.N2.A b;",  // refers to N1.N2.A
      "    R2.A c;",     // refers to N1.N2.A
      "  }",
      "}");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "N1", "N2", "A");
  CHECK_CLASS(class_b, _, "N3", "B");

  CHECK_FIELD(class_b, *class_a, field_a, "a");
  CHECK_FIELD(class_b, *class_a, field_b, "b");
  CHECK_FIELD(class_b, *class_a, field_c, "c");
}

}  // CompilerTest
