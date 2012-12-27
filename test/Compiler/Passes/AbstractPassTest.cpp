#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - AbstractParseTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractPassTest.h"

#include "Compiler/CompileSession.h"

namespace CompilerTest {

using namespace Compiler;

Namespace& AbstractPassTest::global_ns() const {
  return session().global_namespace();
}

// [F]
Class* AbstractPassTest::FindClass(const String& name) const {
  auto const thing = global_ns().Find(Name::Intern(name));
  return thing->DynamicCast<Class>();
}

Class* AbstractPassTest::FindClass(
    const String& name1,
    const String& name) const {
  auto const ns = FindNamespace(name1);
  return ns ? ns->Find(Name::Intern(name))->DynamicCast<Class>() : nullptr;
}

Class* AbstractPassTest::FindClass(
    const String& name1,
    const String& name2,
    const String& name) const {
  auto const ns = FindNamespace(name1, name2);
  return ns ? ns->Find(Name::Intern(name))->DynamicCast<Class>() : nullptr;
}

Method* AbstractPassTest::FindMethod(
    Class* const clazz,
    const String& name,
    Type& rety,
    Array_<Type*>* const paramtys) const {
  ASSERT(clazz != nullptr);
  ASSERT(paramtys != nullptr);

  auto const method_group = FindMethodGroup(*clazz, name);
  if (method_group == nullptr) {
    return nullptr;
  }

  for (auto& method: method_group->methods()) {
    auto& funty = method.function_type();
    if (funty.return_type() != rety) {
      continue;
    }

    // Match parameter types
    Array_<Type*>::Enum actuals(paramtys);
    FunctionType::EnumParamType  expecteds(funty);

    while (!actuals.AtEnd() && !expecteds.AtEnd()) {
      auto& expected = expecteds.Get();
      auto& actual = *actuals.Get();
      if (expected != actual) {
        break;
      }
      actuals.Next();
      expecteds.Next();
    }

    if (actuals.AtEnd() && expecteds.AtEnd())
      return &method;
  } // for method
  return nullptr;
}

Method* AbstractPassTest::FindMethod(
    Class* clazz, const String& name, Type& rety) const {
  Array_<Type*> paramtys(0);
  return FindMethod(clazz, name, rety, &paramtys);
}

Method* AbstractPassTest::FindMethod(
    Class* clazz, const String& name, Type& rety, Type* ty1) const {
  Type* v[] = { ty1 };
  Array_<Type*> paramtys(v, lengthof(v));
  return FindMethod(clazz, name, rety, &paramtys);
}

MethodGroup* AbstractPassTest::FindMethodGroup(
    const Class& clazz, const String& name) const {
  return clazz.Find(Name::Intern(name))->DynamicCast<MethodGroup>();
}

ClassOrNamespace* AbstractPassTest::FindNamespace(
    const String& name) const {
  if (auto const thing = global_ns().Find(Name::Intern(name))) {
    return thing->DynamicCast<ClassOrNamespace>();
  }

  if (auto const thing = Namespace::Global->Find(Name::Intern(name))) {
    return thing->DynamicCast<ClassOrNamespace>();
  }

  return nullptr;
}

ClassOrNamespace* AbstractPassTest::FindNamespace(
    const String& name1,
    const String& name) const {
  auto const ns = FindNamespace(name1);
  return ns->Find(Name::Intern(name))->DynamicCast<ClassOrNamespace>();
}

ClassOrNamespace* AbstractPassTest::FindNamespace(
    const String& name1,
    const String& name2,
    const String& name) const {
  auto const ns = FindNamespace(name1, name2);
  return ns->Find(Name::Intern(name))->DynamicCast<ClassOrNamespace>();
}

// [R]
const char16* AbstractPassTest::RunPasses() {
  foreach (
      Collection_<CompilePass::NewFunction>::Enum,
      passes,
      CompilePass::GetPasses()) {
    auto const ctor = passes.Get();
    auto const pass = ctor(&session());
    pass->Start();
    auto const name = pass->name();
    delete pass;

    if (session().HasError()) {
      return name;
    }

    if (ctor == stop_) {
      break;
    }
  }

  return L"";
}

} // CompilerTest
