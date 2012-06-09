#include "precomp.h"
// @(#)$Id$
//
// Evita Lexer Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include <ostream>

#include "Compiler/CompilationUnit.h"
#include "Compiler/CompileSession.h"
#include "Compiler/Ir/ClassDef.h"
#include "Compiler/Ir/MethodDef.h"
#include "Compiler/Ir/NamespaceBody.h"
#include "Compiler/Ir/NamespaceDef.h"
#include "Compiler/Syntax/Parser.h"
#include "Compiler/VisitFunctor.h"

#include "Common.h"

#include "./AbstractParserTest.h"
#include "../DomBuilder.h"
#include "../DomNode.h"

// Note: MSVC doesn't use operator<<(std::ostream, Dom::Node*) if this
// function in anonymous namespace.
std::ostream& operator<<(std::ostream& os, const Dom::Node* const node) {
  return os << node->ToString().value();
}

namespace CompilerTest {

using namespace Compiler;

namespace {

class DomTreeBuilder : public VisitFunctor {
  private: Dom::Builder builder_;
  private: String name_;
  private: String outer_;
  private: String owner_;

  public: DomTreeBuilder()
      : name_(builder_.InternName("name")),
        outer_(builder_.InternName("outer")),
        owner_(builder_.InternName("owner")) {
  }

  public: virtual ~DomTreeBuilder() {}

  // [F]
  public: Dom::Document& Finish() {
    return builder_.Finish();
  }

  // [G]
  private: String GetTypeName(const Type& type) {
    if (auto const p = type.DynamicCast<Class>()) {
      return p->name();
    }

    if (auto const p = type.DynamicCast<UnresolvedType>()) {
      return p->name_ref().ToString();
    }

    return String("???");
  }

  // [P]
  private: virtual void Process(ClassDef* const clazz) override {
    builder_.StartElement("class")
        .SetAttribute(name_, clazz->name())
        .SetAttribute(outer_, clazz->owner().name());

    foreach (ClassDef::EnumBaseSpec, bases, *clazz) {
      auto& base = *bases.Get()->StaticCast<Class>();
      auto const name = GetTypeName(base);
      if (!name.IsEmpty()) {
        builder_.StartElement("base").SetAttribute(name_, name);
        builder_.EndElement("base");
      }
    }

    foreach (ClassDef::EnumMember, names, *clazz) {
      auto const member = names.Get();
      member->Apply(this);
    }

    builder_.EndElement("class");
  }

  private: virtual void Process(CompilationUnit* const cu) override {
    ProcessAux(&cu->namespace_body());
  }

  private: virtual void Process(MethodDef* const mt) override {
    builder_.StartElement("method");
    builder_.SetAttribute(name_, mt->name());
    ProcessModifiers(mt->GetModifiers());
    builder_.EndElement("method");
  }

  private: virtual void Process(NamespaceBody* const body) override {
    auto& ns_def = body->namespace_def();

    if (ns_def.outer() == &ns_def) {
      ProcessAux(body);
      return;
    }

    builder_.StartElement("namespace")
        .SetAttribute(name_, ns_def.name())
        .SetAttribute( owner_,  ns_def.outer()->name());

    ProcessAux(body);

    builder_.EndElement("namespace");
  }

  private: virtual void Process(UsingNamespace* const uzing) {
    builder_.StartElement("using")
        .SetAttribute(name_, uzing->name_ref().ToString());
    builder_.EndElement("using");
  }

  private: void ProcessAux(NamespaceBody* const body) {
    foreach (NamespaceBody::EnumMember, en, *body) {
      auto const member = en.Get();
      member->Apply(this);
    }
  }

  private: void ProcessModifiers(int const mods) {
    #define DEFMODIFIER(mp_Name, mp_name, mp_nth) \
      if ((mods & Modifier_ ## mp_Name) != 0) { \
        builder_.SetAttribute(#mp_name, "1"); \
      }

    #include "../../../src/Il/Ir/Modifiers.inc"
  }
};

}  // namespace

class ParserTest : public AbstractParserTest {
  // [M]
  protected: Dom::Document& MakeDomTree() {
    DomTreeBuilder builder;
    compilation_unit().Apply(&builder);
    return builder.Finish();
  }
};

#define CHECK_PARSE_TREE() \
  auto& actual_tree = MakeDomTree(); \
  auto& expected_tree = bd.Finish(); \
  EXPECT_PRED2(Dom::Node::TreeEquals, expected_tree, actual_tree); 

#define RUN_PARSER_FOR_STATEMENT(stmts) \
    RUN_PARSER("class Foo {\nvoid Main() {\n" stmts "\n}\n}\n")


TEST_F(ParserTest, Namespace_NestingFullForm) {
  RUN_PARSER(
    "namespace A {",
    "  namespace B {",
    "    namespace C {}",
    "  }",
    "}");

  Dom::Builder bd;
  bd.StartElement("namespace", "name", "A", "owner", "::");
  bd.StartElement("namespace", "name", "B", "owner", "A");
  bd.EmptyElement("namespace", "name", "C", "owner", "B");
  bd.EndElement("namespace");
  bd.EndElement("namespace");

  CHECK_PARSE_TREE();
}

// Short form "A.B.C" creates same parse tree of full form but namespace
// names aren't appeared.
TEST_F(ParserTest, Namespace_NestingShortForm) {
  RUN_PARSER(
    "namespace A.B.C {",
    "}");

  Dom::Builder bd;
  bd.EmptyElement("namespace", "name", "C", "owner", "B");

  CHECK_PARSE_TREE();
}

TEST_F(ParserTest, Namespace_N1_N2_A_B) {
  RUN_PARSER(
    "namespace N1.N2 {",
    "   class A {}"
    "   class B {}"
    "}");

  Dom::Builder bd;
  bd.StartElement("namespace", "name", "N2", "owner", "N1");

  bd.EmptyElement("class", "name", "A", "outer", "N2");
  bd.EmptyElement("class", "name", "B", "outer", "N2");

  bd.EndElement("namespace");

  CHECK_PARSE_TREE();
}

TEST_F(ParserTest, Namespace_N1_N2_A_B_OpenEnd) {
  RUN_PARSER(
    "namespace N1.N2 { class A {} }",
    "namespace N1.N2 { class B {} }");

  Dom::Builder bd;

  // namespace N1.N2
  bd.StartElement("namespace", "name", "N2", "owner", "N1");
  bd.EmptyElement("class", "name", "A", "outer", "N2");
  bd.EndElement("namespace");

  // namespace N1.N2
  bd.StartElement("namespace", "name", "N2", "owner", "N1");
  bd.EmptyElement("class", "name", "B", "outer", "N2");
  bd.EndElement("namespace");

  CHECK_PARSE_TREE();
}

TEST_F(ParserTest, Class_A_B) {
  RUN_PARSER(
    "class A {}"
    "class B : A {}");

  Dom::Builder bd;
  bd.EmptyElement("class", "name", "A", "outer", "::");

  bd.StartElement("class", "name", "B", "outer", "::");
  bd.StartElement("base", "name", "A");
  bd.EndElement("base");
  bd.EndElement("class");

  CHECK_PARSE_TREE();
}

TEST_F(ParserTest, Class_HelloWorld) {
  RUN_PARSER(
    "using Common;"
    "class A {"
    "  public static void Main(String[] argv) {"
    "    Console.WriteLine(\"Hello World!\");"
    "  }"
    "}");

  Dom::Builder bd;
  // TODO(yosi) 2012-01-03 Emit "using" statement into parse tree.
  //bd.EmptyElement("using", "name", "Common");

  bd.StartElement("class", "name", "A", "outer", "::");

  bd.StartElement("method", "name", "Main");
  bd.SetAttribute("public", "1");
  bd.SetAttribute("static", "1");
  bd.EndElement("method");

  bd.EndElement("class");


  CHECK_PARSE_TREE();
}

// Parser doesn't detect circulaly reference in base list.
TEST_F(ParserTest, Class_SelfBase) {
  RUN_PARSER("class A : A {}");
}

TEST_F(ParserTest, ConstructedClass) {
  RUN_PARSER(
    "class Foo : Comparable<Foo> {",
    "  private int x_;",
    "  public Foo(int x) { x_ = x; }",
    "  public int Comparable<Foo>.CompareTo(Foo another) {",
    "   return x_ - another.x_;",
    "  }",
    "}")
}

TEST_F(ParserTest, EnumBasic) {
  RUN_PARSER("enum Color { Red, Green, Blue }");
}

TEST_F(ParserTest, EnumBasicComma) {
  RUN_PARSER("enum Color { Red, Green, Blue, }");
}

TEST_F(ParserTest, EnumBase) {
  RUN_PARSER("enum Color : int64 { Red, Green, Blue, }");
}

TEST_F(ParserTest, EnumBadValue) {
  RUN_PARSER("enum Color : uint { Red = -1, Green = -2, Blue = -3 }");
}

TEST_F(ParserTest, EnumShareValue) {
  RUN_PARSER("enum Color : uint { Red, Green, Blue, Max = Blue }");
}

TEST_F(ParserTest, EnumValue) {
  RUN_PARSER("enum Color : uint { Red, Green = 10, Blue }");
}

TEST_F(ParserTest, EnumCircular) {
  RUN_PARSER("enum Circular { A = B, B }");
}

TEST_F(ParserTest, ExprCondAnd) {
  RUN_PARSER(
    "class Foo {"
    "  bool Bar(int a, int b) {"
    "   return a == 0 && b == 0;"
    "  }"
    "}");
}

TEST_F(ParserTest, ExprCondOr) {
  RUN_PARSER(
    "class Foo {"
    "  bool Bar(int a, int b) {"
    "   return a == 0 || b == 0;"
    "  }"
    "}");
}

TEST_F(ParserTest, ExternMethod) {
  RUN_PARSER(
    "class Foo {",
    "  public extern int Action1();",
    "  public static extern int Action2();",
    "}");
}

TEST_F(ParserTest, GenericClass) {
  RUN_PARSER(
    "class Foo<T> {"
    "  private T value;"
    "  public Foo(T value) {"
    "   this.value = value;"
    "}"
    "  public T Get() {"
    "    return this.value;"
    "  }"
    "}")
}

TEST_F(ParserTest, GenericMethod) {
  RUN_PARSER(
    "class Foo {",
    "  T Action<T>(T x) { return x; }",
    "}");
}

TEST_F(ParserTest, Indexer) {
  RUN_PARSER(
    "class Foo {",
    "  private char[] chars_;",
    "  public Foo(int n) { chars_ = new chars_[n]; }",
    "  public char this[int index] {",
    "    get { return chars_[index]; }",
    "    set { chars_[index] = value; }",
    "  }",
    "}");
}

TEST_F(ParserTest, NewArray) {
  RUN_PARSER(
    "class Foo {"
    "  public char[] Main() {"
    "    return new char[10];"
    "  }"
    "}");
}

TEST_F(ParserTest, NewArrayArray) {
  RUN_PARSER(
    "class Foo {"
    "  public char[][] Main() {"
    "    return new char[10][];"
    "  }"
    "}");
}

TEST_F(ParserTest, Property_Basic) {
  RUN_PARSER(
    "class Foo {\n"
    "  private int state_;"
    "  public int State {\n"
    "    get { return state_; }\n"
    "    set { state_ = value; }\n"
    "  }\n"
    "}");
}

TEST_F(ParserTest, Property_Automatic) {
  RUN_PARSER(
    "class Foo {\n"
    "  public int State { get; set; }\n"
    "}");
}

TEST_F(ParserTest, Stmt_Return_Expr) {
  RUN_PARSER(
    "class Foo {\n"
    "  int Bar() { return 1; }\n"
    "}");
}

TEST_F(ParserTest, Stmt_Return_Void) {
  RUN_PARSER(
    "class Foo {\n"
    "  void Bar() { return; }\n"
    "}");
}

TEST_F(ParserTest, Stmt_Using_Expr) {
  RUN_PARSER_FOR_STATEMENT("using ((Method())) { DoSomething(); }");
}

TEST_F(ParserTest, Stmt_Using_Name) {
  RUN_PARSER_FOR_STATEMENT("using (Method()) { DoSomething(); }");
}

TEST_F(ParserTest, Stmt_Using_Type) {
  RUN_PARSER_FOR_STATEMENT("using (Foo x = Method()) { DoSomething(); }");
}

TEST_F(ParserTest, Stmt_Using_Var) {
  RUN_PARSER_FOR_STATEMENT("using (var x = Method()) { DoSomething(); }");
}

} // CompilerTest
