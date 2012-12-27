#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractFaslTest.h"

#include "Il/Fasl/FastLoader.h"
#include "../Tasks/MockSession.h"

namespace IlTest {

using namespace Il::Fasl;

class FastLoaderTest : public AbstractFaslTest {
  Namespace& another_ns_;

  protected: FastLoaderTest()
    : another_ns_(Namespace::CreateGlobalNamespace()) {}

  protected: Class* FindClass(const char* const name) {
    return FindClassOrNamespace(name)->DynamicCast<Class>();
  }

  protected: ClassOrNamespace* FindClassOrNamespace(const char* const name) {
    return another_ns_.Find(Name::Intern(name))
        ->DynamicCast<ClassOrNamespace>();
  }

  protected: Class* FindClass(
      const char* const name,
      const char* const name2) {
    if (auto const outer = FindClassOrNamespace(name)) {
      return outer->Find(Name::Intern(name2))->DynamicCast<Class>();
    }
    return nullptr;
  }

  protected: bool Load(Session& session, Stream& stream) {
    FastLoader loader(session, another_ns_,  "mem");
    for (;;) {
      uint8 buf[100];
      auto const count = stream.Read(buf, sizeof(buf));
      if (!count) break;
      loader.Feed(buf, count);
      if (loader.HasError()) break;
    }
    loader.Finish();
    return !loader.HasError();
  }
};

// class A {}
// class B : A {}
TEST_F(FastLoaderTest, Class) {
  MemoryStream mem;
  MakeSampleClass(mem);
  MockSession session;
  ASSERT_TRUE(Load(session, mem));

  auto const class_A = FindClass("A");
  ASSERT_TRUE(class_A != nullptr);

  auto const class_B = FindClass("B");
  ASSERT_TRUE(class_B != nullptr);

  EXPECT_EQ(*class_A, *class_B->GetBaseClass());
}

// class Foo { Array<String> x; }
TEST_F(FastLoaderTest, ConstructedClass) {
  MemoryStream mem;
  MakeSampleConstructedClass(mem);
  MockSession session;
  ASSERT_TRUE(Load(session, mem));

  auto const class_Foo = FindClass("Foo");
  ASSERT_TRUE(class_Foo != nullptr);

  auto const field_x = class_Foo->Find(Name::Intern("x"))
      ->DynamicCast<Field>();
  ASSERT_TRUE(field_x != nullptr);
  EXPECT_EQ(*class_Foo, field_x->owner_class());
  auto& ty_string_arr = Ty_Array->Construct(*Ty_String);
  EXPECT_EQ(ty_string_arr, field_x->field_type());
}

// class Foo { int x; }
TEST_F(FastLoaderTest, Field) {
  MemoryStream mem;
  MakeSampleField(mem);
  MockSession session;
  ASSERT_TRUE(Load(session, mem));

  auto const class_Foo = FindClass("Foo");
  ASSERT_TRUE(class_Foo != nullptr);

  auto const field_x = class_Foo->Find(Name::Intern("x"))
      ->DynamicCast<Field>();
  ASSERT_TRUE(field_x != nullptr);
  EXPECT_EQ(*class_Foo, field_x->owner_class());
  EXPECT_EQ(*Ty_Int32, field_x->field_type());
}

// class Foo<T> { T x; }
TEST_F(FastLoaderTest, GenericClass) {
  MemoryStream mem;
  MakeSampleGenericClass(mem);
  MockSession session;
  ASSERT_TRUE(Load(session, mem));

  auto const class_Foo = FindClass("Foo");
  ASSERT_TRUE(class_Foo != nullptr);

  auto& gen_class = *class_Foo->StaticCast<GenericClass>();

  const TypeParam* type_param = nullptr;
  foreach (GenericClass::EnumTypeParam, type_params, gen_class) {
    ASSERT_TRUE(type_param == nullptr);
    type_param = type_params.Get();
  }
  ASSERT_TRUE(type_param != nullptr);
  EXPECT_EQ(Name::Intern("T"), type_param->name());

  auto const field_x = class_Foo->Find(Name::Intern("x"))
      ->DynamicCast<Field>();
  ASSERT_TRUE(field_x != nullptr);
  EXPECT_EQ(*class_Foo, field_x->owner_class());
  EXPECT_EQ(*type_param, field_x->field_type());
}

// class Foo { extern int Bar(); }
TEST_F(FastLoaderTest, Method) {
  MemoryStream mem;
  MakeSampleMethod(mem);
  MockSession session;
  ASSERT_TRUE(Load(session, mem));

  auto const class_Foo = FindClass("Foo");
  ASSERT_TRUE(class_Foo != nullptr);

  auto const method_group_Bar = class_Foo->Find(Name::Intern("Bar"))
    ->DynamicCast<MethodGroup>();
  ASSERT_TRUE(!!method_group_Bar);
  EXPECT_EQ(*class_Foo, method_group_Bar->owner_class());
  ASSERT_EQ(1, method_group_Bar->methods().Count());
  const auto& method = *method_group_Bar->methods().GetFirst();
  EXPECT_EQ(*method_group_Bar, method.method_group());
  EXPECT_EQ(*Ty_Int32, method.return_type());
  EXPECT_EQ(Modifier_Extern | Modifier_Private, method.modifiers());
}

// class Foo { int Bar() { ... } }
TEST_F(FastLoaderTest, MethodBody) {
  MemoryStream mem;
  MakeSampleMethodBody(mem);
  MockSession session;
  ASSERT_TRUE(Load(session, mem));

  auto const class_Foo = FindClass("Foo");
  ASSERT_TRUE(class_Foo != nullptr);

  auto const method_group_Bar = class_Foo->Find(Name::Intern("Bar"))
    ->DynamicCast<MethodGroup>();
  ASSERT_TRUE(!!method_group_Bar);
  EXPECT_EQ(*class_Foo, method_group_Bar->owner_class());
  ASSERT_EQ(1, method_group_Bar->methods().Count());
  const auto& method = *method_group_Bar->methods().GetFirst();
  EXPECT_EQ(*method_group_Bar, method.method_group());
  EXPECT_EQ(*Ty_Int32, method.return_type());
  EXPECT_EQ(Modifier_Private, method.modifiers());
  ASSERT_TRUE(method.GetFunction() != nullptr);
  const auto& fun = *method.GetFunction();

  EXPECT_EQ(
      FunctionType::Intern(*Ty_Int32, ValuesType::Intern(*class_Foo)),
      fun.function_type());

  auto const last_inst = fun.GetStartBB()->GetLastI();
  EXPECT_EQ(Op_Ret, last_inst->GetOp());

  auto const lit = last_inst->GetLx();
  ASSERT_TRUE(lit != nullptr);
  EXPECT_EQ(*Ty_Int32, lit->GetTy());
  EXPECT_EQ(1234, lit->GetInt32());
}

TEST_F(FastLoaderTest, Property) {
  MemoryStream mem;
  MakeSampleProperty(mem);
  MockSession session;
  ASSERT_TRUE(Load(session, mem));

  auto const class_Foo = FindClass("Foo");
  ASSERT_TRUE(class_Foo != nullptr);

  auto const property_x = class_Foo->Find(Name::Intern("Size"))
      ->DynamicCast<Property>();
  ASSERT_TRUE(property_x != nullptr);
  EXPECT_EQ(*class_Foo, property_x->owner_class());
  EXPECT_EQ(*Ty_Int32, property_x->property_type());

  auto const getter = property_x->Get(Name::Intern("get"));
  ASSERT_TRUE(getter != nullptr);
  EXPECT_TRUE((getter->modifiers() & Modifier_SpecialName) != 0);
  EXPECT_EQ(
      FunctionType::Intern(
          property_x->property_type(), ValuesType::Intern()),
      getter->function_type());
}

}  // IlTest
