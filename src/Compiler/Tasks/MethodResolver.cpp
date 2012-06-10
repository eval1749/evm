#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Method Resolver
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (eval1749@)

#include "./MethodResolver.h"

#include "../CompileSession.h"
#include "./PredefinedMethod.h"
#include "./TypeUnifier.h"
#include "../../Il/Tasks/CodeEmitter.h"
#include "../../Il/Tasks/NormalizeTasklet.h"

namespace Compiler {

namespace {
static Class* Class_CompilerInternal;

static const Class* EnsureOperatorClass(const Type& type) {
  if (type.Is<PrimitiveType>()) {
    return Class_CompilerInternal;
  }

  return type.DynamicCast<Class>();
}

static bool IsMoreSpecific(const Type& type1, const Type& type2) {
  if (type1.Is<TypeParam>()) {
    return type2.Is<TypeParam>();
  }
  return type1.IsSubtypeOf(type2) != Subtype_No;
}

// TODO(yosi) 2012-06-10 MethodResolver.cpp IsMoreSpecific should return
// Unknown
static bool IsMoreSpecific(const Method& method1, const Method& method2) {
  ValuesType::ElementScanner scanner1(method1.function_type().params_type());
  ValuesType::ElementScanner scanner2(method2.function_type().params_type());
  while (scanner1.IsRequired() && scanner2.IsRequired()) {
    if (!IsMoreSpecific(scanner1.Get(), scanner2.Get())) {
      return false;
    }
    scanner1.Next();
    scanner2.Next();
  }

  if (!scanner1.IsRequired() && !scanner2.IsRequired()) {
    auto const resty1 = scanner1.GetRestType();
    auto const resty2 = scanner2.GetRestType();
    if (resty1 && resty2) {
      return IsMoreSpecific(*resty1, *resty2);
    }
    return !resty1;
  }

  return scanner1.IsRequired();
}

} // namespace

// ctor
MethodResolver::MethodResolver(
    CompileSession& session,
    const Module& module,
    const SourceInfo& source_info)
    : Tasklet("MethodResolver", session),
      m_compile_session(session),
      module_(module),
      source_info_(source_info) {}

// [B]
bool MethodResolver::BindTypeArg(
    const Method& method,
    TypeArgs& type_args,
    const Type& param_ty,
    const Type& arg_ty) const {

  if (param_ty == arg_ty) {
    return true;
  }

  if (auto const typaram = param_ty.DynamicCast<TypeParam>()) {
    if (auto const tyarg = type_args.Get(*typaram)) {
      return UnifyTypes(type_args, *tyarg, arg_ty);
    }

    if (typaram->owner() == method) {
      DEBUG_FORMAT(" bind %s to %s", *typaram, arg_ty);
      type_args.Add(*typaram, arg_ty);
      return true;
    }

    DEBUG_FORMAT(" try bind %s and %s", *typaram, arg_ty);
    return UnifyTypes(type_args, *typaram, arg_ty);
  }

  if (auto const arrty = param_ty.DynamicCast<ArrayType>()) {
    return BindTypeArg(
        method,
        type_args,
        arrty->element_type(),
        arg_ty.StaticCast<ArrayType>()->element_type());
  }

  if (auto const param_funty = param_ty.DynamicCast<FunctionType>()) {
    auto const arg_funty = arg_ty.StaticCast<FunctionType>();

    return
        BindTypeArg(
            method,
            type_args,
            param_funty->return_type(),
            arg_funty->return_type())
        && BindTypeArg(
            method,
            type_args,
            param_funty->params_type(),
            arg_funty->params_type());
  }

  if (auto const param_ptry = param_ty.DynamicCast<PointerType>()) {
    return BindTypeArg(
        method,
        type_args,
        param_ptry->pointee_type(),
        arg_ty.StaticCast<PointerType>()->pointee_type());
  }

  if (auto const param_vals_ty = param_ty.DynamicCast<ValuesType>()) {
    ValuesType::Enum enum_argty(arg_ty.StaticCast<ValuesType>());
    foreach (ValuesType::Enum, enum_paramty, param_vals_ty) {
      auto& arg_ty = enum_argty.Get();
      auto& param_ty = enum_paramty.Get();
      if (!BindTypeArg(method, type_args, param_ty, arg_ty)) {
        return false;
      }
      enum_argty.Next();
    }
    return true;
  }

  // TODO(yosi) 2012-02-12 NYI Integer type promotion for applicable method.
  return UnifyTypes(type_args, param_ty, arg_ty);
}

const Operand& MethodResolver::BoxingIfNeeded(
    OperandBox& opbox,
    const Type& paramty) {
  auto& op = *opbox.GetOperand();
  auto& argty = op.type();

  DEBUG_FORMAT("%s %s %s", argty, op, paramty);

  if (argty == paramty) {
    DEBUG_FORMAT("%s is %s", argty, paramty);
    return op;
  }

  if (paramty.IsSubtypeOf(*Ty_Value) == Subtype_Yes) {
    DEBUG_FORMAT("Target %s is %s", paramty, Ty_Value);
    return op;
  }

  if (argty.IsSubtypeOf(*Ty_Value) != Subtype_Yes) {
    DEBUG_FORMAT("Source %s isn't %s", argty, Ty_Value);
    return op;
  }

  DEBUG_FORMAT("Box %s %s", argty, op);

  auto& inst = *opbox.GetI();

  auto& r1 = module_.NewRegister();
  auto& box_inst = *new BoxI(&Ty_Box->Construct(argty), &r1, &op);
  inst.GetBB()->InsertBeforeI(box_inst, inst);

  auto& r2 = module_.NewRegister();
  auto& cast_inst = *new StaticCastI(paramty, r2, r1);
  inst.GetBB()->InsertBeforeI(cast_inst, inst);

  opbox.Replace(r2);
  return r2;
}

// [C]

// TODO(yosi) 2012-06-10 We should search method group in class base
// class and interfaces.
ArrayList_<Method*> MethodResolver::ComputeApplicableMethods(
    const CallI& call_inst) const {
  class Local {
    public: static const NameRef::SimpleName* ToSimpleName(
        const Operand& operand) {
      auto const name_ref = operand.DynamicCast<NameRef>();
      return name_ref ? name_ref->GetSimpleName() : nullptr;
    }
  };

  auto& callee = call_inst.op0();
  if (auto const mtg = callee.DynamicCast<MethodGroup>()) {
    return SelectMostSpecific(ComputeApplicableMethods(call_inst, *mtg));
  }

  if (auto const simple_name = Local::ToSimpleName(callee)) {
    auto& name = simple_name->name();
    auto& args_inst = *call_inst.op1().def_inst()->StaticCast<ValuesI>();
    auto& arg_type = args_inst.op0().type().ComputeBoundType();
    if (auto const clazz = EnsureOperatorClass(arg_type)) {
      if (auto const mtg = clazz->Find(name)->DynamicCast<MethodGroup>()) {
        return SelectMostSpecific(ComputeApplicableMethods(call_inst, *mtg));
      }

      return ArrayList_<Method*>();
    }

    if (auto const tyvar = arg_type.DynamicCast<TypeVar>()) {
      ArrayList_<Method*> applicable_methods;
      foreach (TypeVar::Enum, types, *tyvar) {
        if (auto const clazz = EnsureOperatorClass(
                types.Get()->ComputeBoundType())) {
          if (auto const mtg = clazz->Find(name)
                ->DynamicCast<MethodGroup>()) {
            applicable_methods.AddAll(
                SelectMostSpecific(
                    ComputeApplicableMethods(call_inst, *mtg)));
          }
        }
      }
      return SelectMostSpecific(applicable_methods);
    }
  }

  return ArrayList_<Method*>();
}

ArrayList_<Method*> MethodResolver::ComputeApplicableMethods(
    const CallI& call_inst,
    const MethodGroup& method_group) const {
  ArrayList_<Method*> applicable_methods;
  foreach (MethodGroup::EnumMethod, methods, method_group) {
   auto& method = *methods.Get();
   if (auto const gen_method = method.DynamicCast<GenericMethod>()) {
     TypeArgs type_args;
     if (IsApplicable(type_args, method, call_inst)) {
        applicable_methods.Add(&gen_method->Construct(type_args));
     }
   } else if (auto const cons_method =
                method.DynamicCast<ConstructedMethod>()) {
     TypeArgs type_args(cons_method->type_args());
     if (IsApplicable(type_args, method, call_inst)) {
        applicable_methods.Add(
            &cons_method->generic_method().Construct(type_args));
     }
   } else if (IsApplicable(method, call_inst)) {
     applicable_methods.Add(&method);
   }
  }
  return applicable_methods;
}

// [F]
void MethodResolver::FixMethodArgs(CallI& call_inst) {
  auto& args_inst = *call_inst.GetVy()->GetDefI();
  auto& method = *call_inst.GetSx()->StaticCast<Method>();

  Instruction::EnumOperand args(args_inst);
  Method::ParamTypeScanner params(method);

  // Boxing
  while (params.IsRequired()) {
    ASSERT(!args.AtEnd());
    BoxingIfNeeded(*args.GetBox(), params.Get());
    args.Next();
    params.Next();
  }

  // Array conversion
  auto const rest_type = params.GetRestType();
  if (!rest_type) {
      return;
  }

  auto& new_args_inst = *new ValuesI(method.params_type(), *new Values());

  new_args_inst.SetSourceInfo(&args_inst.source_info());

  call_inst.GetBB()->InsertBeforeI(new_args_inst, call_inst);

  call_inst.GetOperandBox(1)->Replace(*new_args_inst.GetVd());

  // New argument list
  {
    Instruction::EnumOperand args(args_inst);
    Method::ParamTypeScanner params(method);
    while (params.IsRequired()) {
      new_args_inst.AppendOperand(args.Get());
      args.Next();
      params.Next();
    }
  }

  // VALUES %v1 <= 1
  // NEWARRAY T[] %r2 <= %v1
  // ELTREF T* %r3 <= %r2 0
  // STORE %r3 %arg0
  // ELTREF T* %r4 <= %r2 1
  // STORE %r4 %arg1
  {
    DEBUG_FORMAT("Rest Arguments %s", args_inst);

    auto const num_elems = args_inst.CountOperands()
        - new_args_inst.CountOperands();

    CodeEmitter emitter(session(), new_args_inst);

    auto& elemty = rest_type->element_type();
    auto& array = emitter.NewArray(elemty, num_elems);
    new_args_inst.AppendOperand(array);
    auto index = 0;
    while (!args.AtEnd()) {
      auto& arg = BoxingIfNeeded(*args.GetBox(), elemty);
      auto& pointer = emitter.EltRef(array, index);
      emitter.Store(pointer, arg);
      ++index;
      args.Next();
    }
  }

  NormalizeTasklet normalizer(session(), module_);
  normalizer.Add(args_inst);
  normalizer.Start();
}

// [I]
void MethodResolver::Init() {
  struct Local {
    static void AddBinary(
        const char* name_string,
        const Type& outy,
        const Class& oprty,
        Op const opcode) {
      auto& name = Name::Intern(name_string);
      auto& method_group = InternMethodGroup(*Class_CompilerInternal, name);
      ValuesTypeBuilder builder(2);
      builder.Append(oprty);
      builder.Append(oprty);
      auto& method = *new PredefinedMethod(
        method_group,
        Modifier_Public | Modifier_Static,
        FunctionType::Intern(outy, builder.GetValuesType()),
        opcode,
        outy == *Ty_Boolean
            ? PredefinedMethod::LhsCmpRhs
            : PredefinedMethod::LhsOpRhs);
      method_group.Add(method);
    }

    static void AddUnary(
        const char* name_string,
        const Class& clazz,
        Op const opcode,
        PredefinedMethod::RewriteRule rewrite_rule) {
      auto& name = Name::Intern(name_string);
      auto& method_group = InternMethodGroup(*Class_CompilerInternal, name);
      auto& method = *new PredefinedMethod(
        method_group,
        Modifier_Public | Modifier_Static,
        FunctionType::Intern(clazz, ValuesType::Intern(clazz)),
        opcode,
        rewrite_rule);
      method_group.Add(method);
    }

    static MethodGroup& InternMethodGroup(
        Class& clazz, 
        const Name& name) {
      if (auto const present = clazz.Find(name)) {
        return *present->StaticCast<MethodGroup>();
      }
      auto& method_group = *new MethodGroup(*Class_CompilerInternal, name);
      clazz.Add(name, method_group);
      return method_group;
    }
  };

  Class_CompilerInternal = new Class(
    Namespace::CreateGlobalNamespace(),
    0,
    Name::Intern("CompilerInternal"));
  Class_CompilerInternal->RealizeClass(CollectionV_<const Class*>(Ty_Object));

  CollectionV_<const Type*> type_list(
      Ty_Float32,
      Ty_Float64,
      Ty_Int32,
      Ty_Int64,
      Ty_UInt32,
      Ty_UInt64);
  foreach (CollectionV_<const Type*>::Enum, types, type_list) {
    auto& clazz = *types.Get()->StaticCast<Class>();
    Local::AddUnary("operator +", clazz, Op_Add,
        PredefinedMethod::ZeroOpLhs);
    Local::AddUnary("operator ++", clazz, Op_Add,
        PredefinedMethod::LhsOpOne);
    Local::AddUnary("operator -", clazz, Op_Sub,
        PredefinedMethod::ZeroOpLhs);
    Local::AddUnary("operator --", clazz, Op_Sub,
        PredefinedMethod::LhsOpOne);
    Local::AddUnary("operator ~", clazz, Op_LogXor,
        PredefinedMethod::LhsOpMinusOne);

    Local::AddBinary("operator +", clazz, clazz, Op_Add);
    Local::AddBinary("operator -", clazz, clazz, Op_Sub);
    Local::AddBinary("operator *", clazz, clazz, Op_Mul);
    Local::AddBinary("operator /", clazz, clazz, Op_Div);
    Local::AddBinary("operator %", clazz, clazz, Op_Rem);

    Local::AddBinary("operator <<", clazz, clazz, Op_Shl);
    Local::AddBinary("operator >>", clazz, clazz, Op_Shr);

    Local::AddBinary("operator &", clazz, clazz, Op_LogAnd);
    Local::AddBinary("operator |", clazz, clazz, Op_LogIor);
    Local::AddBinary("operator ^", clazz, clazz, Op_LogXor);

    Local::AddBinary("operator ==", *Ty_Boolean, clazz, Op_Eq);
    Local::AddBinary("operator >=", *Ty_Boolean, clazz, Op_Ge);
    Local::AddBinary("operator >", *Ty_Boolean, clazz, Op_Gt);
    Local::AddBinary("operator <=", *Ty_Boolean, clazz, Op_Le);
    Local::AddBinary("operator <", *Ty_Boolean, clazz, Op_Lt);
    Local::AddBinary("operator !=", *Ty_Boolean, clazz, Op_Ne);
  }

  Local::AddUnary("operator !", *Ty_Boolean, Op_Ne, 
      PredefinedMethod::LhsEqFalse);
}

bool MethodResolver::IsApplicable(
    const Method& method,
    const CallI& call_inst) const {
  ASSERT(!method.Is<GenericMethod>());
  ASSERT(!method.Is<ConstructedMethod>());
  TypeArgs type_args;
  return IsApplicable(type_args, method, call_inst);
}

// Note: Method return type isn't involved applicable method checking.
bool MethodResolver::IsApplicable(
    TypeArgs& type_args,
    const Method& method,
    const CallI& call_inst) const {

  DEBUG_FORMAT("%s %s %s", method, call_inst, call_inst.args_type());

  auto const has_this_arg =
      call_inst.op0().Is<MethodGroup>()
      || (call_inst.op0().Is<Method>()
          && !call_inst.op0().StaticCast<Method>()->IsStatic());

  auto& params_type = method.params_type();
  auto const num_params = params_type.Count();

  auto& args_type = call_inst.args_type();
  auto const num_args = args_type.Count() - (has_this_arg ? 1 : 0);

  if (num_params > num_args) {
    DEBUG_FORMAT("Requires more arguments %d > %d %s",
        num_params, num_args, args_type);
    return false;
  }

  ValuesType::Enum args(args_type);
  if (has_this_arg) {
    args.Next(); // skip "this" argument.
  }

  Method::ParamTypeScanner params(method);

  while (params.IsRequired()) {
    if (args.AtEnd()) {
        return false;
    }

    if (!BindTypeArg(method, type_args, params.Get(), args.Get())) {
      DEBUG_FORMAT("param(%s) != arg(%s)", params.Get(), args.Get());
      return false;
    }

    args.Next();
    params.Next();
}

  auto const resty = params.GetRestType();

  if (args.AtEnd()) {
    DEBUG_FORMAT(
        resty == nullptr
            ? "Fixed match %s %s"
            : "Empty rest match %s %s",
        method.GetKind(),
        method);
    return true;
  }

  if (resty == nullptr) {
    return false;
  }

  auto& param_type = resty->element_type();
  while (!args.AtEnd()) {
    auto& arg_type = args.Get();
    if (!BindTypeArg(method, type_args, param_type, arg_type)) {
      return false;
    }
    args.Next();
  }

  DEBUG_FORMAT("Rest match %s", method);
  return true;
}

// [S]
// TODO(yosi) 2012-06-10 MethodResolverSelectMostSpecific should return
// multiple methods.
ArrayList_<Method*> MethodResolver::SelectMostSpecific(
    const ArrayList_<Method*>& methods) const {
  if (methods.Count() <= 1) {
    return ArrayList_<Method*>(methods);
  }

  ArrayList_<Method*>::Enum it(methods);
  auto most_specific_method = *it;
  it.Next();
  while (!it.AtEnd()) {
    auto& another = **it;
    if (IsMoreSpecific(another, *most_specific_method)) {
      most_specific_method = &another;
    }
    it.Next();
  }

  ArrayList_<Method*> result(1);
  result.Add(most_specific_method);
  return result;
}

// [U]
bool MethodResolver::UnifyTypes(
    TypeArgs& type_args,
    const Type& lhs_ty,
    const Type& rhs_ty) const {
  TypeUnifier unifier(m_compile_session, source_info_);
  if (!unifier.Unify(lhs_ty, rhs_ty)) {
    return false;
  }

  foreach (TypeUnifier::EnumTypeParam, params, unifier) {
    auto& typaram = params.Get();
    auto& bound_type = params.GetBoundType();
    if (typaram != bound_type) {
      DEBUG_FORMAT(" bind %s to %s", typaram, bound_type);
      type_args.Add(typaram, bound_type);
    }
  }
  return true;
}

} // Compiler
