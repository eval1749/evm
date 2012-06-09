#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- ConstructedMethod
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ConstructedMethod.h"

#include "./GenericClass.h"
#include "./MethodGroup.h"

namespace Il {
namespace Ir {

// ctor
ConstructedMethod::ConstructedMethod(
    const GenericMethod& generic_method,
    const TypeArgs& type_args)
    : Base(
          *generic_method.owner_class().Construct(type_args)
              .StaticCast<Class>()->Find(generic_method.name())
                  ->StaticCast<MethodGroup>(),
          generic_method.GetModifiers(),
          *generic_method.function_type().Construct(type_args)
              .StaticCast<FunctionType>()),
     generic_method_(generic_method),
     type_args_(type_args) {}

} // Ir
} // Il
