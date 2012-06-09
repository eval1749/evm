#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractIlTest.h"

#include "Il/Fasl/TypeSorter.h"

namespace IlTest {

using namespace Il::Fasl;

class TypeSorterTest : public AbstractIlTest {
  protected: const Class& class_A;
  protected: const Class& class_B;
  protected: const Class& class_C;
  protected: const Class& class_D;

  protected: TypeSorterTest() 
      : class_A(NewClass("A")),
        class_B(NewClass("B", class_A)),
        class_C(NewClass("C", class_B)),
        class_D(NewClass("D", class_A)) {}
};

class TypeOrderMap {
  private: HashMap_<const Type*, int> type_order_map_;

  public: TypeOrderMap(const Class& a, const Class& b) {
    Type::Set type_set;
    type_set.Add(&a);
    type_set.Add(&b);
    ScopedPtr_<Type::Array> types(TypeSorter::Sort(type_set));
    Populate(*types);
  }

  public: TypeOrderMap(const Class& a, const Class& b, const Class& c) {
    Type::Set type_set;
    type_set.Add(&a);
    type_set.Add(&b);
    type_set.Add(&c);
    ScopedPtr_<Type::Array> types(TypeSorter::Sort(type_set));
    Populate(*types);
  }

  public: int Get(const Class& clazz) const {
    return type_order_map_.Get(&clazz);
  }

  private: void Populate(Type::Array& types) {
    foreach (Type::Array::Enum, en, types) {
      auto const clazz = *en;
      ASSERT(!type_order_map_.Contains(clazz));
      type_order_map_.Add(clazz, type_order_map_.Count() + 1);
    }
  }
};

// class A {}
// class B : A {}
TEST_F(TypeSorterTest, Basic1) {
  // A < B
  TypeOrderMap orders(class_A, class_B);
  EXPECT_LT(orders.Get(class_A), orders.Get(class_B));
}

// class A {}
// class B : A {}
// class C : B {}
TEST_F(TypeSorterTest, Basic2) {
  // A < B < C
  TypeOrderMap orders(class_A, class_B, class_C);
  EXPECT_LT(orders.Get(class_A), orders.Get(class_B));
  EXPECT_LT(orders.Get(class_B), orders.Get(class_C));
}

// class A {}
// class B : A {}
// class D : A {}
TEST_F(TypeSorterTest, Basic3) {
  // A < B < D
  TypeOrderMap orders(class_A, class_B, class_D);
  EXPECT_LT(orders.Get(class_A), orders.Get(class_B));
  EXPECT_LT(orders.Get(class_A), orders.Get(class_D));
}

// class A {
//   class AB {}
//   class AC {}
// }
TEST_F(TypeSorterTest, Nested) {
  auto& class_AB = NewClass(class_A, "AB");
  auto& class_AC = NewClass(class_A, "AC");

  // A < AB < AC
  TypeOrderMap orders(class_A, class_AB, class_AC);
  EXPECT_LT(orders.Get(class_A), orders.Get(class_AB));
  EXPECT_LT(orders.Get(class_A), orders.Get(class_AC));
}

// class Bar<T> {}
// class Foo : Bar<Foo> {}
TEST_F(TypeSorterTest, SelfRefernce) {
  auto& class_Bar = *new GenericClass(
    global_ns(),
    Modifier_Public | Modifier_ClassVariation_Interface,
    Name::Intern("Bar"),
    CollectionV_<const TypeParam*>(new TypeParam(Name::Intern("T"))));
  class_Bar.RealizeClass(CollectionV_<const Class*>());

  auto& class_Foo = *new Class(
    global_ns(),
    Modifier_Public,
    Name::Intern("Foo"));
  class_Foo.RealizeClass(
    CollectionV_<const Class*>(
        Ty_Object,
        &class_Bar.Construct(class_Foo)));

  // Bar < Foo
  TypeOrderMap orders(class_Bar, class_Foo);
  EXPECT_LT(orders.Get(class_Bar), orders.Get(class_Foo));
}

}  // IlTest
