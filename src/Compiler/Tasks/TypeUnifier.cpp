#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - TypeUnifier
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeUnifier.h"

#include "../CompileSession.h"
#include "../Ir.h"

namespace Compiler {

using Il::Tasks::Tasklet;

namespace {
static const Type& TypeAnd(const Type& a, const Type& b) {
  if (a == b) {
    return a;
  }

  if (a == *Ty_Void || b == *Ty_Void) {
    return *Ty_Void;
  }

  if (a.IsSubtypeOf(b) == Subtype_Yes) {
    return a;
  }

  if (b.IsSubtypeOf(a) == Subtype_Yes) {
    return b;
  }

  return *Ty_Void;
}

static const Type* TypeAnd(const Type* const a, const Type* const b) {
  if (a == b) {
    return a;
  }

  return a && b
    ? &TypeAnd(*a, *b)
    : a ? a : b;
}
} // namespace

void TypeUnifier::Bindings::Bind(const Type& lhs, const Type& rhs) {
  ASSERT(lhs.Is<TypeParam>() || lhs.Is<TypeVar>());
  ASSERT(lhs != rhs);
  if (auto const typaram = lhs.DynamicCast<TypeParam>()) {
    typaram_set_.Add(typaram);

  } else if (auto const tyvar = lhs.DynamicCast<TypeVar>()) {
    tyvar_set_.Add(tyvar);
  }
  binding_stack_.Push(Binding(lhs, rhs));
}

bool TypeUnifier::Bindings::Equiv(const Type& lhs, const Type& rhs) {
  ASSERT(lhs.Is<TypeParam>() || lhs.Is<TypeVar>());
  ASSERT(rhs.Is<TypeParam>() || rhs.Is<TypeVar>());

  class Local {
    public: static bool Lookup(
        const Stack_<Binding>& binding_stack,
        HashSet_<const Type*> seen_set,
        const Type& lhs,
        const Type& rhs) {
      if (seen_set.Contains(&lhs)) {
        return false;
      }
      seen_set.Add(&lhs);
      ArrayList_<const Type*> equivs;
      foreach (Stack_<Binding>::Enum, bindings, binding_stack) {
        auto const& binding = bindings.Get();
        if (binding.lhs() == lhs) {
          auto& ty = binding.rhs();
          if (ty == rhs) {
            return true;
          }
          if (ty.Is<TypeParam>() || ty.Is<TypeVar>()) {
            if (!seen_set.Contains(&ty)) {
              equivs.Add(&ty);
            }
          }
        }
      }

      foreach (ArrayList_<const Type*>::Enum, en, equivs) {
        if (Lookup(binding_stack, seen_set, *en.Get(), rhs)) {
          return true;
        }
      }
      return false;
    }
  };
  ASSERT(lhs.Is<TypeParam>() || lhs.Is<TypeVar>());
  ASSERT(rhs.Is<TypeParam>() || rhs.Is<TypeVar>());
  HashSet_<const Type*> seen_set;
  return Local::Lookup(binding_stack_, seen_set, lhs, rhs);
}

// Returns invariant type bound to tykey.
const Type* TypeUnifier::Bindings::Eval(const Type& tykey) const {
  class Local {
    public: static const Type* Lookup(
        const Stack_<Binding>& binding_stack,
        HashSet_<const Type*> seen_set,
        const Type& tykey) {
      if (seen_set.Contains(&tykey)) {
        return nullptr;
      }
      seen_set.Add(&tykey);
      ArrayList_<const Type*> equivs;
      foreach (Stack_<Binding>::Enum, bindings, binding_stack) {
        auto const& binding = bindings.Get();
        if (binding.lhs() == tykey) {
          auto& ty = binding.rhs();
          if (ty.Is<TypeParam>() || ty.Is<TypeVar>()) {
            if (!seen_set.Contains(&ty)) {
              equivs.Add(&ty);
            }
          } else {
            return &ty;
          }
        }
      }

      foreach (ArrayList_<const Type*>::Enum, en, equivs) {
        if (auto const ty = Lookup(binding_stack, seen_set, *en.Get())) {
          return ty;
        }
      }
      return nullptr;
    }
  };
  ASSERT(tykey.Is<TypeParam>() || tykey.Is<TypeVar>());
  HashSet_<const Type*> seen_set;
  return Local::Lookup(binding_stack_, seen_set, tykey);
}

void TypeUnifier::Bindings::Restore(int const marker) {
  ASSERT(binding_stack_.Count() >= marker);
  while (binding_stack_.Count() > marker) {
    binding_stack_.Pop();
  }
}

int TypeUnifier::Bindings::Save() const {
  return binding_stack_.Count();
}

TypeUnifier::EnumTypeParam::EnumTypeParam(const TypeUnifier& r)
    : Base(r.bindings_.typaram_set()),
      bindings_(r.bindings_) {}

const TypeParam& TypeUnifier::EnumTypeParam::Get() const {
  return *const_cast<TypeParam*>(Base::Get());
}

const Type& TypeUnifier::EnumTypeParam::GetBoundType() const {
  auto& typaram = Get();
  auto const ty = bindings_.Eval(typaram);
  return ty ? *ty : typaram;
}

TypeUnifier::EnumTypeVar::EnumTypeVar(const TypeUnifier& r)
    : Base(r.bindings_.tyvar_set()),
      bindings_(r.bindings_) {}

const TypeVar& TypeUnifier::EnumTypeVar::Get() const {
  return *Base::Get();
}

const Type& TypeUnifier::EnumTypeVar::GetBoundType() const {
  auto& tyvar = Get();
  auto const ty = bindings_.Eval(tyvar);
  return ty ? *ty : tyvar;
}

// ctor
TypeUnifier::TypeUnifier(
    CompileSession& session,
    const SourceInfo& source_info)
    : Tasklet("TypeUnifier", session), source_info_(source_info) {}

// [A]
bool TypeUnifier::AddEquiv(
    const Type& tykey1,
    const Type& tykey2) {
  ASSERT(tykey1.Is<TypeParam>() || tykey1.Is<TypeVar>());
  ASSERT(tykey2.Is<TypeParam>() || tykey2.Is<TypeVar>());

  if (tykey1 == tykey2) {
    return true;
  }

  auto const ty = TypeAnd(bindings_.Eval(tykey1), bindings_.Eval(tykey2));
  if (ty == Ty_Void) {
    return false;
  }

  if (ty) {
    return Bind(tykey1, *ty) && Bind(tykey2, *ty);
  }

  if (!bindings_.Equiv(tykey1, tykey2)) {
    bindings_.Bind(tykey1, tykey2);
    bindings_.Bind(tykey2, tykey1);
  }
  return true;
}

// [B]
bool TypeUnifier::Bind(const Type& tykey, const Type& newty) {
  DEBUG_FORMAT("%s=%s", tykey, newty);
  ASSERT(tykey.Is<TypeParam>() || tykey.Is<TypeVar>());

  if (tykey == newty) {
    return true;
  }

  if (auto const newtyparam = newty.DynamicCast<TypeParam>()) {
    return AddEquiv(tykey, *newtyparam);
  }

  if (auto const newtyvar = newty.DynamicCast<TypeVar>()) {
    return AddEquiv(tykey, *newtyvar);
  }

  if (auto const p = bindings_.Eval(tykey)) {
    auto& curty = *p;
    auto& ty = TypeAnd(curty, newty);
    if (ty == *Ty_Void) {
      DEBUG_FORMAT("%s=%s != %s", tykey, curty, newty);
      return false;
    }

    DEBUG_FORMAT("bind: %s=%s", tykey, ty);
    bindings_.Bind(tykey, ty);
    return true;
  }

  DEBUG_FORMAT("new: %s=%s", tykey, newty);
  bindings_.Bind(tykey, newty);
  return true;
}

// [U]
bool TypeUnifier::Unify(const Type& lhs, const Type& rhs) {
  return UnifyTypes(lhs, rhs);
}

bool TypeUnifier::UnifyClasses(
    const Class& lhs_class,
    const Class& rhs_class) {
  DEBUG_FORMAT("%s %s", lhs_class, rhs_class);

  if (lhs_class == rhs_class) {
    return true;
  }

  if (auto const lhs_cclass = lhs_class.DynamicCast<ConstructedClass>()) {
    if (auto const rhs_cclass = rhs_class.DynamicCast<ConstructedClass>()) {
      auto const marker = bindings_.Save();
      if (UnifyConstructedClasses(*lhs_cclass, *rhs_cclass)) {
        return true;
      }
      bindings_.Restore(marker);
    }

    if (auto const rhs_gclass = rhs_class.DynamicCast<GenericClass>()) {
      auto const marker = bindings_.Save();
      if (UnifyConstructedClassAndGenericClass(*lhs_cclass, *rhs_gclass)) {
        return true;
      }
      bindings_.Restore(marker);
    }
  } else if (auto const lhs_gclass = lhs_class.DynamicCast<GenericClass>()) {
    if (auto const rhs_cclass = rhs_class.DynamicCast<ConstructedClass>()) {
      auto const marker = bindings_.Save();
      if (UnifyGenericClassAndConstructedClass(*lhs_gclass, *rhs_cclass)) {
        return true;
      }
      bindings_.Restore(marker);
    }
  }

  // ConstructedClass >= Class
  // GenericClass >= GenericClass
  // GenericClass >= Class
  // Class >= Class
  foreach (Class::EnumClassTree, enum_rhs, rhs_class) {
    auto& rhs_base = enum_rhs.Get();
    if (rhs_base == rhs_class) {
      continue;
    }
    DEBUG_FORMAT("rhs %s of %s", rhs_base, rhs_class);
    auto const marker = bindings_.Save();
    if (UnifyClasses(lhs_class, rhs_base)) {
      return true;
    }
    bindings_.Restore(marker);
  }

  if (rhs_class.GetBaseClass() == Ty_Enum) {
    auto& enum_type_class = *rhs_class.Find(*QD_value)
        ->StaticCast<Field>()->storage_type().StaticCast<Class>();
    return UnifyClasses(lhs_class, enum_type_class);
  }

  return false;
}

bool TypeUnifier::UnifyConstructedClasses(
    const ConstructedClass& lhs_cclass,
    const ConstructedClass& rhs_cclass) {
  DEBUG_FORMAT("%s %s", lhs_cclass, rhs_cclass);
  if (lhs_cclass == rhs_cclass) {
    return true;
  }

  if (lhs_cclass.generic_class() != rhs_cclass.generic_class()) {
    return false;
  }

 auto const& lhs_type_args = lhs_cclass.type_args();
  auto const& rhs_type_args = rhs_cclass.type_args();

  foreach (TypeArgs::Enum, enum_lhs, lhs_type_args) {
    auto& typaram = *enum_lhs.Get().GetKey();
    auto const lhs_ty = enum_lhs.Get().GetValue();
    if (auto const rhs_ty = rhs_type_args.Get(typaram)) {
      if (UnifyTypes(*lhs_ty, *rhs_ty)) {
        return true;
      }
    }
  }

  foreach (TypeArgs::Enum, enum_rhs, rhs_type_args) {
    auto& typaram = *enum_rhs.Get().GetKey();
    if (auto const lhs_ty = rhs_type_args.Get(typaram)) {
      auto const rhs_ty = enum_rhs.Get().GetValue();
      if (UnifyTypes(*lhs_ty, *rhs_ty)) {
        return true;
      }
    }
  }

  return false;
}

bool TypeUnifier::UnifyConstructedClassAndGenericClass(
    const ConstructedClass& lhs_cclass,
    const GenericClass& rhs_gclass) {
  DEBUG_FORMAT("%s %s", lhs_cclass, rhs_gclass);
  if (lhs_cclass.generic_class() != rhs_gclass) {
    return false;
  }

  foreach (GenericClass::EnumTypeParam, typarams, rhs_gclass) {
    auto& typaram = *typarams.Get();
    if (auto const lhs_ty = lhs_cclass.GetTypeArgOf(typaram)) {
      if (!Bind(typaram,  *lhs_ty)) {
        return false;
      }
    }
 }

 return true;
}

bool TypeUnifier::UnifyGenericClassAndConstructedClass(
    const GenericClass& lhs_gclass, const ConstructedClass& rhs_cclass) {
  DEBUG_FORMAT("%s %s", lhs_gclass, rhs_cclass);
  if (lhs_gclass != rhs_cclass.generic_class()) {
    return false;
  }

  foreach (GenericClass::EnumTypeParam, typarams, lhs_gclass) {
    auto& typaram = *typarams.Get();
    if (auto const rhs_ty = rhs_cclass.GetTypeArgOf(typaram)) {
      if (!Bind(typaram,  *rhs_ty)) {
        return false;
      }
    }
 }

 return true;
}

// See also MethodResolver::BindTypeArg
bool TypeUnifier::UnifyTypes(
    const Type& lhs_ty,
    const Type& rhs_ty) {
  DEBUG_FORMAT("%s %s", lhs_ty, rhs_ty);

  if (lhs_ty == rhs_ty) {
    return true;
  }

  if (auto const lhs_arrty = lhs_ty.DynamicCast<ArrayType>()) {
    if (auto const rhs_arrty = rhs_ty.DynamicCast<ArrayType>()) {
      return UnifyTypes(
          lhs_arrty->element_type(),
          rhs_arrty->element_type());
    }

    if (auto const rhs_class = rhs_ty.DynamicCast<Class>()) {
      return UnifyClasses(
          Ty_Array->Construct(lhs_arrty->element_type()),
          *rhs_class);
    }

    return !rhs_ty.IsBound();
  }

  if (auto const lhs_class = lhs_ty.DynamicCast<Class>()) {
    if (auto const rhs_arrty = rhs_ty.DynamicCast<ArrayType>()) {
      // Class >= T[] == Class >= Array<T>
      return UnifyClasses(
        *lhs_class,
        Ty_Array->Construct(rhs_arrty->element_type()));
    }

    // TODO(yosi) 2012-02-12 NYI Integer type promotion for applicable
    // method.
    if (auto const rhs_class = rhs_ty.DynamicCast<Class>()) {
      // Class >= Class
      return UnifyClasses(*lhs_class, *rhs_class);
    }

    return !rhs_ty.IsBound();
  }

  if (auto const lhs_funty = lhs_ty.DynamicCast<FunctionType>()) {
    if (auto const rhs_funty = rhs_ty.DynamicCast<FunctionType>()) {
      return
          UnifyTypes(
              lhs_funty->return_type(),
              rhs_funty->return_type())
          && UnifyTypes(
              lhs_funty->params_type(),
              rhs_funty->params_type());
    }
    return !rhs_ty.IsBound();
  }

  if (auto const lhs_ptrty = lhs_ty.DynamicCast<PointerType>()) {
    if (auto const rhs_ptrty = rhs_ty.DynamicCast<PointerType>()) {
      return UnifyTypes(
          lhs_ptrty->pointee_type(),
          rhs_ptrty->pointee_type());
    }
    return !rhs_ty.IsBound();
  }

  if (auto const lhs_typaram = lhs_ty.DynamicCast<TypeParam>()) {
    return Bind(*lhs_typaram, rhs_ty);
  }

  if (auto const lhs_tyvar = lhs_ty.DynamicCast<TypeVar>()) {
    return Bind(*lhs_tyvar, rhs_ty);
  }

  if (auto const lhs_valsty = lhs_ty.DynamicCast<ValuesType>()) {
    if (auto const rhs_valsty = rhs_ty.DynamicCast<ValuesType>()) {
      ValuesType::Enum enum_rhs(rhs_valsty);
      foreach (ValuesType::Enum, enum_lhs, lhs_valsty) {
        auto& rhs_subty = enum_rhs.Get();
        auto& lhs_subty = enum_lhs.Get();
        if (!UnifyTypes(lhs_subty, rhs_subty)) {
          return false;
        }
        enum_rhs.Next();
      }
      return true;
    }

    // Note: Language syntax doesn't TypeParam/TypeVar to values type.
    return false;
  }

  session().AddErrorInfo(
    CompileErrorInfo(
        source_info_,
        CompileError_Resolve_Type_Unexpected,
        CollectionV_<Box>(lhs_ty)));
  return false;
}

} // Compiler
