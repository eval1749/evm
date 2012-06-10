#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Class
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (eval1749@)

#include "./GenericClass.h"

#include "./ConstructedClass.h"
#include "./ConstructedMethod.h"
#include "./Method.h"
#include "./MethodGroup.h"
#include "./Property.h"
#include "./ValuesType.h"
#include "../Name.h"

//#include "../../../Common/Collections/Array_.h"
//#include "../../../Common/Collections/Collection_.h"

namespace Il {
namespace Ir {

// ctor
GenericClass::GenericClass(
    const ClassOrNamespace& outer,
    int const modifiers,
    const Name& name,
    const Collection_<const TypeParam*>& type_params)
    : Base(outer, modifiers, name),
      type_params_(type_params) {
  ASSERT(type_params.Count() > 0);
  foreach (TypeParams::Enum, oEnum, type_params) {
    auto& type_param = *const_cast<TypeParam*>(oEnum.Get());
    type_param.BindTo(*this);
    Add(type_param.name(), type_param);
  } // for
}

GenericClass::~GenericClass() {
  DEBUG_PRINTF("%p\n", this);
}

// [C]
ConstructedClass& GenericClass::Construct(const Type& a1) {
  TypeParams::Enum type_params(type_params_);
  auto const p1 = type_params.Get();
  TypeArgs type_args(*p1, a1);
  return static_cast<ConstructedClass&>(Construct(type_args));
}

Type& GenericClass::Construct(const TypeArgs& type_args) const {
  if (auto const present = cons_class_map_.Get(&type_args)) {
    return *present;
  } // if

  auto& cons_class = *new ConstructedClass(*this, type_args);
  cons_class_map_.Add(&cons_class.type_args(), &cons_class);

  Class::List base_list;
  foreach (EnumBaseSpec, bases, *this) {
    auto& base = bases.Get();
    auto& clazz = static_cast<Class&>(base.Construct(type_args));
    base_list.Add(&clazz);
  }

  cons_class.RealizeClass(base_list);

  foreach (EnumMember, members, *this) {
    Operand& member = *members;
    if (auto const mtg = member.DynamicCast<MethodGroup>()) {
      auto& cons_mtg = *new MethodGroup(cons_class, mtg->name());
      cons_class.Add(mtg->name(), cons_mtg);

      foreach (MethodGroup::EnumMethod, methods, mtg) {
        Method& method = *methods;
        if (auto const gen_method = method.DynamicCast<GenericMethod>()) {
          cons_mtg.Add(gen_method->Construct(type_args));
        } else {
          cons_mtg.Add(cons_class.ConstructMethod(cons_mtg, method));
        }
      } // for method
    } // if
  } // for member

  // Construct properties
  foreach (EnumMember, members, *this) {
    auto& member = *members;
    if (auto const gen_prop = member.DynamicCast<Property>()) {
      auto& cons_prop = *new Property(
          cons_class,
          gen_prop->modifiers(),
          gen_prop->property_type().Construct(type_args),
          gen_prop->name());
      cons_class.Add(cons_prop.name(), cons_prop);
      foreach (Property::EnumMember, members, *gen_prop) {
        auto& method_name = Name::Intern(
            String::Format("%s_%s", 
                *members.Get().GetKey(),
                cons_prop.name()));
        cons_prop.Add(
            *members.Get().GetKey(),
            *cons_class.Find(method_name)
                ->DynamicCast<MethodGroup>()
                    ->Find(
                        FunctionType::Intern(
                            cons_prop.property_type(),
                            ValuesType::Intern())));
      }
    }
  }

  foreach (EnumField, fields, this) {
    Field& field = *fields;

    auto& cons_field = *new Field(
        cons_class,
        field.GetModifiers(),
        field.field_type().Construct(type_args),
        field.name(),
        field.GetSourceInfo());

    cons_class.Add(cons_field.name(), cons_field);
  } // for

  return cons_class;
} // Construct

// [G]
const Collection_<const TypeParam*>& GenericClass::GetTypeParams() const {
  return type_params_;
}

// [T]
String GenericClass::ToString() const {
  StringBuilder builder;
  builder.Append(Base::ToString());
  builder.Append('<');
  const char* comma = "";
  foreach (EnumTypeParam, type_params, *this) {
    auto& type_param = *type_params.Get();
    builder.Append(comma);
    comma = ", ";
    builder.Append(type_param.name().ToString());
  }
  builder.Append('>');
  return builder.ToString();
}

} // Ir
} // Il
