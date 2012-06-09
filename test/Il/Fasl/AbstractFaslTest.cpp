#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractFaslTest.h"
#include "../Tasks/MockSession.h"

#include "Common/Io.h"
#include "Il/Fasl/FastWriter.h"

std::ostream& operator<<(
    std::ostream& os,
    const Common::Collections::Array_<uint8>& arr) {
  os << "{" << std::endl;
  char* comma = "";
  foreach (Common::Collections::Array_<uint8>::Enum, bytes, arr) {
    os << comma;
    comma = ", ";
    os << static_cast<int>(bytes.Get());
  }
  os << std::endl << "}";
  return os;
}

namespace IlTest {

using namespace Common::Io;
using namespace Il::Fasl;

Method& AbstractFaslTest::CreateMethod(
    Class& clazz,
    int const modifiers,
    const Name& name,
    const FunctionType& mty) {
  auto& mtg = *new MethodGroup(clazz, name);
  clazz.Add(name, mtg);

  auto& mt = *new Method(mtg, modifiers, mty);
  auto& fun = module().NewFunction(nullptr, Function::Flavor_Named, name);
  ValuesTypeBuilder builder(mty.params_type().Count() + 1);
  if (!mt.IsStatic()) {
    builder.Append(clazz);
  }

  foreach (ValuesType::Enum, types, mty.params_type()) {
    builder.Append(types.Get());
  }

  auto& params_type = builder.GetValuesType();
  auto& funty = FunctionType::Intern(mty.return_type(), params_type);

  fun.SetUp();
  fun.SetFunctionType(funty);
  mt.SetFunction(fun);
  mtg.Add(mt);

  return mt;
}

// class A {}
// class B : A {}
void AbstractFaslTest::MakeSampleClass(MemoryStream& mem) {
  auto& class_A = NewClass("A", *Ty_Object);
  auto& class_B = NewClass("B", class_A);

  MockSession session;
  FastWriter writer(session, mem, global_ns());
  writer.Add(class_A);
  writer.Add(class_B);
  writer.Finish();
}

// class Foo { Array<String> x; }
void AbstractFaslTest::MakeSampleConstructedClass(MemoryStream& mem) {
  auto& class_Foo = NewClass("Foo", *Ty_Object);

  auto& field_x = *new Field(
      class_Foo,
      Modifier_Private,
      Ty_Array->Construct(*Ty_String),
      Name::Intern("x"));

  class_Foo.Add(field_x.name(), field_x);

  MockSession session;
  FastWriter writer(session, mem, global_ns());
  writer.Add(class_Foo);
  writer.Finish();
}

// class Foo { T Bar<T>(); String Baz() { return Bar<String>(); } }
void AbstractFaslTest::MakeSampleConstructedMethod(MemoryStream& mem) {
  auto& class_Foo = NewClass("Foo", *Ty_Object);

  auto& mtg_Bar = *new MethodGroup(class_Foo, Name::Intern("Bar"));
  class_Foo.Add(mtg_Bar.name(), mtg_Bar);

  auto& ty_T = *new TypeParam(Name::Intern("T"));

  auto& gmt_Bar = *new GenericMethod(
      mtg_Bar,
      Modifier_Private,
      FunctionType::Intern(ty_T, ValuesType::Intern()),
      CollectionV_<const TypeParam*>(&ty_T));

  mtg_Bar.Add(gmt_Bar);

  TypeArgs ty_args(ty_T, *Ty_String);
  auto& method = gmt_Bar.Construct(ty_args);

  auto& zone = module().zone();
  auto& mt_Baz = CreateMethod(
      class_Foo,
      Modifier_Private,
      Name::Intern("Baz"),
      FunctionType::Intern(*Ty_String, ValuesType::Intern()));
  auto& fun = *mt_Baz.GetFunction();
  auto& bb = *fun.GetStartBB();
  auto& v1 = *new(zone) Values();
  bb.AppendI(*new(zone) ValuesI(v1, *new Int32Literal(1)));
  auto& r2 = module().NewRegister();
  bb.AppendI(*new(zone) CallI(*Ty_String, r2, method, v1));
  bb.AppendI(*new(zone) RetI(r2));

  MockSession session;
  FastWriter writer(session, mem, global_ns());
  writer.Add(class_Foo);
  writer.Finish();
}

// class Foo { int x; }
void AbstractFaslTest::MakeSampleField(MemoryStream& mem) {
  auto& class_Foo = NewClass("Foo", *Ty_Object);

  auto& field_x = *new Field(
      class_Foo,
      Modifier_Private,
      *Ty_Int32,
      Name::Intern("x"));

  class_Foo.Add(field_x.name(), field_x);

  MockSession session;
  FastWriter writer(session, mem, global_ns());
  writer.Add(class_Foo);
  writer.Finish();
}

// class Foo<T> { T x; }
void AbstractFaslTest::MakeSampleGenericClass(MemoryStream& mem) {
  auto& type_param_T = *new TypeParam(Name::Intern("T"));

  auto& class_Foo = *new GenericClass(
      global_ns(),
      Modifier_Public,
      Name::Intern("Foo"),
      CollectionV_<const TypeParam*>(&type_param_T));

  class_Foo.RealizeClass(CollectionV_<const Class*>(Ty_Object));

  auto& field_x = *new Field(
      class_Foo,
      Modifier_Private,
      type_param_T,
      Name::Intern("x"));

  class_Foo.Add(field_x.name(), field_x);

  MockSession session;
  FastWriter writer(session, mem, global_ns());
  writer.Add(class_Foo);
  writer.Finish();
}

// class Foo { extern int Bar(); }
void AbstractFaslTest::MakeSampleMethod(MemoryStream& mem) {
  auto& class_Foo = NewClass("Foo", *Ty_Object);

  auto& method_group_Bar = *new MethodGroup(
      class_Foo,
      Name::Intern("Bar"));

  class_Foo.Add(method_group_Bar.name(), method_group_Bar);

  auto& method_Bar_void = *new Method(
      method_group_Bar,
      Modifier_Extern | Modifier_Private,
      FunctionType::Intern(*Ty_Int32, ValuesType::Intern()));

  method_group_Bar.Add(method_Bar_void);

  MockSession session;
  FastWriter writer(session, mem, global_ns());
  writer.Add(class_Foo);
  writer.Finish();
}

// class Foo { private int Bar() { ... } }
void AbstractFaslTest::MakeSampleMethodBody(MemoryStream& mem) {
  auto& class_Foo = NewClass("Foo", *Ty_Object);

  auto& mt = CreateMethod(
    class_Foo,
    Modifier_Private,
    Name::Intern("Bar"),
    FunctionType::Intern(*Ty_Int32, ValuesType::Intern()));

  auto& fun = *mt.GetFunction();
  auto& zone = module().zone();
  fun.GetStartBB()->AppendI(*new(zone) RetI(*new(zone) Int32Literal(1234)));

  MockSession session;
  FastWriter writer(session, mem, global_ns());
  writer.Add(class_Foo);
  writer.Finish();
}

// class Foo { public int Size { get { return 1234; } } }
void AbstractFaslTest::MakeSampleProperty(MemoryStream& mem) {
  auto& class_Foo = NewClass("Foo", *Ty_Object);
  auto& name_Size = Name::Intern("Size");

  auto& property_Size = *new Property(
      class_Foo,
      Modifier_Public,
      *Ty_Int32,
      name_Size);

  class_Foo.Add(name_Size, property_Size);

  auto& mt = CreateMethod(
    class_Foo,
    property_Size.modifiers() | Modifier_SpecialName,
    Name::Intern("get_Size"),
    FunctionType::Intern(*Ty_Int32, ValuesType::Intern()));

  property_Size.Add(Name::Intern("get"), mt);

  auto& fun = *mt.GetFunction();
  auto& zone = module().zone();
  fun.GetStartBB()->AppendI(*new(zone) RetI(*new(zone) Int32Literal(1234)));

  MockSession session;
  FastWriter writer(session, mem, global_ns());
  writer.Add(class_Foo);
  writer.Finish();
}

ByteArray* AbstractFaslTest::ToByteArray(MemoryStream& mem) {
  ArrayList_<uint8> list;
  uint8 buf[100];
  while (auto const count = mem.Read(buf, sizeof(buf))) {
    for (int i = 0; i < count; i++) {
      list.Add(buf[i]);
    }
  }
  return list.ToArray();
}

}  // IlTest
