#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - ResolveMethodPass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ResolveMethodPass.h"

#include "../CompileSession.h"
#include "../Ir.h"
#include "../../Il/Tasks/PropagateTypeTasklet.h"

namespace Compiler {

using Il::Tasks::PropagateTypeTasklet;

ResolveMethodPass::ResolveContextForMethod::ResolveContextForMethod(
    const MethodDef& method_def)
    : ResolveContext(
        method_def.namespace_body(),
        method_def.owner_class_def().GetClass()),
      method_def_(method_def) {}

void ResolveMethodPass::ResolveContextForMethod::Resolve(
    const Name& name,
    ArrayList_<const Operand*>& presents) const {
  foreach (MethodDef::EnumTypeParam, typaram_defs, method_def_) {
    auto& typaram_def = *typaram_defs.Get();
    if (typaram_def.name() == name) {
      presents.Add(&typaram_def.type_param());
      return;
    }
  }
  ResolveContext::Resolve(name, presents);
}

// ctor
ResolveMethodPass::ResolveMethodPass(CompileSession* const session)
    : Base(L"ResolveMethodPass", session) {}

// [C]
/// <summary>
///   Compute method signature. Note: method signature doesn't contain
///   "this" parameter.
/// </summary>
FunctionType& ResolveMethodPass::ComputeMethodSignature(
    const MethodDef& method_def) {

  ResolveContextForMethod context(method_def);
  ValuesTypeBuilder builder(method_def.CountParams());
  foreach (MethodDef::EnumParam, params, method_def) {
    auto const param = params.Get();
    auto& type = Resolve(context, *param->GetTy());
    builder.Append(type);
  }

  return FunctionType::Intern(
      Resolve(context , method_def.return_type()),
      builder.GetValuesType());
}

// [I]
MethodGroup& ResolveMethodPass::InternMethodGroup(MethodDef& method_def) {
  auto& mg_def = method_def.method_group_def();
  if (mg_def.IsRealized()) {
    auto& method_group = mg_def.method_group();
    DEBUG_FORMAT("Found %s", method_group);
    return method_group;
  }

  auto& clazz = method_def.owner_class_def().GetClass();
  auto& method_name = method_def.name();
  auto& method_group = *new MethodGroup(clazz, method_name);
  clazz.Add(method_name, method_group);
  mg_def.Realize(method_group);

  DEBUG_FORMAT("Register %s", method_group);
  return method_group;
}

/// <summary>
///  Install default constructor if class doesn't have ctor.
///  Note: To support partial class, we install default ctor here instead
///  of parser.
/// </summary>
void ResolveMethodPass::InstallDefaultCtor(
    Class& clazz,
    const SourceInfo& source_info) {
  if (clazz.IsStatic() || clazz.IsInterface()) {
    // Static class and interface don't have ctor.
    return;
  }

  if (clazz.GetBaseClass() == Ty_Enum) {
    // Enum doesn't have ctor.
    return;
  }

  if (clazz.Find(*QD_ctor) != nullptr) {
    // This class has user defined ctor.
    return;
  }

  auto& module = *new Module();

  auto& method_group = *new MethodGroup(clazz, *QD_ctor);
  clazz.Add(*QD_ctor, method_group);

  auto& method = *new Method(
      method_group,
      Modifier_Public,
      FunctionType::Intern(*Ty_Void, ValuesType::Intern()));

  method_group.Add(method);

  auto& fun = module.NewFunction(nullptr, Function::Flavor_Named, *QD_ctor);
  fun.SetUp();
  fun.SetFunctionType(
      FunctionType::Intern(*Ty_Void, ValuesType::Intern(clazz)));

  auto const pStartBB = fun.GetStartBB();
  pStartBB->AppendI(*new(module.zone()) RetI(*Void));

  foreach (Function::EnumI, insts, fun) {
    insts.Get()->set_source_info(source_info);
  }

  method.SetFunction(fun);
}

// [P]
/// <summary>
///     Resolves base class name and interface names.
/// </summary>
void ResolveMethodPass::Process(ClassDef* const class_def) {
  ASSERT(class_def != nullptr);
  ASSERT(class_def->IsRealized());
  Base::Process(class_def);
  auto& clazz = class_def->GetClass();
  InstallDefaultCtor(clazz, class_def->source_info());
  foreach (ClassDef::EnumMember, members, *class_def) {
    if (auto const prop_def = members.Get()->DynamicCast<PropertyDef>()) {
      auto& prop = *new Property(
          clazz,
          prop_def->modifiers(),
          Resolve(
              ResolveContext(prop_def->namespace_body(), clazz),
              prop_def->property_type()),
          prop_def->name());
      clazz.Add(prop.name(), prop);
      foreach (PropertyDef::EnumMember, members, *prop_def) {
        auto entry = members.Get();
        auto& name = *entry.GetKey();
        auto& method = *entry.GetValue()->operand().StaticCast<Method>();
        prop.Add(name, method);
      }
    }
  }
}

//  1. Construct method or generic method object.
//  2. Set parameter names.
//  3. Record method object to MethodDef
//  4. Add method object ot method group object.
//  5. Set function object to method object.
//     Parser creates a function object for method.
//  6. Set function type
void ResolveMethodPass::Process(MethodDef* const method_def_ptr) {
  ASSERT(method_def_ptr != nullptr);

  auto& method_def = *method_def_ptr;
  auto& method_group = InternMethodGroup(method_def);

  ArrayList_<const TypeParam*> type_params;
  foreach (MethodDef::EnumTypeParam, oEnum, method_def) {
    type_params.Add(&oEnum.Get()->type_param());
  }

  auto& method = type_params.IsEmpty()
      ? *new Method(
          method_group,
          method_def.GetModifiers(),
          ComputeMethodSignature(method_def))
      : *new GenericMethod(
          method_group,
          method_def.GetModifiers(),
          ComputeMethodSignature(method_def),
          type_params);

  ResolveContext context(method_def.namespace_body(), method.owner_class());

  {
    auto index = 0;
    foreach (MethodDef::EnumParam, params, method_def) {
      auto& param = *params.Get();
      method.SetParamName(index, param.name());
      param.set_type(Resolve(context, *param.GetTy()));
      const_cast<Variable&>(param.GetVariable()).SetTy(param.type());
      index++;
    } // for param
  }

  method_def.Realize(method);
  method_group.Add(method);

  // TODO 2011-05-08 eval1749@ NYI check same method signature.
  // Note: instance method foo() and static method foo() have same method
  // signature even if function signatures are different. Function signature
  // of instance method contains "this" parameter.

  // Note: Methods in interface don't have function object.
  if (auto const fun = method_def.function()) {
    method.SetFunction(*fun);
    fun->SetMethod(method);

    ValuesTypeBuilder builder(
        method_def.CountParams() + method.IsStatic() ? 0 : 1);

    if (!method.IsStatic()) {
      builder.Append(method_group.owner_class());
    }

    foreach (MethodDef::EnumParam, params, method_def) {
      auto& param = *params.Get();
      auto& type = Resolve(context, *param.GetTy());
      builder.Append(type);
    }

    fun->SetFunctionType(
        FunctionType::Intern(
            method.return_type(),
            builder.GetValuesType()));

    auto const prologueI = fun->GetPrologueI();
    prologueI->set_output_type(fun->function_type().params_type());

    if (auto const v1 = prologueI->GetVd()) {
      PropagateTypeTasklet type_propagater(session(), method_def.module());
      type_propagater.Add(*v1);
      type_propagater.Start();
    }
  }

  foreach (Module::EnumFunction, funs, method_def.module()) {
    auto& fun = *funs.Get();
    foreach (Function::EnumUpVar, upvars, fun) {
      auto const upvar = upvars.Get();
      auto const upvarI = upvars.GetI();
      auto& outy = upvar->GetDefI()->output_type();
      DEBUG_FORMAT("Resolve %s => %s", upvarI, outy);
      upvarI->set_output_type(outy);
    }
  }

  DEBUG_FORMAT("done: %s", method_def);
}

} // Compiler
