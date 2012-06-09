#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - AbstractResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractResolvePass.h"

#include "../CompileSession.h"
#include "../Ir.h"

#include "../../Il/Ir/Instructions.h"
#include "../../Il/Ir/Operands/Namespace.h"
#include "../../Il/Ir/Operands/ValuesTypeBuilder.h"

#include "../../Il/Tasks/NormalizeTasklet.h"

namespace Compiler {

using Common::Collections::HashSet_;

//using Il::Ir::SelectI;
//using Il::Ir::Values;
using Il::Ir::ValuesType;

using namespace Il::Tasks;

AbstractResolvePass::ResolveContext::ResolveContext(
    LookupMode lookup_mode,
    const NamespaceBody& namespace_body)
    : enclosing_class_(nullptr),
      lookup_mode_(lookup_mode),
      namespace_body_(namespace_body),
      resolve_mode_(DoNotAllowUnresolved) {}

AbstractResolvePass::ResolveContext::ResolveContext(
    const NamespaceBody& namespace_body,
    const Class& enclosing_class)
    : enclosing_class_(&enclosing_class),
      lookup_mode_(IncludeImmediateNamespaceBody),
      namespace_body_(namespace_body),
      resolve_mode_(DoNotAllowUnresolved) {}

AbstractResolvePass::ResolveContext::ResolveContext(
    const NamespaceBody& namespace_body,
    const Class* enclosing_class,
    ResolveMode resolve_mode)
    : enclosing_class_(enclosing_class),
      lookup_mode_(IncludeImmediateNamespaceBody),
      namespace_body_(namespace_body),
      resolve_mode_(resolve_mode) {}

void AbstractResolvePass::ResolveContext::Resolve(
    const Name& name,
    ArrayList_<const Operand*>& presents) const {
  if (!enclosing_class_) {
    return;
  }

  HashSet_<const Class*> visited_set;

  foreach (Class::EnumOuterAndSelf, outers, *enclosing_class_) {
    auto const outer = outers.Get().DynamicCast<Class>();
    if (!outer) {
      continue;
    }

    if (visited_set.Contains(outer)) {
      continue;
    }
    visited_set.Add(outer);

    if (auto const present = outer->Find(name)) {
      presents.Add(present);
      continue;
    }

    foreach (Class::EnumClassTree, classes, *outer) {
      auto& clazz = classes.Get();
      if (visited_set.Contains(&clazz)) {
        continue;
      }
      visited_set.Add(&clazz);
      if (auto const present = clazz.Find(name)) {
        presents.Add(present);
        continue;
      }
    }
  }
}

// ctor
AbstractResolvePass::AbstractResolvePass(
    const char16* const name,
    CompileSession* const session)
    : CompilePass(name, session) {}

// [E]
const Class* AbstractResolvePass::ExpectClass(
    const ResolveContext& context,
    const NameRef::Item& name_ref_item,
    const Operand& operand) {
  if (auto const clazz = operand.DynamicCast<GenericClass>()) {
    if (auto const composite =
            name_ref_item.DynamicCast<NameRef::CompositeName>()) {
      TypeArgs tyargs;
      NameRef::CompositeName::Enum names(*composite);
      foreach (GenericClass::EnumTypeParam, typarams, *clazz) {
        if (names.AtEnd()) {
          compile_session().AddError(
            name_ref_item.source_info(),
            CompileError_Resolve_Class_BadConstructedClass,
            name_ref_item);
            return clazz;
        }

        tyargs.Add(
            *typarams.Get(),
            Resolve(context, *new UnresolvedType(*names)));

        names.Next();
      }
      return clazz->Construct(tyargs).StaticCast<Class>();
    }
    return clazz;
  }

  if (auto const clazz = operand.DynamicCast<Class>()) {
    if (name_ref_item.Is<NameRef::CompositeName>()) {
      compile_session().AddError(
          name_ref_item.source_info(),
          CompileError_Resolve_Name_IsClass,
          name_ref_item);
    }
    return clazz;
  }

  compile_session().AddError(
    name_ref_item.source_info(),
    CompileError_Resolve_ExpectClass,
    name_ref_item);
  return nullptr;
}

// [E]
Namespace* AbstractResolvePass::ExpectNamespace(
    const NameRef::Item& name_ref_item,
    const Operand& operand) {
  if (auto const ns = operand.DynamicCast<Namespace>()) {
    if (name_ref_item.Is<NameRef::CompositeName>()) {
      compile_session().AddError(
          name_ref_item.source_info(),
          CompileError_Resolve_Name_IsNamespace,
          name_ref_item);
    }
    return ns;
  }

  compile_session().AddError(
      name_ref_item.source_info(),
      CompileError_Resolve_ExpectNamespace,
      name_ref_item);
  return nullptr;
}

const Operand* AbstractResolvePass::ExpectNamespaceOrType(
    const ResolveContext& context,
    const NameRef::Item& name_ref_item,
    const Operand& operand) {
  if (auto const ns = operand.DynamicCast<Namespace>()) {
    return ns;
  }

  if (auto const type = operand.DynamicCast<Type>()) {
    return ExpectType(context, name_ref_item, operand);
  }

  compile_session().AddError(
    name_ref_item.source_info(),
    CompileError_Resolve_ExpectNamespaceOrType,
    name_ref_item);
  return nullptr;
}

const Type* AbstractResolvePass::ExpectType(
    const NameRef::Item& name_ref_item,
    const Operand& operand) {
  if (auto const type = operand.DynamicCast<Type>()) {
    return type;
  }

  compile_session().AddError(
    name_ref_item.source_info(),
    CompileError_Resolve_ExpectType,
    name_ref_item);
  return nullptr;
}

const Type* AbstractResolvePass::ExpectType(
    const ResolveContext& context,
    const NameRef::Item& name_ref_item,
    const Operand& operand) {
  if (operand.Is<Class>()) {
    return ExpectClass(context, name_ref_item, operand);
  }

  if (auto const type = operand.DynamicCast<Type>()) {
    if (name_ref_item.Is<NameRef::CompositeName>()) {
      compile_session().AddError(
          name_ref_item.source_info(),
          CompileError_Resolve_Name_IsType,
          name_ref_item);
    }
    return type;
  }

  compile_session().AddError(
    name_ref_item.source_info(),
    CompileError_Resolve_ExpectType,
    name_ref_item);
  return nullptr;
}

// [F]
AbstractResolvePass::FindResult
AbstractResolvePass::FindMostOuter(
    const ResolveContext& context,
    const Name& name,
    const SourceInfo& source_info) {

  foreach (
      NamespaceBody::EnumOuterAndSelf, 
      outer_nsbs, 
      context.namespace_body()) {
    auto& outer_nsb = outer_nsbs.Get();

    auto& ns_def = outer_nsb.namespace_def();
    if (ns_def.name() == name) {
      return FindResult(ns_def.namespaze());
    }

    ArrayList_<Operand*> presents;

    if (context.ShouldInclude(outer_nsb)) {
      if (auto const present = outer_nsb.Find(name)) {
        if (auto const alias_def = present->DynamicCast<AliasDef>()) {
          if (!alias_def->IsRealized()) {
            return FindResult(false);
          }

          if (auto const another = ns_def.FindObject(name)) {
            compile_session().AddError(
              source_info,
              CompileError_Resolve_Alias_Conflict,
              name);
            return FindResult();
          }

          return FindResult(alias_def->operand());
        }

        return present->IsRealized()
            ? FindResult(present->operand())
            : FindResult();
      }

      // Using namespace
      foreach (NamespaceBody::EnumUsingNamespace, uzings, outer_nsb) {
        auto const uzing = uzings.Get();
        if (auto const present = uzing->namezpace()->Find(name)) {
          // Using namespace directive doesn't import members in inner
          // namespaces.
          if (!present->Is<Namespace>()) {
            presents.Add(present);
          }
        }
      }
    }

    // Find name defined in other namespace bodies.
    if (auto const present = ns_def.FindObject(name)) {
      presents.Add(present);
    }

    switch (presents.Count()) {
      case 0:
        break;

      case 1:
        return FindResult(*presents.Get(0));

      default:
        compile_session().AddError(
          source_info,
          CompileError_Resolve_Name_Ambiguous,
          name);
        return FindResult();
    }
  }

  return FindResult();
}

// [L]
const Operand* AbstractResolvePass::LookupNamespaceOrType(
    const ResolveContext& context,
    const NameRef& name_ref) {

  NameRef::Enum names(name_ref);
  auto& most_outer = *names;
  names.Next();

  auto result = FindMostOuter(
      context,
      most_outer.name(),
      most_outer.source_info());
  if (!result.operand_) {
    DEBUG_FORMAT("Not found: %s", name_ref);
    return nullptr;
  }

  auto const root = ExpectNamespaceOrType(
      context, 
      most_outer, 
      *result.operand_);
  if (!root) {
    DEBUG_FORMAT("Not decl space: %s", most_outer);
    return nullptr;
  }

  auto runner = static_cast<const ClassOrNamespace*>(root);
  while (!names.AtEnd()) {
    auto& item = *names;
    auto const present = runner->Find(item.name());
    if (!present) {
      DEBUG_FORMAT("%s isn't in %s", item, runner);
      return nullptr;
    }

    if (auto const ns = present->DynamicCast<Namespace>()) {
      runner = ns;

    } else if (auto const clazz = present->DynamicCast<Class>()) {
      runner = ExpectClass(context, item, *clazz);

    } else if (auto const type = present->DynamicCast<Type>()) {
      if (!type->IsBound()) {
        names.Next();
        return names.AtEnd() ? present : nullptr;
      }

      ExpectNamespaceOrType(context, item, *present);
      return nullptr;

    } else {
      ExpectNamespaceOrType(context, item, *present);
      return nullptr;
    }
    names.Next();
  }

  ASSERT(runner != nullptr);
  return runner;
}

// [R]
const Type& AbstractResolvePass::Resolve(
    const ResolveContext& context,
    const Type& ty) {
  if (auto const pArrayType = ty.DynamicCast<ArrayType>()) {
    return ArrayType::Intern(
        Resolve(context, pArrayType->element_type()),
        pArrayType->rank());
  }

  if (auto const pCons = ty.DynamicCast<ConstructedClass>()) {
    auto& gen_class = pCons->generic_class();
    TypeArgs oTypeArgs;
    foreach (GenericClass::EnumTypeParam, oEnum, gen_class) {
        auto& tyParam = *oEnum.Get();
        auto const tyArg = pCons->GetTypeArgOf(tyParam);
        oTypeArgs.Add(
            tyParam,
            Resolve(context, tyArg ? *tyArg : tyParam));
    }
    return gen_class.Construct(oTypeArgs);
  }

  if (auto const pFunType = ty.DynamicCast<FunctionType>()) {
    return FunctionType::Intern(
        Resolve(context, pFunType->return_type()),
        *Resolve(context, pFunType->params_type()).StaticCast<ValuesType>());
  }

  if (auto const pPointerType = ty.DynamicCast<PointerType>()) {
    return PointerType::Intern(
        Resolve(context, pPointerType->pointee_type()));
  }

  if (auto const tyVar = ty.DynamicCast<TypeVar>()) {
    return tyVar->IsBound() ? tyVar->ComputeBoundType() : ty;
  }

  if (auto const uncons = ty.DynamicCast<UnresolvedConstructedType>()) {
    auto& clazz = ResolveUnresolvedType(context, *uncons);
    DEBUG_FORMAT("%s %s", clazz.GetKind(), clazz);
    auto const gen = clazz.DynamicCast<GenericClass>();
    if (!gen) {
      compile_session().AddError(
          uncons->source_info(),
          CompileError_Resolve_Class_NotGenericClass,
          uncons->name_ref());
      return ty;
    }

    Type::Collection::Enum tys(uncons->types());
    TypeArgs tyargs;
    foreach (GenericClass::EnumTypeParam, typarams, *gen) {
      if (tys.AtEnd()) {
        compile_session().AddError(
            uncons->source_info(),
            CompileError_Resolve_Class_BadConstructedClass,
            uncons->name_ref());
        return ty;
      }

      auto& typaram = *typarams.Get();
      tyargs.Add(
          typaram,
          Resolve(context, *tys.Get()));
      tys.Next();
    }

    return gen->Construct(tyargs);
  }

  if (auto const pUnresolved = ty.DynamicCast<UnresolvedType>()) {
    return ResolveUnresolvedType(context, *pUnresolved);
  }

  if (auto const pValuesType = ty.DynamicCast<ValuesType>()) {
    ValuesTypeBuilder builder(pValuesType->Count());

    foreach (ValuesType::Enum, oEnum, pValuesType) {
        builder.Append(Resolve(context, *oEnum));
    }

    return builder.GetValuesType();
  }

  return ty;
}

// Note: Resolve method must not reutrn null.
const Type& AbstractResolvePass::ResolveUnresolvedType(
    const ResolveContext& context,
    const UnresolvedType& unresolved) {
  if (unresolved.IsBound()) {
    return unresolved.resolved_type();
  }

  auto& name_ref = unresolved.name_ref();

  NameRef::Enum inners(name_ref);

  auto& most_outer = *inners;
  auto& most_outer_name = most_outer.name();

  inners.Next();

  ArrayList_<const Operand*> presents;
  context.Resolve(most_outer_name, presents);

  switch (presents.Count()) {
    case 0: {
      // Lookup in enclosing namespaces.
      auto const operand = LookupNamespaceOrType(
          context,
          name_ref);
      if (operand) {
        if (auto const type = ExpectType(most_outer, *operand)) {
          const_cast<UnresolvedType&>(unresolved).Bind(*type);
          return *type;
        }
      }

      if (!context.IsAllowUnresolved()) {
        compile_session().AddError(
            most_outer.source_info(),
            CompileError_Resolve_Class_NotFound,
            most_outer.name());
      }
      return unresolved;
    }

    case 1:
      if (inners.AtEnd()) {
        if (auto const type = 
                ExpectType(context, most_outer, *presents.Get(0))) {
          const_cast<UnresolvedType&>(unresolved).Bind(*type);
          return *type;
        }
        return unresolved;
      }

      if (auto runner = 
                ExpectClass(context, most_outer, *presents.Get(0))) {
        auto const inner = inners.Get();
        auto const present = runner->Find(inner->name());
        if (!present) {
          if (!context.IsAllowUnresolved()) {
            compile_session().AddError(
                inner->source_info(),
                CompileError_Resolve_Class_NotFound,
                const_cast<Name*>(&inner->name()));
          }
          return unresolved;
        }

        inners.Next();
        if (inners.AtEnd()) {
          if (auto const type = ExpectType(context, *inner, *present)) {
            const_cast<UnresolvedType&>(unresolved).Bind(*type);
            return *type;
          }
        }

        runner = ExpectClass(context, *inner, *present);
        if (!runner) {
          return unresolved;
        }
      }

    default:
      compile_session().AddError(
        name_ref.source_info(),
        CompileError_Resolve_Name_Ambiguous,
        presents[0],
        presents[1]);
      return unresolved;
  }
}

// [S]
void AbstractResolvePass::Start() {
  DEBUG_PRINTF("Start %ls ~~~~~~~~~~~~~~~~~~~~\n", name());
  compile_session().Apply(this);
  DEBUG_PRINTF("End %ls ~~~~~~~~~~~~~~~~~~~~\n", name());
}

} // Compiler
