#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Fasl - Fast Dumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeSorter.h"

#include "../Ir/Operands.h"

namespace Il {
namespace Fasl {

// ctor
TypeSorter::TypeSorter(const Type::Set& type_set) {
  pending_set_.AddAll(type_set);
}

// [I]
bool TypeSorter::IsReady(const Class& clazz) const {
  if (pending_set_.Contains(&clazz)) {
    return false;
  }

  if (auto const outer = clazz.outer().DynamicCast<Class>()) {
    if (!IsReady(*outer)) {
      return false;
    }
  }

  if (auto const cons = clazz.DynamicCast<ConstructedClass>()) {
    if (!IsReady(cons->generic_class())) {
      return false;
    }
  }

  foreach (Class::EnumBaseSpec, bases, clazz) {
    if (!IsReady(*bases)) {
      return false;
    }
  }

  return true;
}

bool TypeSorter::IsReady(const Type& type) const {
  if (pending_set_.Contains(&type)) {
    return false;
  }

  if (auto const p = type.DynamicCast<ArrayType>()) {
    return IsReady(p->element_type());
  }

  if (auto const p = type.DynamicCast<FunctionType>()) {
    return IsReady(p->return_type()) && IsReady(p->params_type());
  }

  if (auto const p = type.DynamicCast<Class>()) {
    return IsReady(*p);
  }

  if (auto const p = type.DynamicCast<PointerType>()) {
    return IsReady(p->pointee_type());
  }

  if (auto const p = type.DynamicCast<ValuesType>()) {
    foreach (ValuesType::Enum, types, p) {
      if (!IsReady(*types)) {
        return false;
      }
    }
    return true;
  }

  return true;
}

// [M]
void TypeSorter::Map(Type::Queue& types, Type::Queue& pendings) {
  while (!types.IsEmpty()) {
    auto const type = types.Take();
    pending_set_.Remove(type);
    if (IsReady(*type)) {
      result_list_.Add(type);
    } else {
      pendings.Give(type);
      pending_set_.Add(type);
    }
  }
}

// [S]
Type::Array* TypeSorter::Sort() {
  Type::Queue pending_queue;
  {
    Type::Queue ready_queue;
    foreach (Type::Set::Enum, types, pending_set_) {
      ready_queue.Give(*types);
    }
    Map(ready_queue, pending_queue);
  }

  while (!pending_set_.IsEmpty()) {
    Type::Queue not_ready_queue;
    Map(pending_queue, not_ready_queue);
    while (!not_ready_queue.IsEmpty()) {
      pending_queue.Give(not_ready_queue.Take());
    }
  }

  ASSERT(pending_set_.IsEmpty());
  return result_list_.ToArray();
}

Type::Array* LibExport TypeSorter::Sort(const Type::Set& type_set) {
  TypeSorter sorter(type_set);
  return sorter.Sort();
}

} // Fasl
} // Il
