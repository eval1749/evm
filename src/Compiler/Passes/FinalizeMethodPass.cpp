#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - FinalizeMethodPass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FinalizeMethodPass.h"

#include "../CompileSession.h"
#include "../Ir.h"

#include "../Tasks/MethodResolver.h"
#include "../Tasks/NameRefResolver.h"
#include "../Tasks/TypeVarResolver.h"
#include "../Tasks/TypeVarRewriter.h"

namespace Compiler {

// ctor
FinalizeMethodPass::FinalizeMethodPass(CompileSession* const pCompileSession)
    : Base(L"FinalizeMethodPass", pCompileSession) {}

// [F]
void FinalizeMethodPass::FixFunction(MethodDef& method_def, Function& fun) {
  auto& clazz = method_def.owner_class_def().GetClass();
  ResolveContext context(method_def.namespace_body(), clazz);
  TypeVarResolver resolver(compile_session(), method_def);

  WorkList_<Instruction> name_refs;
  {
    foreach (Function::EnumI, insts, fun) {
      auto& inst = *insts.Get();
      if (auto const name_ref_inst = inst.DynamicCast<NameRefI>()) {
        name_refs.Push(name_ref_inst);
      } else {
        auto& new_outy = Resolve(context, inst.output_type());
        if (inst.output_type() != new_outy) {
          DEBUG_FORMAT("update output type of %s to %s", inst, new_outy);
          inst.set_output_type(new_outy);

          if (inst.opcode() == Op_Prologue) {
            auto& new_funty = FunctionType::Intern(
                fun.function_type().return_type(),
                *new_outy.StaticCast<ValuesType>());
            DEBUG_FORMAT("update function type of %s to %s", fun, new_funty);
            fun.SetFunctionType(new_funty);
          }
        }
        resolver.Add(inst);
      }
    }
  }

  {
    DEBUG_FORMAT("=== Resolve NameRef in %s ===", fun);
    NormalizeTasklet normalizer(compile_session(), fun.module());
    NameRefResolver resolver(compile_session(), method_def);
    while (!name_refs.IsEmpty()) {
      auto& name_ref_inst = *name_refs.Pop()->StaticCast<NameRefI>();
      normalizer.Add(name_ref_inst);
      normalizer.Add(name_ref_inst.output());
      resolver.Resolve(name_ref_inst);
    }
    normalizer.Start();
  }

  if (compile_session().HasError()) {
    return;
  }

  resolver.Resolve();

  if (compile_session().HasError()) {
    return;
  }

  {
    DEBUG_FORMAT("=== Rewrite TypeVars in %s ===", fun);
    TypeVarRewriter rewriter(compile_session(), fun.module());
    foreach (Function::EnumI, insts, fun) {
      auto& inst = *insts.Get();
      rewriter.Rewrite(inst);
    }
    rewriter.Finish();
  }

  {
    DEBUG_FORMAT("=== Update Variables in %s ===", fun);

    NormalizeTasklet normalizer(session(), fun.module());
    for (auto& var: fun.variables()) {
      auto& origty = var.GetTy();
      auto& varty = origty.ComputeBoundType();
      DEBUG_FORMAT("var %s %s", var, origty);
      if (varty != origty) {
        DEBUG_FORMAT("update type of var %s to %s", var, varty);
        Class& cell_type = Ty_ClosedCell->Construct(varty);
        auto& vardef_inst = *var.GetDefI();
        vardef_inst.set_output_type(cell_type);
        var.SetTy(varty);
        normalizer.Add(vardef_inst);
      }
    }
    normalizer.Start();
  }

  {
    auto& cur_funty = fun.function_type();

    auto& new_funty = FunctionType::Intern(
        Resolve(context, cur_funty.return_type()),
        *Resolve(context, cur_funty.params_type()).StaticCast<ValuesType>());

    if (cur_funty != new_funty) {
      DEBUG_FORMAT("update function type %s to %s",
          cur_funty,
          new_funty);

      fun.SetFunctionType(new_funty);
    }
  }
}

// [P]
/// <summary>
///     Resolves base class name and interface names.
/// </summary>
void FinalizeMethodPass::Process(MethodDef* const pMethodDef) {
  ASSERT(pMethodDef != nullptr);

  auto& method_def = *pMethodDef;

  if (!method_def.function()) {
    DEBUG_FORMAT("Ignore extern method %s", method_def);
    return;
  }

  ResolveContext context(
      method_def.namespace_body(),
      method_def.owner_class_def().GetClass());

  for (auto& fun: method_def.module().functions())
    FixFunction(method_def, fun);
}

} // Compiler
