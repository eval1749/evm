#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - TypeUnifier
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeVarRewriter.h"

#include "../../Il/Tasks/CodeEmitter.h"
#include "../../Il/Tasks/PropagateTypeTasklet.h"

#include "../CompileSession.h"
#include "../Ir.h"

#include "./MethodResolver.h"
#include "./PredefinedMethod.h"

namespace Compiler {

// ctor
TypeVarRewriter::TypeVarRewriter(
    CompileSession& session,
    const Module& module)
    : Tasklet("TypeVarRewriter", session),
      compile_session_(session),
      normalizer_(session, module) {}

// [C]
Field& TypeVarRewriter::ComputeBoundField(const Field& cur_field) {
  auto const cur_cclass_ptr = cur_field.owner_class()
      .DynamicCast<ConstructedClass>();
  if (!cur_cclass_ptr) {
    DEBUG_FORMAT("%s isn't constructed class", cur_field.owner_class());
    return const_cast<Field&>(cur_field);
  }

  auto& cur_cclass = *cur_cclass_ptr;
  TypeArgs tyargs;
  foreach (ConstructedClass::EnumTypeArg, en, cur_cclass) {
    tyargs.Add(en.GetTypeParam(), ComputeBoundType(en.GetTypeArg()));
  }
  auto& new_cclass = *cur_cclass.generic_class().Construct(tyargs)
      .StaticCast<ConstructedClass>();
  if (cur_cclass == new_cclass) {
    DEBUG_FORMAT("%s has no type var", cur_cclass);
    return const_cast<Field&>(cur_field);
  }

  auto const present = new_cclass.Find(cur_field.name());
  if (!present) {
    compile_session_.AddError(
       cur_field.source_info(),
       CompileError_Resolve_Field_NotFound,
       cur_field.name(),
       new_cclass);
    return const_cast<Field&>(cur_field);
  }

  if (auto const new_field = present->DynamicCast<Field>()) {
    return *new_field;
  }

  compile_session_.AddError(
      cur_field.source_info(),
      CompileError_Resolve_ExpectField,
      present);
  return const_cast<Field&>(cur_field);
}

// Bound constructed method.
Method& TypeVarRewriter::ComputeBoundMethod(const CallI& call_inst) {
  auto& cur_mt = ComputeBoundMethod(
      call_inst,
      *call_inst.op0().StaticCast<Method>());
  ASSERT(!cur_mt.Is<GenericMethod>());
  if (auto const cons_mt = cur_mt.DynamicCast<ConstructedMethod>()) {
    TypeArgs type_args;
    foreach (TypeArgs::Enum, en, cons_mt->type_args()) {
      type_args.Add(
        *en.Get().GetKey(),
        ComputeBoundType(*en.Get().GetValue()));
    }
    auto& bound_method = cons_mt->generic_method().Construct(type_args);
    DEBUG_FORMAT("bound method for %s is %s", *cons_mt, bound_method);
    return bound_method;
  }
  return cur_mt;
}

// Construct method in constructed class.
Method& TypeVarRewriter::ComputeBoundMethod(
    const CallI& call_inst,
    const Method& cur_mt) {
  auto const cur_cclass_ptr = cur_mt.owner_class()
      .DynamicCast<ConstructedClass>();
  if (!cur_cclass_ptr) {
    DEBUG_FORMAT("%s isn't constructed class", cur_mt.owner_class());
    return const_cast<Method&>(cur_mt);
  }

  auto& cur_cclass = *cur_cclass_ptr;
  TypeArgs tyargs;
  foreach (ConstructedClass::EnumTypeArg, en, cur_cclass) {
    tyargs.Add(en.GetTypeParam(), ComputeBoundType(en.GetTypeArg()));
  }
  auto& new_cclass = *cur_cclass.generic_class().Construct(tyargs)
      .StaticCast<ConstructedClass>();
  if (cur_cclass == new_cclass) {
    DEBUG_FORMAT("%s has no type var", cur_cclass);
    return const_cast<Method&>(cur_mt);
  }

  auto& new_mtg = *new_cclass.Find(cur_mt.name())->StaticCast<MethodGroup>();
  auto& new_funty = FunctionType::Intern(
      ComputeBoundType(cur_mt.return_type()),
      *ComputeBoundType(cur_mt.params_type()).StaticCast<ValuesType>());
  if (auto const new_thing = new_mtg.Find(new_funty)) {
    if (auto const new_mt = new_thing->DynamicCast<Method>()) {
      return *new_mt;
    }

    compile_session_.AddError(
       call_inst.source_info(),
       CompileError_Resolve_ExpectMethod,
       new_thing);
    return const_cast<Method&>(cur_mt);
  }

  compile_session_.AddError(
      call_inst.source_info(),
      CompileError_Resolve_Method_NoApplicableMethod,
      new_mtg,
      new_funty);
  return const_cast<Method&>(cur_mt);
}

const Type& TypeVarRewriter::ComputeBoundType(const Type& ty) {
  return ty.ComputeBoundType();
}

// [F]
void TypeVarRewriter::Finish() {
  normalizer_.Start();
}

bool TypeVarRewriter::FixInsturction(Instruction& inst) {
  auto changed = FixOutputType(inst, ComputeBoundType(inst.output_type()));

  if (auto const call_inst = inst.DynamicCast<CallI>()) {
    auto& callee = inst.op0();
    if (auto const cur_mt = callee.DynamicCast<Method>()) {
      auto& new_mt = ComputeBoundMethod(*call_inst);
      if (cur_mt != &new_mt) {
        DEBUG_FORMAT("update callee %s to %s", inst, new_mt);
        inst.GetOperandBox(0)->Replace(new_mt);
        FixOutputType(inst, new_mt.return_type());
      }

      MethodResolver resolver(
          compile_session_,
          call_inst->GetBBlock()->GetFunction()->module(),
          call_inst->source_info());
      resolver.FixMethodArgs(*call_inst);
      changed = true;

    } else if (auto const op_name_ref = callee.DynamicCast<NameRef>()) {
      FixOperatorCall(*call_inst);
    }

  } else if (auto const field_inst = inst.DynamicCast<FieldPtrI>()) {
    if (auto const cur_field = inst.op1().DynamicCast<Field>()) {
      auto& new_field = ComputeBoundField(*cur_field);
      if (cur_field != &new_field) {
        DEBUG_FORMAT("update field %s to %s", inst, new_field);
        inst.GetOperandBox(1)->Replace(new_field);
      }
    }
  }

  return changed;
}

void TypeVarRewriter::FixOperatorCall(CallI& call_inst) {
  MethodResolver resolver(
      compile_session_,
      call_inst.GetBBlock()->GetFunction()->module(),
      call_inst.source_info());

  auto const applicable_methods = resolver.ComputeApplicableMethods(
      call_inst);

  switch (applicable_methods.Count()) {
    case 0:
      compile_session_.AddError(
          call_inst.source_info(),
          CompileError_Instruction_Invalid,
          call_inst,
          "No such operator");
      return;

    case 1: {
      auto& method = *applicable_methods.Get(0);
      if (auto const predefined = method.DynamicCast<PredefinedMethod>()) {
        RewriteOperator(call_inst, *predefined);
      } else {
        call_inst.GetOperandBox(0)->Replace(method);
      }
      return;
    }

    default:
      compile_session_.AddError(
          call_inst.source_info(),
          CompileError_Resolve_Method_Ambiguous,
          call_inst,
          applicable_methods.Get(0),
          applicable_methods.Get(1));
      return;
  }
}

bool TypeVarRewriter::FixOutputType(
    Instruction& inst,
    const Type& new_outy) {
  if (inst.output_type() == new_outy) {
    return false;
  }

  DEBUG_FORMAT("update %s to %s", inst, new_outy);
  inst.set_output_type(new_outy);
  PropagateTypeTasklet task(
      compile_session_,
      inst.GetBBlock()->GetFunction()->module());
  task.Add(inst.output());
  task.Start();
  return true;
}

void TypeVarRewriter::Rewrite(Instruction& inst) {
  if (FixInsturction(inst)) {
    normalizer_.Add(inst);
  }
}

void TypeVarRewriter::RewriteOperator(
    CallI& call_inst,
    const PredefinedMethod& predefined_method) {
  class Local {
    public: static Instruction* GetGetterInstruction(const Operand& operand) {
      if (auto const call_inst = operand.def_inst()->DynamicCast<CallI>()) {
        if (auto const method = call_inst->op0().DynamicCast<Method>()) {
          return &method->name() == Q_get_Item ? call_inst : nullptr;
        }
      }
      return nullptr;
    }

    public: static MethodGroup* GetSetterMethodGroup(
        const Instruction& inst) {
      if (inst.Is<CallI>()) {
        auto const method = inst.op0().DynamicCast<Method>();
        return method
            ? method->owner_class().Find(*Q_set_Item)
                ->DynamicCast<MethodGroup>()
            : nullptr;
      }
      return nullptr;
    }
  };

  auto& args_inst = *call_inst.op1().def_inst()->StaticCast<ValuesI>();
  auto& arg0 = args_inst.op0();
  auto& type = arg0.type();

  CodeEmitter emitter(compile_session_, call_inst);
  auto& op_inst = Instruction::New(
      emitter.zone(),
      predefined_method.opcode());
  if (call_inst.output_type() == *Ty_Boolean) {
    op_inst.set_output(emitter.NewBoolOutput());
    op_inst.set_output_type(*Ty_Bool);
  } else {
    op_inst.set_output(emitter.NewOutput(call_inst.output_type()));
    op_inst.set_output_type(call_inst.output_type());
  }

  switch (predefined_method.rewrite_rule()) {
    case PredefinedMethod::LhsCmpRhs:
      op_inst.AppendOperand(arg0);
      op_inst.AppendOperand(args_inst.op1());
      emitter.Emit(op_inst);
      break;

    case PredefinedMethod::LhsEqFalse:
      op_inst.AppendOperand(arg0);
      op_inst.AppendOperand(emitter.NewLiteral(false));
      emitter.Emit(op_inst);
      break;

    case PredefinedMethod::LhsOpMinusOne:
      op_inst.AppendOperand(arg0);
      op_inst.AppendOperand(emitter.NewLiteral(type, -1));
      emitter.Emit(op_inst);
      break;

    case PredefinedMethod::LhsOpOne: {
      if (!arg0.Is<Register>()) {
        compile_session_.AddError(
            call_inst.source_info(),
            CompileError_Parse_Expect_LeftValue,
            arg0);
        return;
      }

      op_inst.AppendOperand(arg0);
      op_inst.AppendOperand(emitter.NewLiteral(type, 1));

      if (op_inst.output().Is<VoidOutput>()) {
        op_inst.set_output(emitter.NewRegister());
        op_inst.set_output_type(type);
      }

      emitter.Emit(op_inst);

      if (arg0.def_inst()->Is<LoadI>()) {
        // Increment/Decrement local variable
        auto& load_ptr_inst = *arg0.def_inst()->op0().def_inst();
        auto& store_ptr_inst = Instruction::New(
            emitter.zone(),
            load_ptr_inst.opcode());
        foreach (Instruction::EnumOperand, operands, load_ptr_inst) {
          store_ptr_inst.AppendOperand(*operands);
        }
        store_ptr_inst.set_output_type(load_ptr_inst.output_type());
        store_ptr_inst.set_output(emitter.NewRegister());
        emitter.Emit(store_ptr_inst);
        emitter.Store(
            *store_ptr_inst.output().StaticCast<Register>(),
            op_inst.output());
        break;
      }

      if (auto const getter_inst = Local::GetGetterInstruction(arg0)) {
        // Increment/Decrement indexer, e.g. array[i, j]++
        auto const setter = Local::GetSetterMethodGroup(*getter_inst);
        if (!setter) {
          compile_session_.AddError(
              call_inst.source_info(),
              CompileError_Parse_Expect_LeftValue,
              arg0);
          return;
        }

        auto& setter_args = emitter.NewValues();
        auto& setter_args_inst = *new(emitter.zone()) ValuesI(setter_args);
        Instruction::EnumOperand getter_args(getter_inst->op1().def_inst());

        setter_args_inst.AppendOperand(getter_args.Get());
        getter_args.Next();

        setter_args_inst.AppendOperand(op_inst.output());

        while (!getter_args.AtEnd()) {
          setter_args_inst.AppendOperand(getter_args.Get());
          getter_args.Next();
        }

        setter_args_inst.UpdateOutputType();
        emitter.Emit(setter_args_inst);

        auto& setter_inst = *new(emitter.zone()) CallI(
            *Ty_Void,
            *Void,
            *setter,
            setter_args);
        emitter.Emit(setter_inst);

        MethodResolver resolver(
            compile_session_,
            setter_inst.GetBBlock()->GetFunction()->module(),
            setter_inst.source_info());

        auto const applicable_methods = resolver.ComputeApplicableMethods(
            setter_inst);
        switch (applicable_methods.Count()) {
          case 0:
            compile_session_.AddError(
                call_inst.source_info(),
                CompileError_Resolve_Method_NoApplicableMethod,
                setter_inst,
                setter_inst.op1().type());
            return;

           case 1:
             setter_inst.GetOperandBox(0)->Replace(
                *applicable_methods.Get(0));
             break;

           default:
             compile_session_.AddError(
                setter_inst.source_info(),
                CompileError_Resolve_Method_Ambiguous,
                setter_inst,
                applicable_methods.Get(0),
                applicable_methods.Get(1));
              return;
        }
        break;
      }

      compile_session_.AddError(
          call_inst.source_info(),
          CompileError_Resolve_Unexpected,
          String::Format("Unsupported getter %s", arg0.def_inst()));
      break;
    }

    case PredefinedMethod::LhsOpRhs:
      ASSERT(arg0.type() == args_inst.op1().type());
      op_inst.AppendOperand(arg0);
      op_inst.AppendOperand(args_inst.op1());
      op_inst.set_output_type(arg0.type());
      emitter.Emit(op_inst);
      break;

    case PredefinedMethod::ZeroOpLhs:
      op_inst.AppendOperand(arg0);
      op_inst.AppendOperand(emitter.NewLiteral(type, 0));
      emitter.Emit(op_inst);
      break;

    default:
      compile_session_.AddError(
          call_inst.source_info(),
          CompileError_Resolve_Unexpected,
          String::Format("predfined_method.rewrite_rule() is %d",
            predefined_method.rewrite_rule()));
      break;
   }

  if (auto const old_out = call_inst.output().DynamicCast<SsaOutput>()) {
    if (auto const bool_out = op_inst.output().DynamicCast<BoolOutput>()) {
      auto& new_out = emitter.NewRegister();
      emitter.Emit(
        *new(emitter.zone()) IfI(
            *Ty_Boolean, new_out, *bool_out,
            emitter.NewLiteral(true),
            emitter.NewLiteral(false)));
      normalizer_.ReplaceAll(new_out, *old_out);
    } else {
      normalizer_.ReplaceAll(op_inst.output(), *old_out);
    }
  }

  call_inst.set_output(*Useless);
  normalizer_.Add(call_inst);
}

} // Compiler
