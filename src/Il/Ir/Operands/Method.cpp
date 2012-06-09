#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Method
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Method.h"

#include "./Class.h"
#include "./ConstructedMethod.h"
#include "./Function.h"
#include "./MethodGroup.h"
#include "./ValuesType.h"
#include "../Name.h"

namespace Il {
namespace Ir {

// ctor
Method::Method(
  const MethodGroup& method_group,
  int const iModifiers,
  const FunctionType& function_type)
  : WithModifiers(iModifiers),
    m_pFunction(nullptr),
    function_type_(function_type),
    method_group_(method_group),
    m_pParamNames(new ParamNames(function_type.params_type().Count())) {}

Module& Method::module() const {
  return GetFunction()->module();
}

const Name& Method::name() const {
  return GetMethodGroup()->name();
}

const ValuesType& Method::params_type() const {
  return function_type_.params_type();
}

const Type& Method::return_type() const {
  return function_type_.return_type();
}

// [C]
const ArrayType* Method::ComputeRestType() const {
  return function_type_.ComputeRestType();
}

int Method::CountParams() const {
  return function_type_.CountParams();
}

// [G]
Namespace& Method::GetNamespace() const {
  return GetMethodGroup()->GetNamespace();
}

const Class* Method::GetOwnerClass() const {
  return &GetMethodGroup()->owner_class();
}

/// <summary>
///   We assume this method and specified method are applicable for same
//    argument list.
/// </summary>
bool Method::IsMoreSpecific(const Method* const that) const {
  ASSERT(nullptr != that);
  ASSERT(that->GetMethodGroup() == GetMethodGroup());
  ASSERT(this != that);

  auto const iDiff = CountParams() - that->CountParams();
  if (iDiff > 0) {
      // this(A, B, C) > that(A, B[]) : Aboid array conversion
      return true;
  }

  if (iDiff < 0) {
      // this(A, B[]) < that(A, B, C) : Requires array conversion
      return false;
  }

  // Check rest type
  {
      auto const pThatRestType = that->ComputeRestType();
      auto const pThisRestType = ComputeRestType();

      if (nullptr == pThisRestType && nullptr == pThatRestType) {
          // Both this and that have no rest parameters.

      } else if (nullptr == pThisRestType) {
          // this(A, B, C) > that(A, B, C[])
          return true;

      } else {
          // this(A, B, C[]) < that(A, B, C)
          return false;
      }
  }

  EnumParamType oEnumThat(that);

  foreach (EnumParamType, oEnumThis, this) {
      auto& thatty = oEnumThat.Get();
      auto& thisty = oEnumThis.Get();

      if (thisty == thatty) {
          continue;
      }

      switch (thisty.IsSubtypeOf(thatty)) {
      case Subtype_No:
          return false;

      case Subtype_Yes:
          return true;

      case Subtype_Unknown:
          break;

      default:
          CAN_NOT_HAPPEN();
      }

      oEnumThat.Next();
  }

  return true;
}

// [S]
void Method::SetFunction(Function& fun) {
  ASSERT(m_pFunction == nullptr);
  m_pFunction = &fun;
}

void Method::SetParamName(int const index, const Name& name) {
  m_pParamNames->Set(index, &name);
}

// [T]
String Method::ToString() const {
  return String::Format("%s %s.%s%s",
    function_type_.return_type(),
    owner_class(),
    name(),
    function_type_.params_type());
}

} // Ir
} // Il
