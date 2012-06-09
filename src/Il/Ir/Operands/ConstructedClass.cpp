#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- ConstructedClass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ConstructedClass.h"

#include "./Function.h"
#include "./GenericClass.h"
#include "./GenericMethod.h"
#include "./Label.h"
#include "./Method.h"
#include "./MethodGroup.h"
#include "../Module.h"
#include "./Namespace.h"
#include "./PrimitiveType.h"

namespace Il {
namespace Ir {

// ctor
ConstructedClass::ConstructedClass(
    const GenericClass& generic_class,
    const TypeArgs& type_args)
    : WithCastable1_(generic_class),
      generic_class_(generic_class),
      type_args_(type_args) {}

// [C]
Type& ConstructedClass::ComputeBoundType() const {
  TypeArgs tyargs;
  foreach (TypeArgs::Enum, en, type_args_) {
    tyargs.Add(en.GetTypeParam(), en.GetTypeArg().ComputeBoundType());
  }
  return generic_class().Construct(tyargs);
}

Type& ConstructedClass::Construct(const TypeArgs& tyargs) const {
  TypeArgs cctyargs;
  foreach (EnumTypeArg, tyarg, *this) {
    cctyargs.Add(
        tyarg.GetTypeParam(),
        tyarg.GetTypeArg().Construct(tyargs));
  }
  return generic_class().Construct(cctyargs);
}

Method& ConstructedClass::ConstructMethod(
    MethodGroup& mtg,
    const Method& templ_mt) const {
  ASSERT(mtg.owner_class() == *this);
  ASSERT(!templ_mt.Is<GenericMethod>());

  auto& method = *new Method(
    mtg,
    templ_mt.modifiers(),
    *templ_mt.function_type().Construct(type_args_)
        .StaticCast<FunctionType>());

  if (templ_mt.GetFunction()) {
    auto& module = templ_mt.module().Clone(type_args_);
    foreach (Module::EnumFunction, fns, module) {
      auto& fn = *fns.Get();
      if (fn.GetMethod() == &templ_mt) {
        fn.SetMethod(method);
        method.SetFunction(fn);
      }
    }
  }

  return method;
}

// [G]
const Type& ConstructedClass::GetTypeArg() const {
  ASSERT(type_args_.Count() == 1);
  TypeArgs::Enum enum_args(type_args_);
  return *enum_args.Get().GetValue();
}

const Type* ConstructedClass::GetTypeArgOf(const TypeParam& type_param) const {
  return type_args_.Get(type_param);
}

// [I]
bool ConstructedClass::IsBound() const {
  foreach (TypeArgs::Enum, en, type_args_) {
    if (!en.Get().GetValue()->IsBound()) {
      return false;
    }
  }
  return true;
}

bool ConstructedClass::IsOpen() const {
  return type_args_.Count() < generic_class_.GetTypeParams().Count();
}

// [T]
String ConstructedClass::ToString() const {
  StringBuilder builder;
  builder.Append(generic_class().Class::ToString());
  builder.Append('<');
  const char* comma = "";
  foreach (EnumTypeArg, type_args, *this) {
    auto& type_arg = *type_args.Get().GetValue();
    builder.Append(comma);
    comma = ", ";
    builder.Append(type_arg.ToString());
  }
  builder.Append('>');
  return builder.ToString();
}

} // Ir
} // Il
