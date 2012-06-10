#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - TypeUnifier
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeVarResolver.h"

#include "../CompileSession.h"
#include "../Ir.h"

#include "./MethodResolver.h"
#include "./TypeUnifier.h"

namespace Compiler {

// ctor
TypeVarResolver::TypeVarResolver(
    CompileSession& session,
    const MethodDef& method_def)
    : Tasklet("TypeVarResolver", session),
      compile_session_(session),
      method_def_(method_def) {}

// [A]
void TypeVarResolver::Add(Instruction& inst) {
  class Local {
    public: static bool IsTypeVarBinder(const Instruction& inst) {
      return inst.Is<CallI>()
        || inst.opcode() == Op_StaticCast
        || inst.Is<PhiI>()
        || inst.Is<RetI>();
    }
  };

  if (Local::IsTypeVarBinder(inst)) {
    type_var_insts_.Add(&inst);
  }
}

// [F]
// TODO(yosi) 2012-05-03 FixTypeVarWithMethodCall is identical to
// UpdateTypeVarWithMethod except for TypeVar::And or TypeVar::Or.
void TypeVarResolver::FixTypeVarWithMethodCall(const CallI& call_inst) {
  auto& method = *call_inst.op0().StaticCast<Method>();
  TypeArgs empty_type_args;
  auto& type_args = method.Is<ConstructedMethod>()
      ? method.StaticCast<ConstructedMethod>()->type_args()
      : empty_type_args;
  auto& argsty = *call_inst.op1().type().StaticCast<ValuesType>();
  auto& outy = call_inst.output_type();

  if (auto const tyvar = outy.DynamicCast<TypeVar>()) {
    tyvar->And(method.return_type().Construct(type_args));
    type_var_set_.Add(tyvar);
  }

  Method::ParamTypeScanner params(method);
  ValuesType::Enum args(argsty);

  if (!method.IsStatic()) {
    // Skip "this" argument.
    args.Next();
  }

  while (params.IsRequired()) {
    if (auto const tyvar = args->DynamicCast<TypeVar>()) {
      auto& paramty = params.Get().Construct(type_args);
      DEBUG_FORMAT("outy=%s rety=%s", tyvar, paramty);
      tyvar->And(paramty);
      type_var_set_.Add(tyvar);
    }
    args.Next();
    params.Next();
  }

  if (auto const resty = params.GetRestType()) {
    auto& paramty = resty->element_type().Construct(type_args);
    while (!args.AtEnd()) {
      if (auto const tyvar = args->DynamicCast<TypeVar>()) {
        DEBUG_FORMAT("argty=%s paramty=%s", tyvar, paramty);
        tyvar->And(paramty);
        type_var_set_.Add(tyvar);
      }
      args.Next();
    }

  } else if (!args.AtEnd()) {
    compile_session_.AddError(
      call_inst.source_info(),
      CompileError_Instruction_Invalid,
      call_inst);
    return;
  }
}

// [F]
void TypeVarResolver::Resolve() {
  {
    auto changed = true;
    while (changed) {
      DEBUG_FORMAT("type_var_insts_=%s", type_var_insts_);
      changed = false;
      WorkList_<Instruction> pendings;
      WorkList_<Instruction> pending_calls;
      foreach (ArrayList_<Instruction*>::Enum, insts, type_var_insts_) {
        auto& inst = *insts.Get();
        if (inst.Is<CallI>()) {
          pending_calls.Push(&inst);
        } else {
          pendings.Push(&inst);
        }
      }

      type_var_insts_.Clear();

      while (!pendings.IsEmpty()) {
        auto& inst = *pendings.Pop();
        if (UpdateTypeVar(inst)) {
          type_var_insts_.Add(&inst);
          changed = true;
        }
      }

      if (compile_session_.HasError()) {
        return;
      }

      while (!pending_calls.IsEmpty()) {
        auto& inst = *pending_calls.Pop();
        if (UpdateTypeVar(inst)) {
          type_var_insts_.Add(&inst);
          changed = true;
        }
      }

      if (compile_session_.HasError()) {
        return;
      }

      ArrayList_<const TypeVar*> fixed_type_vars;
      foreach (HashSet_<const TypeVar*>::Enum, typevars, type_var_set_) {
        auto& tyvar = *typevars.Get();
        if (tyvar.IsBound()) {
          DEBUG_FORMAT("bound %s", tyvar);

        } else if (tyvar.IsEmpty()) {
          compile_session_.AddError(
            method_def_.source_info(),
            CompileError_Resolve_Type_Conflict,
            tyvar);

        } else if (tyvar.IsFixed()) {
          const_cast<TypeVar&>(tyvar).Close();
          fixed_type_vars.Add(&tyvar);
        }
      }

      if (compile_session_.HasError()) {
        return;
      }

      foreach (ArrayList_<const TypeVar*>::Enum, tyvars, fixed_type_vars) {
        auto const tyvar = tyvars.Get();
        type_var_set_.Remove(tyvar);
      }

      if (type_var_set_.IsEmpty()) {
        break;
      }
    }
  }

  foreach (HashSet_<const TypeVar*>::Enum, types, type_var_set_) {
    auto& tyvar = *types.Get();
    if (!tyvar.IsBound()) {
      compile_session_.AddError(
        method_def_.source_info(),
        CompileError_Resolve_Type_Conflict,
        tyvar);
    }
  }
}

// [U]
bool TypeVarResolver::UnifyTypes(
    const Instruction& inst,
    const Type& lhs,
    const Type& rhs) {
  TypeUnifier unifier(compile_session_, inst.source_info());
  if (!unifier.Unify(lhs, rhs)) {
    compile_session_.AddError(
        inst.source_info(),
        CompileError_Resolve_Type_Conflict,
        lhs,
        rhs);
    return false;
  }

  auto has_tyvar = false;
  foreach (TypeUnifier::EnumTypeVar, tyvars, unifier) {
    auto& tyvar = const_cast<TypeVar&>(tyvars.Get());
    auto& ty = tyvars.GetBoundType();
    type_var_set_.Add(&tyvar);
    if (ty.IsBound()) {
      auto& andty = tyvar.And(ty);
      DEBUG_FORMAT(" type_and(%s,  %s) = %s", tyvar, ty, andty);
      if (andty == *Ty_Void) {
        compile_session_.AddError(
            inst.source_info(),
            CompileError_Resolve_Type_Conflict,
            lhs,
            rhs);
        return false;
      }
    }
    has_tyvar = true;
  }
  return has_tyvar;
}

// Following instructions provides type constraints:
//  o STATICAST T %r2 <= %r1:S
//  o Call T %v2 <= Method %v1
//  o PHI T %r1 <= ...
//  o RET %r1:T
//  ? VALUES T %v3 <= %r1 %r2...
bool TypeVarResolver::UpdateTypeVar(const Instruction& inst) {
  if (auto const call_inst = inst.DynamicCast<CallI>()) {
    return UpdateTypeVarWithCall(*call_inst);
  }

  if (inst.opcode() == Op_StaticCast) {
    DEBUG_FORMAT("unify output of %s and %s", inst, inst.op0().type());
    return UnifyTypes(inst, inst.output_type(), inst.op0().type());
  }

  if (inst.Is<PhiI>()) {
    bool has_tyvar = false;
    auto& outy = inst.output_type();
    foreach (Instruction::EnumOperand, operands, inst) {
      if (UnifyTypes(inst, outy, operands.Get()->type())) {
        has_tyvar = true;
      }
      if (compile_session_.HasError()) {
        return false;
      }
    }
    return has_tyvar;
  }

  if (inst.Is<RetI>()) {
    DEBUG_FORMAT("%s", inst);
    return UnifyTypes(inst, method_def_.return_type(), inst.op0().type());
  }

  return false;
}

bool TypeVarResolver::UpdateTypeVarWithCall(const CallI& call_inst) {
  DEBUG_FORMAT("%s", call_inst);

  auto& callee = call_inst.op0();
  if (callee.Is<Output>()) {
    return false;
  }

  if (auto const mtg = callee.DynamicCast<MethodGroup>()) {
    return UpdateTypeVarWithMethodGroup(call_inst);
  }

  if (auto const method = callee.DynamicCast<Method>()) {
    FixTypeVarWithMethodCall(call_inst);
    return true;
  }

  // TODO(yosi) 2012-05-03 This code fragment should be removed, because
  // parse doesn't emit CALL instruction with function object.
  if (callee.Is<Function>()) {
    // for-each and using statements generate Function object callee for
    // calling Disposable<T>.Dispose method.
    return false;
  }

  if (auto const name_ref = callee.DynamicCast<NameRef>()) {
    return UpdateTypeVarWithOperator(call_inst);
  }

  if (callee.Is<TypeVar>()) {
    // Maybe constructor.
    return false;
  }

  compile_session_.AddError(
      call_inst.source_info(),
      CompileError_Instruction_Invalid,
      call_inst,
      String::Format("Invalid callee %s", callee.GetKind()));
  return false;
}

bool TypeVarResolver::UpdateTypeVarWithMethod(
    const CallI& call_inst,
    const Method& method) {
  auto& args_inst = call_inst.args_inst();
  auto& argsty = *args_inst.output_type().StaticCast<ValuesType>();
  auto& outy = call_inst.output_type();

  if (auto const tyvar = outy.DynamicCast<TypeVar>()) {
    tyvar->Or(method.return_type());
    type_var_set_.Add(tyvar);
  }

  Method::ParamTypeScanner params(method);
  ValuesType::Enum args(argsty);
  // Skip "this" argument.
  args.Next();

  while (params.IsRequired()) {
    if (auto const tyvar = args->DynamicCast<TypeVar>()) {
      auto& paramty = params.Get();
      DEBUG_FORMAT("outy=%s rety=%s", tyvar, paramty);
      tyvar->Or(paramty);
      type_var_set_.Add(tyvar);
    }
    args.Next();
    params.Next();
  }

  if (auto const resty = params.GetRestType()) {
    auto& paramty = resty->element_type();
    while (!args.AtEnd()) {
      if (auto const tyvar = args->DynamicCast<TypeVar>()) {
        DEBUG_FORMAT("argty=%s paramty=%s", tyvar, paramty);
        tyvar->Or(paramty);
        type_var_set_.Add(tyvar);
      }
      args.Next();
    }

  } else if (!args.AtEnd()) {
    compile_session_.AddError(
      call_inst.source_info(),
      CompileError_Instruction_Invalid,
      call_inst,
      args_inst);
    return false;
  }

  return true;
}

// Note: To call static method with this parameter is extend method call.
// e.g. class StringExtend { static void Trim(String this x) { ... } }
// Note: Optional parameter is lower prioiry in lookup.
// e.g. Bar(int x = 0) < Bar(void)
bool TypeVarResolver::UpdateTypeVarWithMethodGroup(const CallI& call_inst) {
  MethodResolver resolver(
      compile_session_,
      call_inst.GetBBlock()->GetFunction()->module(),
      call_inst.source_info());

  auto const applicable_methods = resolver.ComputeApplicableMethods(
      call_inst);

  switch (applicable_methods.Count()) {
    case 0:
      DEBUG_FORMAT("No applicable method: %s", call_inst);
      compile_session_.AddError(
          call_inst.source_info(),
          CompileError_Resolve_Method_NoApplicableMethod,
          call_inst,
          call_inst.op1().type());
      return false;

    case 1: {
      auto& method = *applicable_methods.Get(0);
      call_inst.GetOperandBox(0)->Replace(method);
      DEBUG_FORMAT("One applicable method: %s", call_inst);
      ASSERT(!method.Is<GenericMethod>());

      auto& args_inst = call_inst.args_inst();
      auto& argsty = *args_inst.output_type().StaticCast<ValuesType>();

      ValuesType::Enum types(argsty);

      if (method.IsStatic()) {
        if (types.Get() != *Ty_Void) {
          compile_session_.AddError(
            call_inst.source_info(),
            CompileError_Resolve_Method_StaticMethod,
            call_inst);
          return false;
        }

        DEBUG_FORMAT("Remove first argument: %s", args_inst);
        args_inst.RemoveOperand(args_inst.GetOperandBox(0));

      } else if (types.Get() == *Ty_Void) {
        if (method_def_.IsStatic()) {
          compile_session_.AddError(
            call_inst.source_info(),
            CompileError_Resolve_Method_InstanceMethod,
            call_inst);
          return false;
        }

        DEBUG_FORMAT("Replace 'this' argument: %s", args_inst);
        args_inst.GetOperandBox(0)->Replace(method_def_.this_operand());
      }

      args_inst.UpdateOutputType();

      FixTypeVarWithMethodCall(call_inst);
      return true;
    }

    default: {
      DEBUG_FORMAT("%s has %d applicable methods: %s",
        call_inst,
        applicable_methods.Count(),
        applicable_methods);
      auto& outy = call_inst.output_type();
      auto& args_inst = call_inst.args_inst();
      auto& argsty = *args_inst.output_type().StaticCast<ValuesType>();

      foreach (ArrayList_<Method*>::Enum, methods, applicable_methods) {
        auto& method = *methods.Get();
        UpdateTypeVarWithMethod(call_inst, method);
        if (auto const tyvar = outy.DynamicCast<TypeVar>()) {
          tyvar->Or(method.return_type());
          type_var_set_.Add(tyvar);
        }

        Method::ParamTypeScanner params(method);
        ValuesType::Enum args(argsty);
        args.Next();

        while (params.IsRequired()) {
          if (auto const tyvar = args->DynamicCast<TypeVar>()) {
            auto& paramty = params.Get();
            DEBUG_FORMAT("outy=%s rety=%s", tyvar, paramty);
            tyvar->Or(paramty);
            type_var_set_.Add(tyvar);
          }
          args.Next();
          params.Next();
        }

        if (auto const resty = params.GetRestType()) {
          auto& paramty = resty->element_type();
          while (!args.AtEnd()) {
            if (auto const tyvar = args->DynamicCast<TypeVar>()) {
              DEBUG_FORMAT("argty=%s paramty=%s", tyvar, paramty);
              tyvar->Or(paramty);
              type_var_set_.Add(tyvar);
            }
            args.Next();
          }

        } else if (!args.AtEnd()) {
          compile_session_.AddError(
            call_inst.source_info(),
            CompileError_Instruction_Invalid,
            call_inst,
            args_inst);
          return false;
        }
      }
      return true;
    }
  }
}

bool TypeVarResolver::UpdateTypeVarWithOperator(const CallI& call_inst) {
  auto& args_inst = call_inst.args_inst();
  auto const clazz = args_inst.op0().type().DynamicCast<Class>();
  if (!clazz) {
    DEBUG_FORMAT("type=%s", args_inst.op0().type());
    return false;
  }
  return false;
}

} // Compiler
