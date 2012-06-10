#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - ResolveClassPass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ResolveClassPass.h"

#include "../CompileSession.h"
#include "../Ir.h"

namespace Compiler {

namespace {

class CycleChecker {
  private: const Class& start_class_;
  private: Type::Set visited_set_;

  public: CycleChecker(const Class& start_class)
      : start_class_(start_class) {}

  public: bool HasCycle() {
    return VisitBaseSpecs(start_class_);
  }

  private: bool Visit(const Class& clazz) {
    if (visited_set_.Contains(&clazz)) {
      return false;
    }

    visited_set_.Add(&clazz);

    if (start_class_ == clazz) {
      return true;
    }

    return VisitBaseSpecs(clazz);
  }

  private: bool VisitBaseSpecs(const Class& clazz) {
    foreach (Class::EnumBaseSpec, it, clazz) {
      if (Visit(*it)) {
        return true;
      }
    }

    foreach (Class::EnumOuterAndSelf, it, clazz) {
      if (it.Get() == clazz) {
        continue;
      }

      // Check outer classes don't depend start class.
      if (auto const outer_class = it.Get().DynamicCast<Class>()) {
        if (VisitBaseSpecs(*outer_class)) {
          return true;
        }
      }
    }
    return false;
  }

  DISALLOW_COPY_AND_ASSIGN(CycleChecker);
};
} // namespace

// ctor
ResolveClassPass::ResolveClassPass(CompileSession* const pCompileSession)
    : Base(L"ResolveClassPass", pCompileSession) {
}

// [A]
void ResolveClassPass::AnalyzeError() {
  foreach (PendingSet::Enum, pendings, &pendings_) {
    auto const name_def = static_cast<NameDef*>(pendings.Get());
    if (auto const alias_def = name_def->DynamicCast<AliasDef>()) {
      compile_session().AddError(
        alias_def->source_info(),
        CompileError_Resolve_Alias_NotFound,
        alias_def->real_name());

    } else if (auto const class_def = name_def->DynamicCast<ClassDef>()) {
      foreach (ClassDef::EnumBaseSpec, specs, *class_def) {
        auto& unresolved = *specs.Get()->StaticCast<UnresolvedType>();
        compile_session().AddError(
            unresolved.source_info(),
            CompileError_Resolve_Class_NotFound,
            unresolved.name_ref());
     }
    } else {
      CAN_NOT_HAPPEN();
    }
  }
}

// [F]
bool ResolveClassPass::FixAliasDef(AliasDef& alias_def) {
  ASSERT(!IsPending(alias_def));

  if (alias_def.IsRealized()) {
    ASSERT(alias_def.operand().Is<Namespace>());
    return false;
  }

  auto& real_name = *alias_def.real_name();

  ResolveContext context(
      ExcludeImmediateNamespaceBody,
      *alias_def.namespace_body());

  auto const thing = LookupNamespaceOrType(context, real_name);
  if (!thing) {
    Postpone(alias_def);
    return false;
  }

  DEBUG_FORMAT("Realize alias %s=%s (%s)",
      alias_def.name(),
      real_name,
      thing);

  alias_def.Realize(*thing->StaticCast<Type>());
  return true;
}

bool ResolveClassPass::FixClassDef(ClassDef& class_def) {
  ASSERT(!IsPending(class_def));

  DEBUG_FORMAT("process class %s", class_def);

  ArrayList_<const Class*> base_spec_list;

  ResolveContext context(
    class_def.namespace_body(),
    class_def.owner().operand().DynamicCast<Class>(),
    AllowUnresolved);

  foreach (ClassDef::EnumBaseSpec, bases, class_def) {
    auto& base_spec = *bases;
    auto& type = Resolve(context, base_spec);

    if (!type.IsBound()) {
      DEBUG_FORMAT("Postpone class %s by class %s", class_def, type);
      Postpone(class_def);
      return false;
    }

    auto const unty = base_spec.DynamicCast<UnresolvedType>();

    SourceInfo source_info(unty
        ? unty->source_info()
        : class_def.source_info());

    if (type == class_def.GetClass()) {
      compile_session().AddError(
          class_def.source_info(),
          CompileError_Resolve_Class_CyclicBase,
          class_def);
    }

    if (auto const clazz = type.DynamicCast<Class>()) {
      if (auto const gen = clazz->DynamicCast<GenericClass>()) {
          compile_session().AddError(
              source_info,
              CompileError_Resolve_ExpectNonGenericClass,
              gen->name());

      } else if (clazz->IsInterface()) {
        if (!class_def.IsInterface() && base_spec_list.IsEmpty()) {
          base_spec_list.Add(Ty_Object);
        }
        base_spec_list.Add(clazz);

      } else if (class_def.IsInterface()) {
        compile_session().AddError(
            source_info,
            CompileError_Resolve_ExpectInterface,
            class_def,
            base_spec);

      } else if (base_spec_list.IsEmpty()) {
        base_spec_list.Add(clazz);

      } else {
        compile_session().AddError(
            source_info,
            CompileError_Resolve_Class_MoreThanOneBase,
            class_def,
            base_spec);
      }

    } else {
      compile_session().AddError(
          source_info,
          CompileError_Resolve_ExpectClass,
          class_def,
          base_spec);
    }
  }

  if (compile_session().HasError()) {
   return true;
  }

  foreach (ClassDef::EnumTypeParam, it, class_def) {
    // TODO(yosi) 2012-06-10 NYI: Realize type param from TypeParamDef.
    const_cast<TypeParam&>(it->type_param()).RealizeTypeParam(
        CollectionV_<const Class*>(),
        it->is_newable() ? TypeParam::Newable : TypeParam::NotNewable);
  }

  DEBUG_FORMAT("Realize %s %s : %s",
      class_def.IsInterface() ? "interface" : "class",
      class_def, base_spec_list);

  auto& clazz = class_def.GetClass();
  if (clazz.outer().Find(clazz.name())) {
    compile_session().AddError(
        class_def.source_info(),
        CompileError_Resolve_Unexpected,
        String::Format("Class %s is already defined.", class_def));
    return true;
  }

  clazz.RealizeClass(base_spec_list);
  return true;
}

bool ResolveClassPass::FixNameDef(NameDef& name_def) {
  if (auto const alias_def = name_def.DynamicCast<AliasDef>()) {
    return FixAliasDef(*alias_def);
  }

  if (auto const class_def = name_def.DynamicCast<ClassDef>()) {
    return FixClassDef(*class_def);
  }

  CAN_NOT_HAPPEN();
}

// [I]
bool ResolveClassPass::IsPending(NameDef& name_def) const {
  return pendings_.Contains(&name_def);
}

// [P]
void ResolveClassPass::Postpone(AliasDef& alias_def) {
  DEBUG_FORMAT("Postpone %s", alias_def);
  pendings_.Add(&alias_def);
}

void ResolveClassPass::Postpone(ClassDef& class_def) {
  DEBUG_FORMAT("Postpone %s", class_def);
  pendings_.Add(&class_def);
}

void ResolveClassPass::Process(AliasDef* const alias_def) {
  ASSERT(alias_def != nullptr);
  FixAliasDef(*alias_def);
}

void ResolveClassPass::Process(ClassDef* const class_def) {
  ASSERT(class_def != nullptr);
  class_defs_.Add(class_def);
  Postpone(*class_def);
  Base::Process(class_def);
}

// [S]
void ResolveClassPass::Start() {
  compile_session().Apply(this);

  for(;;) {
    DEBUG_PRINTF("Resolve loop ~~~~~~~~~~~~~~~~~~~~\n");
    Queue_<NameDef*> ready_queue;

    foreach (PendingSet::Enum, pendings, &pendings_) {
      auto const name_def = pendings.Get();
      ready_queue.Give(name_def);
    }

    pendings_.Clear();

    bool more = false;
    while (!ready_queue.IsEmpty()) {
      auto& name_def = *ready_queue.Take();
      if (FixNameDef(name_def)) {
          more = true;
      }
    }

    if (compile_session().HasError()) {
      break;
    }

    if (!more) {
      AnalyzeError();
      break;
    }
  }

  foreach (ArrayList_<const ClassDef*>::Enum, it, class_defs_) {
    auto& class_def = *it.Get();
    CycleChecker checker(class_def.GetClass());
    if (checker.HasCycle()) {
      compile_session().AddError(
          class_def.source_info(),
          CompileError_Resolve_Class_CyclicBase,
          class_def);
    }
  }
}

} // Compiler
