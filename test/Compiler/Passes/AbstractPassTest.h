// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Compiler_Passes_AbstractPassTest_h)
#define INCLUDE_test_Compiler_Passes_AbstractPassTest_h

#include <ostream>

#include "../Syntax/AbstractParserTest.h"
#include "Compiler/CompilePass.h"
#include "Il/Ir/Name.h"


namespace CompilerTest {

using namespace Compiler;

class AbstractPassTest : public AbstractParserTest {
  private: CompilePass::NewFunction stop_;

  // ctor
  protected: AbstractPassTest(CompilePass::NewFunction stop)
      : stop_(stop) {
  }

  // proprties
  protected: Namespace& global_ns() const;

  // [F]
  protected: Class* FindClass(const String&) const;
  protected: Class* FindClass(const String&, const String&) const;
  protected: Class* FindClass(const String&, const String&,
                              const String&) const;

  protected: Method* FindMethod(
    Class*, const String&, Type&, Array_<Type*>*) const;

  protected: Method* FindMethod(Class*, const String&, Type&) const;

  protected: Method* FindMethod(
    Class*, const String&, Type&, Type*) const;

  protected: MethodGroup* FindMethodGroup(const Class&, const String&) const;

  protected: ClassOrNamespace* FindNamespace(const String&) const;
  protected: ClassOrNamespace* FindNamespace(const String&, const String&) const;

  protected: ClassOrNamespace* FindNamespace(
      const String&, const String&, const String&) const;

  // [R]
  protected: const char16* RunPasses();

  DISALLOW_COPY_AND_ASSIGN(AbstractPassTest);
};

#define _ Ty_Object

#define CHECK_CLASS(var, base_class, ...) \
  auto const var = FindClass(__VA_ARGS__); \
  ASSERT_TRUE(var != nullptr); \
  EXPECT_EQ(Box(base_class), Box(var->GetBaseClass()));

#define CHECK_FIELD(owner, type, var, name) \
  auto const var = owner->Find(Name::Intern(name))->DynamicCast<Field>(); \
  ASSERT_TRUE(var != nullptr); \
  EXPECT_EQ(type, var->storage_type());

#define CHECK_CONSTANT_FIELD(ty, expected, var) \
  if (auto const actual = var->GetOperand()->DynamicCast<ty ## Literal>()) { \
    EXPECT_EQ(expected, actual->value()); \
  } else { \
    FAIL() << "Field " << var->name() << " isn't constant."; \
  }

#define CHECK_METHOD(class, name, rety, ...) \
  auto const method_ ## name = FindMethod(class, #name, rety, __VA_ARGS__); \
  ASSERT_TRUE(method_ ## name != nullptr); \

#define CHECK_PASS_ERROR(code, line, column, ...) \
  RUN_PASSES_WITH_ERROR(); \
  auto actual_tree = GetErrors(); \
  auto expected_tree = MakeErrorTree( \
      ErrorItem(code, line, column, __VA_ARGS__)); \
  EXPECT_EQ(expected_tree, actual_tree);

#define CHECK_PASS_ERRORS(...) \
  RUN_PASSES_WITH_ERROR(); \
  auto actual_tree = GetErrors(); \
  auto expected_tree = MakeErrorTree(__VA_ARGS__); \
  EXPECT_EQ(expected_tree, actual_tree);


#define RUN_PASSES() { \
  auto const pass_name = RunPasses(); \
  ASSERT_FALSE(session().HasError()) << "In " << pass_name; \
}

#define RUN_PASSES_WITH_ERROR() { \
  auto const pass_name = RunPasses(); \
  ASSERT_TRUE(session().HasError()) << "In " << pass_name; \
}

} // CompilerTest

#endif // !defined(INCLUDE_test_Compiler_Passes_AbstractPassTest_h)
