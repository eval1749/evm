#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Fasl - Fast Reader
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FastLoader.h"

#include "./FaslErrorInfo.h"
#include "./Name64.h"

#include "../Ir.h"
#include "../Ir/Instructions.h"
#include "../Tasks/VerifyTask.h"

namespace Il {
namespace Fasl {

using Common::Io::Stream;
using Il::Tasks::VerifyTask;

namespace {

static const char* const k_FaslOp[] = {
    #define DEFOP(mp_name) # mp_name,
    #include "./FaslOp.inc"
};

} // namespace

FastLoader::FastLoader(
    Session& session,
    Namespace& global_ns,
    const String& file_name,
    Callback* callback)
    : Tasklet(L"FastLoader", session),
      callback_(callback),
      global_ns_(global_ns),
      ALLOW_THIS_IN_INITIALIZER_LIST(reader_(session, file_name, *this)) {
  DEBUG_PRINTF("%p\n", this);
  ResetAll();
}

FastLoader::~FastLoader() {
  DEBUG_PRINTF("%p\n", this);
}

// [E]
void FastLoader::Error(FaslError const error_code) {
  // TODO(yosi) 2012-02-14 How do we specify file name to FastLoader?
  SourceInfo source_info;
  reader_.Error(FaslErrorInfo(source_info, error_code));
}

void FastLoader::Error(FaslError const error_code, Box a) {
  // TODO(yosi) 2012-02-14 How do we specify file name to FastLoader?
  SourceInfo source_info;
  reader_.Error(FaslErrorInfo(source_info, error_code, a));
}

void FastLoader::Error(FaslError const error_code, Box a, Box b) {
  // TODO(yosi) 2012-02-14 How do we specify file name to FastLoader?
  SourceInfo source_info;
  reader_.Error(FaslErrorInfo(source_info, error_code, a, b));
}

FastLoader::ObjectArray& FastLoader::ExpectArray(const Object* const object) {
  if (!object) {
    Error(FaslError_Expect_Array, object);
    return *new(zone_) ObjectArray(0);
  }

  if (auto const array = object->DynamicCast<ObjectArray>()) {
    return *array;
  }

  Error(FaslError_Expect_Array, object);
  return *new(zone_) ObjectArray(0);
}

const Class& FastLoader::ExpectClass(const Object* const object) {
  if (auto const clazz = object->DynamicCast<Class>()) {
    return *clazz;
  }
  Error(FaslError_Expect_Class, object);
  return *Ty_Void;
}

const ClassOrNamespace& FastLoader::ExpectClassOrNamespace(
    const Object* const object) {
  if (!object) {
    return global_ns_;
  }

  if (auto const clazz = object->DynamicCast<Class>()) {
    return *clazz;
  }

  if (auto const ns = object->DynamicCast<Namespace>()) {
    return *ns;
  }

  Error(FaslError_Expect_Class, object);
  return global_ns_;
}

const FunctionType& FastLoader::ExpectFunctionType(
    const Object* const object) {
  if (auto const type = object->DynamicCast<FunctionType>()) {
    return *type;
  }
  Error(FaslError_Expect_FunctionType, object);
  return FunctionType::Intern(*Ty_Void, ValuesType::Intern());
}

const GenericClass& FastLoader::ExpectGenericClass(
        const Object* const object) {
  if (auto const clazz = object->DynamicCast<GenericClass>()) {
    return *clazz;
  }
  Error(FaslError_Expect_GenericClass, object);
  return *Ty_Array;
}

const GenericMethod& FastLoader::ExpectGenericMethod(
        const Object* const object) {
  if (auto const method = object->DynamicCast<GenericMethod>()) {
    return *method;
  }

  Error(FaslError_Expect_GenericMethod, object);

  return *Ty_Array->Find(Name::Intern("Create"))
            ->StaticCast<MethodGroup>()
                ->methods().GetFirst()
                    ->StaticCast<GenericMethod>();
}

int FastLoader::ExpectInt32(const Object* const object) {
  if (auto const i32 = object->DynamicCast<Int32>()) {
    return i32->value();
  }

  Error(FaslError_Expect_Int32, object);
  return 0;
}

const Method& FastLoader::ExpectMethod(const Object* const object) {
  if (auto const method_group = object->DynamicCast<Method>()) {
    return *method_group;
  }
  Error(FaslError_Expect_Method, object);
  return *Ty_Object->Find(Name::Intern("ToString"))
    ->StaticCast<MethodGroup>()
        ->Find(FunctionType::Intern(*Ty_String, ValuesType::Intern()));
}

const MethodGroup& FastLoader::ExpectMethodGroup(const Object* const object) {
  if (auto const method_group = object->DynamicCast<MethodGroup>()) {
    return *method_group;
  }
  Error(FaslError_Expect_MethodGroup, object);
  return *Ty_Object->Find(Name::Intern("ToString"))
      ->StaticCast<MethodGroup>();
}

const Name& FastLoader::ExpectName(const Object* const object) {
  if (auto const name = object->DynamicCast<Name>()) {
    return *name;
  }

  Error(FaslError_Expect_Name, object);
  return Name::Intern(".null.");
}

const Namespace& FastLoader::ExpectNamespace(const Object* const object) {
  if (!object) {
    return global_ns_;
  }

  if (auto const ns = object->DynamicCast<Namespace>()) {
    return *ns;
  }

  Error(FaslError_Expect_Namespace, object);
  return global_ns_;
}

const Type& FastLoader::ExpectType(const Object* const object) {
  if (auto const type = object->DynamicCast<Type>()) {
    return *type;
  }
  Error(FaslError_Expect_Type, object);
  return *Ty_Void;
}

const TypeParam& FastLoader::ExpectTypeParam(const Object* const object) {
  if (auto const typaram= object->DynamicCast<TypeParam>()) {
    return *typaram;
  }
  Error(FaslError_Expect_TypeParam, object);
  return *new TypeParam(Name::Intern(".error."));
}

const ValuesType& FastLoader::ExpectValuesType(const Object* const object) {
  if (auto const type = object->DynamicCast<ValuesType>()) {
    return *type;
  }
  Error(FaslError_Expect_ValuesType, object);
  return ValuesType::Intern();
}

// [F]
void FastLoader::Feed(const void* const pv, uint const size) {
  reader_.Feed(pv, size);
  if (reader_.HasError()) {
    Error(FaslError_Invalid_FaslOp);
  }
}

void FastLoader::Finish() {
  if (HasError()) {
    return;
  }

  if (!operand_stack_.IsEmpty()) {
    Error(FaslError_Invalid_Stack);
  }
}

// [H]
bool FastLoader::HasError() const {
  return reader_.HasError();
}

void FastLoader::HandleArray(int const count) {
  ASSERT(count >= 0);
  if (count) {
    ScopedArray_<Object*> objects(new Object*[count]);
    if (Pop(objects, count)) {
      auto& arr = *new(zone_) ObjectArray(objects, count);
      Push(arr);
    }
  } else {
    auto& arr = *new(zone_) ObjectArray(0);
    Push(arr);
  }
}

// [H][A]
void FastLoader::HandleArrayType() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& elty = ExpectType(objects[0]);
  auto const rank = ExpectInt32(objects[1]);
  if (HasError()) return;

  auto& arrty = ArrayType::Intern(elty, rank);
  PushAndRemember(arrty);
}

// [H][C]
// Stack:
//  [0] Ref(Outer)
//  [1] Modifiers
//  [2] Name
void FastLoader::HandleClass() {
  Object* objects[3];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& outer = ExpectClassOrNamespace(objects[0]);
  auto const modifiers = ExpectInt32(objects[1]);
  auto& name = ExpectName(objects[2]);
  if (HasError()) return;

  auto& clazz = *new Class(outer, modifiers, name);
  Remember(clazz);
}

// Stack
//  [0] Ref(Class)
void FastLoader::HandleClassContext() {
  Object* objects[1];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }
  auto& clazz = ExpectClass(objects[0]);
  if (HasError()) return;
  context_.class_ = &const_cast<Class&>(clazz);
  context_.method_group_ = nullptr;
  context_.property_ = nullptr;
}

// Stack:
//  [0] Ref(ClassOrNamespace)
//  [1] Name
void FastLoader::HandleClassRef() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }
  auto& outer = ExpectClassOrNamespace(objects[0]);
  auto& name = ExpectName(objects[1]);
  if (HasError()) return;

  if (auto const present = outer.Find(name)) {
    if (auto const clazz = present->DynamicCast<Class>()) {
      PushAndRemember(*clazz);
      return;
    }
  }

  Error(FaslError_NotFound_Class, outer, name);
  return;
}

// Stack:
//  [0] Ref(GenericClass)
//  [1] Array(Type)
void FastLoader::HandleConstructedClassRef() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }
  auto& gen_class = ExpectGenericClass(objects[0]);
  auto& types = ExpectArray(objects[1]);
  if (HasError()) return;

  TypeArgs type_args;
  ObjectArray::Enum enum_types(types);
  foreach (GenericClass::EnumTypeParam, type_params, gen_class) {
    if (enum_types.AtEnd()) {
      Error(FaslError_Invalid_ConstructedClass);
      return;
    }
    auto& type_param = *type_params.Get();
    auto& type_arg = ExpectType(enum_types.Get());
    if (HasError()) return;
    type_args.Add(type_param, type_arg);
    enum_types.Next();
  }

  if (!enum_types.AtEnd()) {
    Error(FaslError_Invalid_ConstructedClass);
    return;
  }

  auto& clazz = gen_class.Construct(type_args);
  PushAndRemember(clazz);
}

// Stack:
//  [0] Ref(GenericMethod)
//  [1] Array(Type)
void FastLoader::HandleConstructedMethodRef() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }
  auto& gen_method = ExpectGenericMethod(objects[0]);
  auto& types = ExpectArray(objects[1]);
  if (HasError()) return;

  TypeArgs type_args;
  ObjectArray::Enum enum_types(types);
  foreach (GenericMethod::EnumTypeParam, type_params, gen_method) {
    if (enum_types.AtEnd()) {
      Error(FaslError_Invalid_ConstructedClass);
      return;
    }
    auto& type_param = *type_params.Get();
    auto& type_arg = ExpectType(enum_types.Get());
    if (HasError()) return;
    type_args.Add(type_param, type_arg);
    enum_types.Next();
  }

  if (!enum_types.AtEnd()) {
    Error(FaslError_Invalid_ConstructedClass);
    return;
  }

  auto& method = gen_method.Construct(type_args);
  PushAndRemember(method);
}

// [H][E]
void FastLoader::HandleError(const FaslErrorInfo& error) {
  AddErrorInfo(error);
}

// [H][F]
// Stack:
//  [0] PosInt(Modifier)
//  [1] Ref(Type)
//  [2] Name
// Context:
//  class_
void FastLoader::HandleField(const SourceInfo& source_info) {
  Object* objects[3];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }
  auto const modifiers = ExpectInt32(objects[0]);
  auto& type = ExpectType(objects[1]);
  auto& name = ExpectName(objects[2]);
  if (HasError()) return;

  if (!context_.class_) {
    Error(FaslError_Context_Class);
    return;
  }

  auto& clazz = *context_.class_;
  if (auto const present = clazz.Find(name)) {
    Error(FaslError_Duplicate_Field, *present);
    return;
  }

  auto& field = *new Field(clazz, modifiers, type, name, &source_info);
  clazz.Add(name, field);
  Remember(field);
}

void FastLoader::HandleFileMagic() {
  Object* objects[1];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  if (objects[0] != &Name::Intern(FileMagic)) {
    Error(FaslError_Expect_FileMagic);
  }
}

// Load nested function. A method funciton is created by HandleMethod.
//
// Stack:
//  [0] Int32(Flavor)
//  [1] Name
//  [2] Ref(FunctionType)
// Context:
//  function_
void FastLoader::HandleFunction() {
  Object* objects[3];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto const flavor = static_cast<Function::Flavor>(ExpectInt32(objects[0]));
  const auto& name = ExpectName(objects[1]);
  auto& funty = ExpectFunctionType(objects[2]);
  if (HasError()) return;

  if (!context_.function_) {
    Error(FaslError_Invalid_Function, name);
    return;
  }

  auto& outer = *context_.function_;
  auto& module = outer.module();
  auto& fun = module.NewFunction(&outer, flavor, name);
  fun.SetFunctionType(funty);
  Remember(fun);
}

// Stack:
//  [0] Ref(Function)
//  [1] Int32(NumberOfBBlocks)
void FastLoader::HandleFunctionBody() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  if (!objects[0] || !objects[0]->Is<Function>()) {
    Error(FaslError_Invalid_Function, objects[0]);
    return;
  }

  auto& fun = *objects[0]->StaticCast<Function>();

  auto const bblock_count = ExpectInt32(objects[1]);
  if (HasError()) return;

  context_.bblocks_.Clear();
  for (int i = 0; i < bblock_count; i++) {
    auto& bblock = fun.module().NewBBlock();
    bblock.SetName(i + 1);
    context_.bblocks_.Add(&bblock);
    fun.AppendBBlock(&bblock);
  }

  context_.bblock_ = fun.GetEntryBB();
  context_.function_ = &fun;
  ResetOutput();
}

// Stack:
//  [0] Ref(ValuesType)
//  [1] Ref(Type)
void FastLoader::HandleFunctionType() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& paramsty = ExpectValuesType(objects[0]);
  auto& rety = ExpectType(objects[1]);
  if (HasError()) return;

  auto& funty = FunctionType::Intern(rety, paramsty);
  PushAndRemember(funty);
}

// [H][G]
// Stack:
//  [0] Ref(ClassOrNamespace)
//  [1] Modifiers
//  [2] Name
//  [3] Array(TypeParam)
void FastLoader::HandleGenericClass() {
  Object* objects[4];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& outer = ExpectClassOrNamespace(objects[0]);
  auto const modifiers = ExpectInt32(objects[1]);
  auto& name = ExpectName(objects[2]);
  auto& name_list = ExpectArray(objects[3]);
  if (HasError()) return;

  Array_<const TypeParam*> type_params(name_list.Count());
  auto num_type_params = 0;
  for (auto const name: name_list) {
    auto& typaram = ExpectTypeParam(name);
    if (HasError()) return;
    type_params.Set(num_type_params, &typaram);
    ++num_type_params;
  }

  auto& clazz = *new GenericClass(
      outer,
      modifiers,
      name,
      Collection_<const TypeParam*>(type_params));

  Remember(clazz);
}

// Stack:
//  [0] Modifiers
//  [1] Array(Name)
//  [2] FunctionType
// Context:
//  method_group_
void FastLoader::HandleGenericMethod() {
  Object* objects[3];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto const modifiers = ExpectInt32(objects[0]);
  auto& name_list = ExpectArray(objects[1]);
  auto& funty = ExpectFunctionType(objects[2]);
  if (HasError()) return;

  Array_<const TypeParam*> type_params(name_list.Count());
  auto num_type_params = 0;
  for (auto const name: name_list) {
    auto& typaram = ExpectTypeParam(name);
    if (HasError()) return;
    type_params.Set(num_type_params++, &typaram);
  }

  auto& module = *new Module();
  auto& method_group = *context_.method_group_;

  auto& method = *new GenericMethod(
      method_group,
      modifiers,
      funty,
      type_params);
  Remember(method);

  method_group.Add(method);

  auto& fn = module.NewFunction(
    nullptr,
    Function::Flavor_Named,
    method.method_group().name());

  if (method.IsStatic()) {
    fn.SetFunctionType(method.function_type());

  } else {
    ValuesTypeBuilder builder;
    builder.Append(method.owner_class());
    foreach (ValuesType::Enum, types, method.params_type()) {
      builder.Append(*types);
    }

    fn.SetFunctionType(
      FunctionType::Intern(method.return_type(), builder.GetValuesType()));
  }

  method.SetFunction(fn);
  DEBUG_FORMAT("Method function %s", fn);

  context_.function_ = &fn;
  Remember(fn);
}

// [H][I]
// Construction an instruction from
//  Opcode
//  Output Type (on strack)
//  Operands (on stack as Array)
void FastLoader::HandleInstruction(
    Op const opcode,
    const SourceInfo& source_info) {
  if (opcode < 0 || opcode >= Op_Limit) {
    DEBUG_PRINTF("Invalid opcode %d\n", opcode);
    Error(FaslError_Invalid_Instruction);
    return;
  }

  if (!context_.bblock_) {
    DEBUG_PRINTF("No bblock\n");
    Error(FaslError_Invalid_BBlock);
    return;
  }

  if (!context_.function_) {
    DEBUG_PRINTF("No owner function\n");
    Error(FaslError_Invalid_Function);
    return;
  }

  auto& bblock = *context_.bblock_;

  if (bblock.name() > context_.bblocks_.Count()) {
    Error(FaslError_Invalid_Instruction);
    return;
  }

  if (context_.bblocks_.Get(bblock.name() - 1) != &bblock) {
    Error(FaslError_Invalid_Instruction);
    return;
  }

  if (bblock.GetFunction() != context_.function_) {
    Error(FaslError_Invalid_Instruction);
    return;
  }

  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& outy = ExpectType(objects[0]);
  if (HasError()) return;

  const auto& operands = ExpectArray(objects[1]);
  if (HasError()) return;

  auto& fun = *context_.function_;
  auto& zone = fun.module().zone();
  auto& inst = Instruction::New(zone, opcode);
  inst.set_output_type(outy);
  inst.SetSourceInfo(&source_info);

  if (inst.opcode() == Op_OpenCatch) {
    auto& frame_reg = *new(fun.module().zone()) FrameReg(
        context_.frame_reg_,
        FrameReg::Kind_Catch);
    inst.set_output(frame_reg);
    output_list_.Add(&frame_reg);
    frame_reg.set_name(output_list_.Count());
    context_.frame_reg_ = &frame_reg;

  } else if (inst.opcode() == Op_OpenFinally) {
    auto& frame_reg = *new(fun.module().zone()) FrameReg(
        context_.frame_reg_,
        FrameReg::Kind_Finally);
    inst.set_output(frame_reg);
    output_list_.Add(&frame_reg);
    frame_reg.set_name(output_list_.Count());
    context_.frame_reg_ = &frame_reg;

  } else if (outy != *Ty_Void) {
    auto& out = fun.module().NewOutput(outy);
    inst.set_output(out);
    output_list_.Add(&out);
    out.set_name(output_list_.Count());
  }

  {
    enum State {
      State_Phi_BBlock,
      State_Phi_Operand,
      State_Operand,
      State_Switch_BBlock,
      State_Switch_Operand,
    };

    State state = inst.Is<PhiI>()
        ? State_Phi_BBlock
            : inst.Is<SwitchI>()
                ? State_Switch_BBlock : State_Operand;

    Label* label = nullptr;

    for (auto const thing: operands) {
      auto const operand = thing->DynamicCast<Operand>();
      if (!operand) {
        Error(FaslError_Expect_Operand, thing);
        return;
      }

      switch (state) {
        case State_Phi_BBlock:
          label = operand->DynamicCast<Label>();
          if (!label) {
            Error(FaslError_Expect_Label, operand);
            return;
          }
          state = State_Phi_Operand;
          break;

        case State_Phi_Operand:
          static_cast<PhiI&>(inst).AddOperand(label->GetBB(), operand);
          state = State_Phi_BBlock;
          break;

        case State_Operand:
          inst.AppendOperand(operand);
          break;

        case State_Switch_BBlock:
          label = operand->DynamicCast<Label>();
          if (!label) {
            Error(FaslError_Expect_Label, operand);
            return;
          }
          state = State_Switch_Operand;
          break;

        case State_Switch_Operand:
          static_cast<SwitchI&>(inst).AddOperand(label->GetBB(), operand);
          state = State_Switch_BBlock;
          break;

        default:
          CAN_NOT_HAPPEN();
      }
    }

    if (state == State_Phi_Operand) {
      Error(FaslError_Invalid_PhiI, inst);
      return;
    }

    if (state == State_Switch_Operand) {
      Error(FaslError_Invalid_SwitchI, inst);
      return;
    }
  }

  bblock.AppendI(inst);

  if (inst.opcode() == Op_Exit) {
    context_.bblocks_.Clear();
    context_.bblock_ = nullptr;
    context_.frame_reg_ = nullptr;
    context_.function_ = nullptr;
    VerifyTask verifier(session(), fun.module());
    verifier.VerifyFunction(fun);

  } else if (inst.Is<LastInstruction>()) {
    context_.bblock_ = context_.bblock_->GetNext();
  }

  DEBUG_FORMAT("%s", inst);
}

void FastLoader::HandleInt(int64 const i64) {
  if (i64 < Int32::MinValue || i64 > Int32::MaxValue) {
    Push(*new(zone_) Int64(i64));
  } else {
    Push(*new(zone_) Int32(static_cast<int32>(i64)));
  }
}

// [H][L]
void FastLoader::HandleLabel(int const bblock_name) {
  if (bblock_name <= 0 || bblock_name > context_.bblocks_.Count()) {
    Error(FaslError_Invalid_Label, bblock_name);
    return;
  }

  const auto& bblock = *context_.bblocks_.Get(bblock_name - 1);
  Push(bblock.label());
}

void FastLoader::HandleLiteral(const Literal& lit) {
  DEBUG_FORMAT("%s", lit);
  Push(const_cast<Literal&>(lit));
}

// [H][M]
// Stack:
//  [0] Ref(MethodGroup)
//  [1] PosInt(Modifiers)
//  [2] Ref(FunType)
// Context:
//  class_
void FastLoader::HandleMethod() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto const modifiers = ExpectInt32(objects[0]);
  auto& funty = ExpectFunctionType(objects[1]);
  if (HasError()) return;

  if (!context_.class_) {
    Error(FaslError_Context_Class);
    return;
  }

  auto& module = *new Module();
  auto& method_group = *context_.method_group_;

  if (auto const present = method_group.Find(funty)) {
    Error(FaslError_Duplicate_Method, *present);
    return;
  }

  auto& method = *new Method(
      method_group,
      modifiers,
      funty);
  Remember(method);

  method_group.Add(method);

  auto& fn = module.NewFunction(
    nullptr,
    Function::Flavor_Named,
    method.method_group().name());

  fn.SetMethod(method);

  // TODO(yosi) 2012-03-25 We should set actual function type instead of
  // method function type for ease of optimization.
  if (method.IsStatic()) {
    fn.SetFunctionType(method.function_type());

  } else {
    ValuesTypeBuilder builder;
    builder.Append(method.owner_class());
    foreach (ValuesType::Enum, types, method.params_type()) {
      builder.Append(*types);
    }

    fn.SetFunctionType(
      FunctionType::Intern(method.return_type(), builder.GetValuesType()));
  }

  method.SetFunction(fn);
  DEBUG_FORMAT("Method function %s", fn);

  context_.function_ = &fn;
  Remember(fn);
}

// Stack:
//  [0] Name
// Context:
//  class_ an owner class of method group.
void FastLoader::HandleMethodGroup() {
  Object* objects[1];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& name = ExpectName(objects[0]);
  if (HasError()) return;

  if (!context_.class_) {
    Error(FaslError_Context_Class);
    return;
  }

  if (context_.class_->Find(name)) {
    Error(FaslError_Duplicate_MethodGroup, name);
    return;
  }

  auto& method_group = *new MethodGroup(*context_.class_, name);
  context_.class_->Add(name, method_group);
  Remember(method_group);
  context_.method_group_ = &method_group;
}

// Stack:
//  [0] Ref(Class)
//  [1] Ref(FunctionType)
//  [2] Name
void FastLoader::HandleMethodRef() {
  Object* objects[3];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& owner_class = ExpectClass(objects[0]);
  auto& funty =ExpectFunctionType(objects[1]);
  auto& name = ExpectName(objects[2]);
  if (HasError()) return;

  auto const present = owner_class.Find(name);
  if (!present) {
    Error(FaslError_NotFound_MethodGroup, owner_class, name);
    return;
  }

  auto const mtg = present->DynamicCast<MethodGroup>();
  if (!mtg) {
    Error(FaslError_Expect_MethodGroup, owner_class, present);
    return;
  }

  auto const mt = mtg->Find(funty);
  if (!mt) {
    Error(FaslError_NotFound_Method, mtg, funty);
    return;
  }

  PushAndRemember(*mt);
}

// [H][N]
void FastLoader::HandleName(const char16* const chars) {
  ASSERT(chars != nullptr);
  ASSERT(*chars != 0);
  auto& name = Name::Intern(chars);
  Push(name);
  operand_list_.Add(const_cast<Name*>(&name));
  DEBUG_FORMAT("[%d]=Name |%s|", operand_list_.Count(), name);
}

void FastLoader::HandleNamespace() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& outer_ns = ExpectNamespace(objects[0]);
  auto& name = ExpectName(objects[1]);
  if (HasError()) return;

  if (auto const present = outer_ns.Find(name)) {
    PushAndRemember(ExpectNamespace(present));
    return;
  }

  if (outer_ns == global_ns_) {
    if (auto const present = Namespace::Global->Find(name)) {
        PushAndRemember(ExpectNamespace(present));
        return;
    }
  }

  DEBUG_FORMAT("New namespace %s.%s", outer_ns, name);
  PushAndRemember(*new Namespace(outer_ns, name));
}

void FastLoader::HandleOutput(int const name) {
  if (name <= 0 || name > output_list_.Count()) {
    Error(FaslError_Invalid_Output, name);
    return;
  }
  Push(*output_list_.Get(name - 1));
}

// [H][P]
void FastLoader::HandlePatchPhi() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto const phi_output = objects[0]->DynamicCast<SsaOutput>();
  if (!phi_output) {
    Error(FaslError_Expect_SsaOutput);
    return;
  }

  auto const phi_inst = phi_output->GetDefI()->DynamicCast<PhiI>();
  if (!phi_inst) {
    Error(FaslError_Expect_PhiI);
    return;
  }

  auto const r1 = objects[1]->DynamicCast<SsaOutput>();
  if (!r1) {
    Error(FaslError_Expect_SsaOutput);
    return;
  }

  DEBUG_FORMAT("Add (%s %s) to %s", r1->GetDefI()->GetBB(), r1, phi_inst);
  phi_inst->AddOperand(r1->GetDefI()->GetBB(), r1);
}

void FastLoader::HandlePointerType() {
  Object* objects[1];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }
  auto& elty = ExpectType(objects[0]);
  if (HasError()) return;
  auto& ptrty = PointerType::Intern(elty);
  PushAndRemember(ptrty);
}

// Stack:
//  [0] Modifiers
//  [1] Ref(Type)
//  [2] Name
// Context:
//  class_  an owner of property
void FastLoader::HandleProperty() {
  Object* objects[3];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }
  auto const modifiers = ExpectInt32(objects[0]);
  auto& type = ExpectType(objects[1]);
  auto& name = ExpectName(objects[2]);
  if (HasError()) return;

  if (!context_.class_) {
    Error(FaslError_Context_Class);
    return;
  }

  auto& property = *new Property(*context_.class_, modifiers, type, name);
  context_.class_->Add(name, property);
  context_.property_ = &property;
}

// Stack:
//  [0] Name
//  [1] Ref(Method)
// Context:
//  property_  an owner of property member
void FastLoader::HandlePropertyMember() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& name = ExpectName(objects[0]);
  auto& method = ExpectMethod(objects[1]);
  if (HasError()) return;

  if (!context_.property_) {
    Error(FaslError_Context_Property);
    return;
  }

  auto& property = *context_.property_;

  if ((property.modifiers() ^ method.modifiers()) != Modifier_SpecialName) {
    Error(FaslError_Invalid_Property);
    return;
  }

  property.Add(name, method);
}

void FastLoader::HandleRealizeClass() {
  Object* objects[2];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }

  auto& clazz = ExpectClass(objects[0]);
  auto& thing_list = ExpectArray(objects[1]);
  if (HasError()) return;

  ArrayList_<const Class*> class_list;
  for (auto const thing: thing_list) {
    class_list.Add(&ExpectClass(thing));
    if (HasError()) return;
  }

  const_cast<Class&>(clazz).RealizeClass(class_list);
  context_.class_ = &const_cast<Class&>(clazz);
  if (callback_) {
    callback_->HandleClass(const_cast<Class&>(clazz));
  }
}

// Stack:
//  [0] TypeParam
//  [1] Array(Class)
//  [2] newable
void FastLoader::HandleRealizeTypeParam() {
  Object* objects[3];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }
  if (HasError()) return;

  auto& typaram = ExpectTypeParam(objects[0]);
  auto& things = ExpectArray(objects[1]);
  auto const newable = ExpectInt32(objects[2]);

  ArrayList_<const Class*> constraints(things.Count());
  auto index = 0;
  for (auto const thing: things) {
    constraints[index] = &ExpectClass(thing);
    ++index;
  }
  if (HasError()) return;

  const_cast<TypeParam&>(typaram).RealizeTypeParam(
      constraints,
      newable ? TypeParam::Newable : TypeParam::NotNewable);
}

void FastLoader::HandleRef(int const ref_id) {
  PushRef(ref_id);
}

void FastLoader::HandleReset() {
  ResetAll();
}

// [H][T]

// Stack:
//  [0] name
void FastLoader::HandleTypeParam() {
  Object* objects[1];
  if (!Pop(objects, ARRAYSIZE(objects))) {
    return;
  }
  if (HasError()) return;

  auto& name = ExpectName(objects[0]);
  if (HasError()) return;
  auto& typaram = *new TypeParam(name);
  PushAndRemember(typaram);
}

// [H][V]
void FastLoader::HandleValuesType() {
  Object* objects[1];
  if (!Pop(objects, ARRAYSIZE(objects))) return;

  auto& array = ExpectArray(objects[0]);
  if (HasError()) return;

  ValuesTypeBuilder builder(array.Count());
  for (auto const elem: array) {
    auto& type = ExpectType(elem);
    builder.Append(type);
  }
  PushAndRemember(builder.GetValuesType());
}

void FastLoader::HandleVariable() {
  if (!context_.function_) {
    Error(FaslError_Invalid_Variable);
    return;
  }

  Object* objects[1];
  if (!Pop(objects, ARRAYSIZE(objects))) return;

  auto& name = ExpectName(objects[0]);
  if (HasError()) return;

  auto& zone = context_.function_->module().zone();
  auto& var = *new(zone) Variable(name);
  Remember(var);
}

// [H][Z]
void FastLoader::HandleZero() {
  operand_stack_.Push(nullptr);
}

// [P]
bool FastLoader::Pop(Object** const objects, int const count) {
  ASSERT(count > 0);
  ASSERT(objects != nullptr);
  auto p = objects + count;
  while (!operand_stack_.IsEmpty()) {
    if (p == objects) {
      break;
    }
    --p;
    *p = const_cast<Object*>(operand_stack_.Pop());
  }

  if (p != objects) {
    Error(FaslError_Expect_Operand);
    return false;
  }

  return true;
}

void FastLoader::Push(const Object& obj) {
  DEBUG_FORMAT("Stack[%d]=%s", operand_stack_.Count(), obj);
  operand_stack_.Push(&const_cast<Object&>(obj));
}

void FastLoader::PushAndRemember(const Operand& operand) {
  Push(operand);
  Remember(operand);
}

void FastLoader::PushRef(int const ref_id) {
  if (ref_id <= 0 || ref_id > operand_list_.Count()) {
    Error(FaslError_Invalid_Reference, ref_id);
    return;
  }
  auto& obj = *operand_list_.Get(ref_id - 1);
  DEBUG_FORMAT("[%d] %s %s", ref_id, obj.GetKind(), obj);
  Push(obj);
}

// [R]
void FastLoader::Remember(const Operand& obj) {
  operand_list_.Add(&obj);
  #ifndef NDEBUG
    auto const ref_id = operand_list_.Count();
    DEBUG_FORMAT("[%d]=%hs %ls", ref_id, obj.GetKind(), obj);
  #endif
}

void FastLoader::ResetAll() {
  if (!operand_stack_.IsEmpty()) {
    Error(FaslError_Invalid_Stack);
    return;
  }

  operand_list_.Clear();

  ResetOutput();

  // List of remembered object must be as same as FastWriter.
  Remember(*Ty_Bool);
  Remember(*Ty_Int32);
  Remember(*Ty_Object);
  Remember(*Ty_String);
  Remember(*Ty_Void);
  Remember(*Void);
  context_.Reset();
}

void FastLoader::ResetOutput() {
  output_list_.Clear();

  // List of special outputs
  ASSERT(False->name() == 1);
  ASSERT(True->name() == 2);
  ASSERT(Void->name() == 3);
  output_list_.Add(False);
  output_list_.Add(True);
  output_list_.Add(Void);
}

} // Fasl
} // Il
