#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - ResolveClassPassTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractPassTest.h"

#include "Compiler/CompileSession.h"
#include "Compiler/Passes/ResolveClassPass.h"
#include "Compiler/Syntax/Parser.h"

namespace CompilerTest {

using namespace Compiler;

class ResolveClassPassTest : public AbstractPassTest {
  protected: ResolveClassPassTest()
      : AbstractPassTest(ResolveClassPass::Create) {
  }
};

TEST_F(ResolveClassPassTest, ClassA_ClassB) {
  RUN_PARSER(
    "class A {}"
    "class B : A {}");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "A");
  CHECK_CLASS(class_b, class_a, "B");
}

TEST_F(ResolveClassPassTest, ClassB_ClassA) {
  // It is legal to use forward referenced class as base class. Source code
  // order doesn't affect class definition.
  RUN_PARSER(
    "class B : A {}",
    "class A {}");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "A");
  CHECK_CLASS(class_b, class_a, "B");
}

TEST_F(ResolveClassPassTest, BaseClassAsNamespace) {
  RUN_PARSER(
    "namespace N1 {",
    "  class A : N1 {}",
    "}");

  CHECK_PASS_ERROR("Resolve_ExpectClass", 1,13, "N1.A", "?N1=N1");
}

TEST_F(ResolveClassPassTest, ConstructedClass) {
  RUN_PARSER(
    "class A : G<A> {}",
    "class G<T> {}");
  RUN_PASSES();
  CHECK_CLASS(class_g, _, "G");
  auto const class_a = FindClass("A");
  ASSERT_TRUE(class_a != nullptr);
  TypeArgs type_args(
      *class_g->StaticCast<GenericClass>()->GetTypeParams().Get(0), 
      *class_a);
  EXPECT_EQ(Box(class_g->Construct(type_args)), Box(class_a->GetBaseClass()));
}

TEST_F(ResolveClassPassTest, UsingAlias) {
  RUN_PARSER(
    "namespace N1.N2 { class A {} }"
    "namespace N3 {"
    "  using C = N1.N2.A;"
    "  class B : C {}"
    "}");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "N1", "N2", "A");
  CHECK_CLASS(class_b, class_a, "N3", "B");
}

// Source code order of declaration isn't significant.
TEST_F(ResolveClassPassTest, UsingAlias2) {
  RUN_PARSER(
    "namespace N3 {"
    "  using C = N1.N2.A;"
    "  class B : C {}"
    "}"
    "namespace N1.N2 { class A {} }");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "N1", "N2", "A");
  CHECK_CLASS(class_b, class_a, "N3", "B");
}

// Note: MS C# compiler doesn't report error if alias A isn't used.
TEST_F(ResolveClassPassTest, UsingAliasAlreadyExists) {
  RUN_PARSER(
    "namespace N1.N2 { class A {} }",
    "namespace N3 { class A {} }",
    "namespace N3 {",
    "  using A = N1.N2.A;",
    "  class B : A {}",
    "}");

  CHECK_PASS_ERROR("Resolve_Alias_Conflict", 4, 13, "A");
}

// Scope of using alias directive is limitied into namespace body.
TEST_F(ResolveClassPassTest, UsingAliasScopeLimited) {
  RUN_PARSER(
    "namespace N1.N2 { class A {} }",
    "namespace N3 { using R = N1.N2; }",
    "namespace N3 { class B : R.A {} }"); // Error: R unknown

  CHECK_PASS_ERROR("Resolve_Class_NotFound", 2, 26, "R.A");
}

TEST_F(ResolveClassPassTest, UsingAliasExtent) {
  RUN_PARSER(
    "using R = N1.N2;",
    "namespace N1.N2 { class A {} }",
    "namespace N3 { class B : R.A {} }",
    "namespace N3 { class C : R.A {} }");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "N1", "N2", "A");
  CHECK_CLASS(class_b, class_a, "N3", "B");
  CHECK_CLASS(class_c, class_a, "N3", "C");
}

TEST_F(ResolveClassPassTest, UsingAliasScopeHide) {
  RUN_PARSER(
    "using R = N1.N2;",
    "namespace N1.N2 { class A {} }",
    "namespace N3 {",
    "  class R {}",         // Class R hides alias R in toplevel.
    "  class B : R.A {}",   // Error: R has no member A.
    "}");

  CHECK_PASS_ERROR("Resolve_Class_NotFound", 4, 13, "R.A");
}

TEST_F(ResolveClassPassTest, UsingAliasScopeResolution) {
  RUN_PARSER(
    "namespace N1.N2 {}",
    "namespace N3 {",
    "  using R1 = N1;",
    "  using R2 = N1.N2;",
    "  using R3 = R1.N2;",  // Error: R1 is unknown.
    "}");

  CHECK_PASS_ERROR("Resolve_Alias_NotFound", 4, 9, "R1.N2");
}

TEST_F(ResolveClassPassTest, UsingNamespace) {
  RUN_PARSER(
    "namespace N1.N2 { class A {} }",
    "namespace N3 {",
    "  using N1.N2;",
    "  class B : A {}",
    "}");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "N1", "N2", "A");
  CHECK_CLASS(class_b, class_a, "N3", "B");
}

TEST_F(ResolveClassPassTest, UsingNamespaceNotImportNested) {
  RUN_PARSER(
    "namespace N1.N2 { class A {} }",
    "namespace N3 {",
    "  using N1;",          // using doesn't import nested namespace.
    "  class B : N2.A {}",  // Error: N2 unknown
    "}");

  CHECK_PASS_ERROR("Resolve_Class_NotFound", 3, 13, "N2.A");
}

TEST_F(ResolveClassPassTest, UsingNamespaceHide) {
  RUN_PARSER(
    "namespace N1.N2 { class A {} }",
    "namespace N3 {",
    "  using N1.N2;",
    "  class A {}",
    "}");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "N1", "N2", "A");
  CHECK_CLASS(class_n3_a, _, "N3", "A");
  EXPECT_NE(class_a, class_n3_a);
}

TEST_F(ResolveClassPassTest, UsingNamespaceAmbiguous) {
  RUN_PARSER(
    "namespace N1 { class A {} }",
    "namespace N2 { class A {} }",
    "namespace N3 {",
    "  using N1;",
    "  using N2;",
    "  class B : A {}",  // Error: A is ambiguous
    "}");

  CHECK_PASS_ERROR("Resolve_Name_Ambiguous", 5, 13, "A");
}

TEST_F(ResolveClassPassTest, UsingNamespaceNotAmbiguous) {
  RUN_PARSER(
    "namespace N1 { class A {} }",
    "namespace N2 { class A {} }",
    "namespace N3 {",
    "  using N1;",
    "  using N2;",
    "  using A = N1.A;",
    "  class B : A {}",  // A is N1.A
    "}");

  RUN_PASSES();

  CHECK_CLASS(class_a, _, "N1", "A");
  CHECK_CLASS(class_b, class_a, "N3", "B");
}

TEST_F(ResolveClassPassTest, CircularlyDependency) {
  RUN_PARSER(
    "class A : B {}",
    "class B : C {}",
    "class C : A {}");

  CHECK_PASS_ERRORS(
    ErrorItem("Resolve_Class_CyclicBase", 0, 7, "A"),
    ErrorItem("Resolve_Class_CyclicBase", 1, 7, "B"),
    ErrorItem("Resolve_Class_CyclicBase", 2, 7, "C"));
}

TEST_F(ResolveClassPassTest, CircularlyDependencyNested) {
  RUN_PARSER(
    "class A : B.C {}",     // A depends on B and B.C.
    "class B : A {",        // B depends on A.
    "  public class C {}",  // C depends on B.
    "}");

  CHECK_PASS_ERRORS(
    ErrorItem("Resolve_Class_CyclicBase", 0, 7, "A"),
    ErrorItem("Resolve_Class_CyclicBase", 2, 16, "B.C"));
}

TEST_F(ResolveClassPassTest, NestedDependency) {
  RUN_PARSER(
    "class A {",
    "  class B : A {}",
    "}");

  RUN_PASSES();
  CHECK_CLASS(class_a, _, "A");
  CHECK_CLASS(class_b, class_a, "A", "B");
}

// Note: Interface doesn't have base class. So, this test is false.
#if 0
// Base class of interface class is always Object. Base interfaces are stored
// in interface list field.
TEST_F(ResolveClassPassTest, BaseInterface) {
  RUN_PARSER(
    "interface A {}",
    "interface B : A {}");
  RUN_PASSES();
  CHECK_CLASS(class_a, _, "A");
  CHECK_CLASS(class_b, _, "B");
}
#endif

TEST_F(ResolveClassPassTest, SelfBase) {
  RUN_PARSER("class A : A {}");
  // TypeVar for base class list allows us to report Resolve_Class_CyclicBase
  CHECK_PASS_ERROR("Resolve_Class_CyclicBase", 0, 7, "A");
}

#if 0
// TODO(yosi) 2012-01-09 NYI final class
TEST_F(ResolveClassPassTest, CircularlyDependencyNested) {
  RUN_PARSER(
    "final class A {}",
    "class B : A {}");

  CHECK_PASS_ERROR("Resolve_Class_NotFound", 3, 16, "N2.A");
}
#endif
} // CompilerTest
