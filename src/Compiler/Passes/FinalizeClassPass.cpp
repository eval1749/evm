#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - FinalizeClassPass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FinalizeClassPass.h"

#include "../CompileSession.h"
#include "../Ir.h"

namespace Compiler {

// ctor
FinalizeClassPass::FinalizeClassPass(CompileSession* const session)
    : Base(L"FinalizeClassPass", session) {
}

void FinalizeClassPass::Process(ClassDef* const class_def) {
  ASSERT(class_def != nullptr);

  auto& clazz = class_def->GetClass();

  ResolveContext context(class_def->namespace_body(), clazz);

  DEBUG_FORMAT("Resolve fields of %s", clazz);

  for (auto& thing: class_def->members()) {
    if (auto const field_def = thing.DynamicCast<FieldDef>()) {
        auto& type = Resolve(context, *field_def->GetTy());
        auto& name = field_def->name();

        if (!type.IsBound()) {
          DEBUG_FORMAT("Can't resolve type for %s", name);
          compile_session().AddError(
              field_def->source_info(),
              CompileError_Resolve_Type_NotFound,
              type);
          continue;
        }

        if (auto const gen_clazz = type.DynamicCast<GenericClass>()) {
          compile_session().AddError(
              field_def->source_info(),
              CompileError_Resolve_ExpectNonGenericClass,
              gen_clazz->name());
          continue;
        }

        DEBUG_FORMAT("%s : %s", name, type);

        auto& field = *new Field(
            clazz,
            field_def->GetModifiers(),
            type,
            name,
            field_def->GetSourceInfo());

        field_def->Realize(field);
        clazz.Add(name, field);
    }
  }
}

} // Compiler
