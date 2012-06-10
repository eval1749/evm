#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Fasl - Fast Writer
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FastWriter.h"

#include "./FaslErrorInfo.h"
#include "./Name64.h"
#include "./TypeSorter.h"

#include "../Ir.h"
#include "../../Common/Io/Stream.h"

namespace Il {
namespace Fasl {

using Common::Io::Stream;

namespace {

class Static {
  public: static bool IsName64(const Name& name) {
    foreach (String::EnumChar, chars, name) {
      if (Name64::FromChar(*chars) == 0) {
        return false;
      }
    }
    return true;
  }
}; // Static

static const char* const k_fasl_op_names[] = {
   #define DEFOP(mp_name) # mp_name,
   #include "./FaslOp.inc"
};

} // namespace

FastWriter::FastWriter(
    Session& session, Stream&
    stream,
    Namespace& global_ns)
    : Tasklet("FastWriter", session),
      bit_writer_(stream),
      global_ns_(global_ns),
      name_map_(zone_),
      operand_counter_(0),
      operand_map_(zone_),
      source_info_(nullptr) {
  // List of remembered object must be as same as FastReader.
  Remember(Ty_Bool); // 1
  Remember(Ty_Int32); // 2
  Remember(Ty_Object); // 3
  Remember(Ty_String); // 4
  Remember(Ty_Void); // 5
  Remember(Void); // 6
  WriteName(Name::Intern(FileMagic));
  WriteFaslOp(FaslOp_FileMagic);
}

FastWriter::~FastWriter() {
  bit_writer_.Flush();
}

// [A]
void FastWriter::Add(const Type& type) {
  type_set_.Add(&type);
}

// [F]
void FastWriter::Finish() {
  ScopedPtr_<Type::Array> types(TypeSorter::Sort(type_set_));

  // Write type.
  foreach (Type::Array::Enum, tys, types) {
    auto const type = *tys;
    if (auto const clazz = type->DynamicCast<Class>()) {
      WriteClassDef(*clazz);
    } else {
      WriteTypeRef(*type);
    }
  }

  foreach (Type::Array::Enum, tys, types) {
    auto const type = *tys;
    if (auto const clazz = type->DynamicCast<Class>()) {
      WriteTypeRef(*clazz);
      // Write base specifications as Array.
      auto num_base_specs = 0;
      foreach (Class::EnumBaseSpec, base_specs, *clazz) {
        WriteTypeRef(*base_specs);
        ++num_base_specs;
      }
      WriteArrayFaslOp(num_base_specs);
      WriteFaslOp(FaslOp_RealizeClass);
    }
  }

  // Write fields, methods, method groups, and properties.
  ArrayList_<const Method*> method_list;
  foreach (Type::Array::Enum, tys, types) {
    auto const ty = *tys;
    if (auto const clazz = ty->DynamicCast<Class>()) {
      WriteTypeRef(*clazz);
      WriteFaslOp(FaslOp_ClassContext);
      foreach (Class::EnumField, fields, clazz) {
        const auto& field = *fields;
        WriteModifiers(field.modifiers());
        WriteTypeRef(field.storage_type());
        WriteName(field.name());
        WriteFaslOp(FaslOp_Field);
        Remember(field);
      }

      // All methods must be emitted before properties for using property
      // reference in property member registation.
      foreach (Class::EnumMember, members, *clazz) {
        auto& member = *members;
        if (auto const method_group = member.DynamicCast<MethodGroup>()) {
          WriteName(method_group->name());
          WriteFaslOp(FaslOp_MethodGroup);
          Remember(*method_group);

          foreach (MethodGroup::EnumMethod, methods, method_group) {
            const auto& method = *methods;
            method_list.Add(&method);
            WriteMethod(method);
          }
        }
      }

      foreach (Class::EnumMember, members, *clazz) {
        auto& member = *members;
        if (auto const property = member.DynamicCast<Property>()) {
          WriteProperty(*property);
        }
      }
    }
  }

  foreach (ArrayList_<const Method*>::Enum, methods, method_list) {
    const auto& method = **methods;
    if (method.GetFunction()) {
      foreach (Module::EnumFunction, funs, method.module()) {
        WriteFunctionBody(*funs);
      }
    }
  }

  WriteFaslOp(FaslOp_Reset);

  DEBUG_PRINTF("TotalBits=%d"
      " Op_Limit=%d FaslOp_Limit=%d"
      " Name=%d Operand=%d\n",
      bit_writer_.total_bits(),
      Op_Limit,
      FaslOp_Limit,
      operand_counter_,
      operand_counter_);

  bit_writer_.Flush();
}

// [G]
int FastWriter::GetRefId(const Operand& operand) const {
  return operand_map_.Get(&operand);
}

// [I]
bool FastWriter::IsWritten(const Operand& operand) const {
  return operand_map_.Contains(&operand);
}

SsaOutput* FastWriter::NeedPhiOperandPatch(const Output& output) const {
  if (auto const r1 = output.DynamicCast<SsaOutput>()) {
    return phi_operand_set_.Contains(r1) ? r1 : nullptr;
  }
  return nullptr;
}

// Fast Loader needs to patch Phi instruction if phi operand is ssa-output
// and its defined after current basic block, when FastLoader load Phi
// instruction.
SsaOutput* FastWriter::NeedPhiOperandPatch(
    const PhiOperandBox& phi_box) const {
  if (auto const r1 = phi_box.GetOperand()->DynamicCast<SsaOutput>()) {
    return phi_box.GetI()->GetBB()->name() < r1->GetDefI()->GetBB()->name()
        ? r1 : nullptr;
  }

  return nullptr;
}

// [R]
int FastWriter::Remember(const Operand& operand) {
  ASSERT(operand != global_ns_);
  ASSERT(!IsWritten(operand));

  operand_counter_++;
  operand_map_.Add(&const_cast<Operand&>(operand), operand_counter_);

  DEBUG_FORMAT("[%d]=%s %s", operand_counter_, operand.GetKind(), operand);

  return operand_counter_;
}

// [U]
void FastWriter::UnexpectedOperand(const Operand& operand) {
  DEBUG_FORMAT("%s %s", operand.GetKind(), operand);
  AddErrorInfo(
    FaslErrorInfo(
        source_info_ ? *source_info_ : SourceInfo(),
        FaslError_Unexpected_Operand,
        operand));
}

// [W]
void FastWriter::WriteArrayFaslOp(int const count) {
  ASSERT(count >= 0);
  WriteFaslOp(FaslOp_Array);
  WriteUInt32V4(count);
}

void FastWriter::WriteBBlock(const BBlock& bblock) {
  ASSERT(bblock.GetLastI()->Is<LastInstruction>());
  ASSERT(bblock.GetLastI()->GetOutput() == Void);
  ASSERT(&bblock.GetLastI()->output_type() == Ty_Void);
  foreach (BBlock::EnumI, insts, bblock) {
    const auto& inst = *insts;
    WriteInstruction(inst);
  }
}

// Class
//  1 Outer
//  2 Modifiers
//  3 Name
//  4 Class[]
void FastWriter::WriteClassDef(const Class& clazz) {
  ASSERT(!IsWritten(clazz));

  WriteClassOrNamespaceRef(clazz.outer());
  WriteModifiers(clazz.modifiers());
  WriteName(clazz.name());

  // Write type parameters as Array if GenericClass.
  if (auto const gen_class = clazz.DynamicCast<GenericClass>()) {
    auto num_type_params = 0;
    foreach (GenericClass::EnumTypeParam, type_params, *gen_class) {
      WriteTypeParam(*type_params.Get());
      ++num_type_params;
    }
    WriteArrayFaslOp(num_type_params);
    WriteFaslOp(FaslOp_GenericClass);

  } else {
    WriteFaslOp(FaslOp_Class);
  }

  Remember(&clazz);
}

void FastWriter::WriteClassOrNamespaceRef(const ClassOrNamespace& thing) {
  if (auto const ns = thing.DynamicCast<Namespace>()) {
     WriteNamespaceRef(*ns);
  } else if (auto const clazz = thing.DynamicCast<Class>()) {
     WriteTypeRef(*clazz);
  } else {
    UnexpectedOperand(thing);
  }
}

void FastWriter::WriteFaslOp(FaslOp const fasl_op) {
  WriteUInt32V5(fasl_op);

  DEBUG_PRINTF("%d: FaslOp %hs\n",
    bit_writer_.total_bits(),
    fasl_op < 0 || fasl_op >= Op_Limit
        ? "*invalid*"
        : k_fasl_op_names[fasl_op]);
}

void FastWriter::WriteFaslOpV8(FaslOp const fasl_op, uint const operand) {
  DEBUG_PRINTF("FaslOp %hs %u\n",
    fasl_op < 0 || fasl_op >= Op_Limit
        ? "*invalid*"
        : k_fasl_op_names[fasl_op],
    operand);
  WriteUInt32V5(fasl_op);
  WriteUInt32V8(operand);
}

void FastWriter::WriteFunctionBody(const Function& fun) {
  WriteRef(fun);

  // Renumber bblocks and outputs
  ASSERT(False->name() == 1);
  ASSERT(True->name() == 2);
  ASSERT(Void->name() == 3);
  {
    int num_bblocks = 0;
    int num_outputs = 3;
    foreach (Function::EnumBBlockPostorderReverse,  bblocks, fun) {
      auto& bblock = *bblocks;
      if (&bblock == fun.GetExitBB()) {
        continue;
      }
      ++num_bblocks;
      bblock.SetName(num_bblocks);
      foreach (BBlock::EnumI, insts, bblock) {
        auto& inst = *insts;
        if (auto const out = inst.GetOutput()) {
          if (out != Void) {
            ++num_outputs;
            out->set_name(num_outputs);
          }
        }
      }
    }

    ++num_bblocks;
    fun.GetExitBB()->SetName(num_bblocks);

    ASSERT(fun.GetEntryBB()->name() == 1);
    ASSERT(fun.GetStartBB()->name() == 2);
    ASSERT(fun.GetExitBB()->name() == num_bblocks);

    DEBUG_FORMAT("%s has %d bblocks and %d outputs.",
        fun, num_bblocks, num_outputs);
    WritePushInt32(num_bblocks);
  }

  ASSERT(phi_operand_set_.IsEmpty());

  WriteFaslOp(FaslOp_FunctionBody);

  foreach (Function::EnumBBlockPostorderReverse,  bblocks, fun) {
    const auto& bblock = *bblocks;
    if (&bblock != fun.GetExitBB()) {
      WriteBBlock(bblock);
    }
  }

  WriteBBlock(*fun.GetExitBB());
}

// Stack:
//  [0] Int32(Flavor)
//  [1] Name
//  [2] Ref(FunctionType)
void FastWriter::WriteFunctionDef(const Function& fun) {
  ASSERT(fun.GetOuter() != nullptr);
  WritePushInt32(fun.GetFlavor());
  WriteName(fun.name());
  WriteTypeRef(fun.function_type());
  WriteFaslOp(FaslOp_Function);
  Remember(fun);
}

// [W][I]
bool FastWriter::WriteIfRemembered(const Operand& operand) {
  if (auto const ref_id = GetRefId(operand)) {
    WriteRefId(ref_id);
    return true;
  }
  return false;
}

// Instruction
//  1 Ref(type)
//  2 Array(operand...)
//  FaslOp(Instruction) V8(opcode)
void FastWriter::WriteInstruction(const Instruction& inst) {
  source_info_ = &inst.source_info();

  // Note: We don't write output number into FASL. We can compute it from
  // position of instruction.
  WriteTypeRef(inst.output_type());

  auto operand_count = 0;
  foreach (Instruction::EnumOperand, operands, inst) {
    auto const& operand = *operands;
    auto const& box = *operands.GetBox();

    if (auto const phi_box = box.DynamicCast<PhiOperandBox>()) {
      if (auto const r1 = NeedPhiOperandPatch(*phi_box)) {
        phi_operand_set_.Add(r1);
        continue;
      }

      ++operand_count;
      WriteLabel(phi_box->GetBB()->label());

    } else if (auto const sw_box = box.DynamicCast<SwitchOperandBox>()) {
      ++operand_count;
      WriteLabel(sw_box->GetBB()->label());
    }

    ++operand_count;
    WriteOperand(operand);
  }

  WriteArrayFaslOp(operand_count);
  WriteFaslOpV8(FaslOp_Instruction, inst.GetOp());
  DEBUG_FORMAT("%s", inst);

  // Emit PatchPhi if needed
  if (auto const r1 = NeedPhiOperandPatch(inst.output())) {
    phi_operand_set_.Remove(r1);
    foreach (SsaOutput::EnumUser, users, r1) {
      auto const phi_inst = users.Get()->GetI()->DynamicCast<PhiI>();
      if (!phi_inst) continue;
      if (phi_inst->bblock().name() < inst.bblock().name()) {
        DEBUG_FORMAT("Patch %s with %s", phi_inst->output(), *r1);
        WriteOutputRef(phi_inst->output());
        WriteOutputRef(*r1);
        WriteFaslOp(FaslOp_PatchPhi);
      }
    }
  }
}

void FastWriter::WriteLabel(const Label& label) {
  WriteFaslOp(FaslOp_Label);
  WriteUInt32V8(label.GetBB()->name());
}

void FastWriter::WriteLiteral(const Literal& lit) {
  if (auto const p = lit.DynamicCast<BooleanLiteral>()) {
    WriteFaslOp(p->value() ? FaslOp_True : FaslOp_False);

  } else if (auto const p = lit.DynamicCast<CharLiteral>()) {
    WriteFaslOpV8(FaslOp_Char, p->value());

  } else if (auto const p = lit.DynamicCast<Int16Literal>()) {
    WriteFaslOpV8(FaslOp_Int16, p->value());

  } else if (auto const p = lit.DynamicCast<Int32Literal>()) {
    WriteFaslOpV8(FaslOp_Int32, p->value());

  } else if (auto const p = lit.DynamicCast<Int64Literal>()) {
    WriteFaslOp(FaslOp_Int64);
    WriteUInt64V8(p->value());

  } else if (auto const p = lit.DynamicCast<Int8Literal>()) {
    WriteFaslOpV8(FaslOp_Int8, p->value());

  } else if (auto const p = lit.DynamicCast<UInt16Literal>()) {
    WriteFaslOpV8(FaslOp_UInt16, p->value());

  } else if (auto const p = lit.DynamicCast<UInt32Literal>()) {
    WriteFaslOpV8(FaslOp_UInt32, p->value());

  } else if (auto const p = lit.DynamicCast<UInt64Literal>()) {
    WriteFaslOp(FaslOp_UInt64);
    WriteUInt64V8(p->value());

  } else if (auto const p = lit.DynamicCast<UInt8Literal>()) {
    WriteFaslOpV8(FaslOp_UInt8, p->value());

  } else if (auto const p = lit.DynamicCast<StringLiteral>()) {
    const auto& str = p->value();
    WriteFaslOp(FaslOp_String);
    WriteUInt32V8(str.length());
    foreach (String::EnumChar, chars, str) {
      WriteUInt32V8(*chars);
    }
    DEBUG_FORMAT("String(%s).length=%d", str, str.length());

  } else {
    // TODO(yosi) NYI FastWriter float operand
    UnexpectedOperand(lit);
  }
}

// [W][M]
void FastWriter::WriteMethod(const Method& method) {
  WriteModifiers(method.modifiers());

  if (auto const gen_method = method.DynamicCast<GenericMethod>()) {
    auto num_type_params = 0;
    foreach (GenericMethod::EnumTypeParam, type_params, *gen_method) {
      WriteTypeParam(*type_params.Get());
      ++num_type_params;
    }
    WriteArrayFaslOp(num_type_params);
    WriteTypeRef(method.function_type());
    WriteFaslOp(FaslOp_GenericMethod);

  } else {
    WriteTypeRef(method.function_type());
    WriteFaslOp(FaslOp_Method);
  }
  Remember(method);

  if (method.GetFunction() == nullptr) {
    return;
  }

  const auto& method_function = *method.GetFunction();
  foreach (Module::EnumFunction, funs, method.module()) {
    const auto& fun = *funs;
    if (&method_function == &fun) {
      // To reduce size of FASL file, writer doesn't emit method function.
      Remember(fun);
    } else {
      WriteFunctionDef(fun);
    }

    // Varaibles for instruction operands
    foreach (Function::EnumVar, vars, fun) {
      const auto& var = *vars;
      WriteName(var.name());
      WriteFaslOp(FaslOp_Variable);
      Remember(var);
    }
  }
}

void FastWriter::WriteModifiers(const int modifiers) {
  WritePushInt32(modifiers);
}

// Name V8+ 0
// Name64 I6+ 0
void FastWriter::WriteName(const Name& name) {
  if (auto const ref_id = name_map_.Get(&name)) {
    WriteRefId(ref_id);
    return;
  }

  operand_counter_++;
  name_map_.Add(&name, operand_counter_);

  DEBUG_FORMAT("Name[%d] %s", operand_counter_, name);

  if (Static::IsName64(name)) {
    WriteFaslOp(FaslOp_Name64);
    foreach (String::EnumChar, chars, name) {
      bit_writer_.Write6(Name64::FromChar(*chars));
    }
    bit_writer_.Write6(0);
    return;
  }

  WriteFaslOp(FaslOp_Name);
  WriteUInt32V8(name.ToString().length());
  foreach (String::EnumChar, chars, name) {
    WriteUInt32V8(*chars);
  }
}

void FastWriter::WriteNamespaceRef(const Namespace& ns) {
  if (ns == global_ns_ || ns.IsGlobalNamespace()) {
    WriteFaslOp(FaslOp_Zero);
    return;
  }

  if (WriteIfRemembered(ns)) {
    return;
  }

  WriteNamespaceRef(*ns.outer().StaticCast<Namespace>());
  WriteName(ns.name());
  WriteFaslOp(FaslOp_Namespace);
  Remember(ns);
}

// [W][O]
void FastWriter::WriteOperand(const Operand& operand) {
  if (auto const field = operand.DynamicCast<Field>()) {
    if (!WriteIfRemembered(*field)) {
      WriteTypeRef(field->owner_class());
      WriteName(field->name());
      WriteFaslOp(FaslOp_FieldRef);
      Remember(*field);
    }

  } else if (operand.Is<Function>()) {
    if (!WriteIfRemembered(operand)) {
      DEBUG_FORMAT("Must be remembered: %s", operand);
      UnexpectedOperand(operand);
    }

  } else if (auto const label = operand.DynamicCast<Label>()) {
    WriteLabel(*label);

  } else if (auto const lit = operand.DynamicCast<Literal>()) {
    WriteLiteral(*lit);

  } else if (auto const output = operand.DynamicCast<Output>()) {
    WriteOutputRef(*output);

  } else if (auto const method = operand.DynamicCast<ConstructedMethod>()) {
    if (!WriteIfRemembered(*method)) {
      WriteOperand(method->generic_method());
      auto num_type_args = 0;
      foreach (ConstructedMethod::EnumTypeArg, type_args, *method) {
        auto& type_arg = *type_args.Get().GetValue();
        WriteTypeRef(type_arg);
        ++num_type_args;
      }
      WriteArrayFaslOp(num_type_args);
      WriteFaslOp(FaslOp_ConstructedMethodRef);
      Remember(*method);
    }

  } else if (auto const method = operand.DynamicCast<Method>()) {
    if (!WriteIfRemembered(*method)) {
      WriteTypeRef(method->owner_class());
      WriteTypeRef(method->function_type());
      WriteName(method->name());
      WriteFaslOp(FaslOp_MethodRef);
      Remember(*method);
    }

  } else if (auto const type = operand.DynamicCast<Type>()) {
    WriteTypeRef(*type);

  } else {
    DEBUG_FORMAT("NYI: %s %s", operand.GetKind(), operand);
    UnexpectedOperand(operand);
  }
}

// Write output reference based on bit size of output name:
//  FaslOp_Reg1 [1,33]      5bit+5bit
//  FaslOp_Reg2 [34,66]     5bit+5bit
//  FaslOp_RegV >= 67       5bit+8bit+
void FastWriter::WriteOutputRef(const Output& output) {
  auto const name = output.name();
  if (name <= 33) {
    WriteFaslOp(FaslOp_Reg1);
    bit_writer_.Write5(name - 1);
    DEBUG_PRINTF("  Reg1=%d\n", name);

  } else if (name <= 66) {
    WriteFaslOp(FaslOp_Reg2);
    bit_writer_.Write5(name - 34);
    DEBUG_PRINTF("  Reg2=%d\n", name);

  } else {
    WriteFaslOpV8(FaslOp_RegV, name - 67);
  }
}

// [W][P]

// [0] Modifier
// [1] Type
// [2] Name
// Property
// [0] Name -- a kind of method, e.g. "get", "set", or others.
// [1] Method
// PropertyMember
void FastWriter::WriteProperty(const Property& property) {
  WriteModifiers(property.modifiers());
  WriteTypeRef(property.property_type());
  WriteName(property.name());
  WriteFaslOp(FaslOp_Property);

  auto num_members = 0;
  foreach (Property::EnumMember, members, property) {
    auto entry = members.Get();
    WriteName(*entry.GetKey());
    WriteRef(*entry.GetValue());
    WriteFaslOp(FaslOp_PropertyMember);
    ++num_members;
  }

  if (!num_members) {
    // Property must have members.
    AddErrorInfo(
        FaslErrorInfo(
            property.source_info(),
            FaslError_Invalid_Property,
            property));
  }
}

void FastWriter::WritePushInt32(int32 const i32) {
  if (i32 >= 0) {
    WriteFaslOpV8(FaslOp_PosInt, i32);

  } else if (i32 == Int32::MinValue) {
    WriteFaslOpV8(FaslOp_PosInt, i32);

  } else {
    WriteFaslOpV8(FaslOp_NegInt, -i32);
  }
}

// [W][R]
void FastWriter::WriteRef(const Operand& operand) {
  if (!WriteIfRemembered(operand)) {
    UnexpectedOperand(operand);
  }
}

// Emits ref_id in four different forms depend on number of bits of ref_id:
//  FaslOp_Ref1 [0,15]   5bit + 4bit = 9bit
//  FaslOp_Ref2 [0,255]  5bit + 8bit = 13bit
//  FaslOp_Ref3 [0,4095] 5bit + 12bit = 17bit
//  FaslOp_RefV >= 4096  5bit + 4bit+
void FastWriter::WriteRefId(int const ref_id) {
  ASSERT(ref_id > 0);

  if (ref_id < 16) {
    // TODO(yosi) 2012-02-06 Because of we don't use ref_id=0, Ref1 can
    // cover 1 <= ref_id <= 16.
    WriteFaslOp(FaslOp_Ref1);
    bit_writer_.Write4(ref_id);
    DEBUG_PRINTF("Ref1=%d\n", ref_id);
    return;
  }

  if (ref_id < 16 * 16) {
    // TODO(yosi) 2012-02-06 We should write 8bit value one call rather than
    // two 4bit write.
    WriteFaslOp(FaslOp_Ref2);
    bit_writer_.Write4(ref_id >> 4);
    bit_writer_.Write4(ref_id >> 0);
    DEBUG_PRINTF("Ref2=%d\n", ref_id);
    return;
  }

  if (ref_id < 16 * 16 * 16) {
    WriteFaslOp(FaslOp_Ref3);
    bit_writer_.Write8(ref_id >> 8);
    bit_writer_.Write4(ref_id >> 0);
    DEBUG_PRINTF("Ref3=%d\n", ref_id);
    return;
  }

  WriteFaslOp(FaslOp_RefV);
  WriteUInt32V4(ref_id);
  DEBUG_PRINTF("RefV=%d\n", ref_id);
}

void FastWriter::WriteSourceInfo(const SourceInfo* source_info) {
  if (source_info == nullptr) {
      DEBUG_PRINTF("No source info\n");
      return;
  }

  if (!source_info->HasInfo()) {
      DEBUG_PRINTF("No source info\n");
      return;
  }

// TODO(yosi) 2012-02-06 Disabled until FastReader handles SourceInfo.
#if 0
  if (source_info_ == nullptr
      || source_info_->file_name() == source_info->file_name()) {

   DEBUG_FORMAT("Full Source Info: %s(%d,%d)",
       source_info->file_name(),
       source_info->line(),
       source_info->column());

   WriteName(Name::Intern(source_info->file_name()));
   WriteFaslOp(FaslOp_LineInfo);
   WriteUInt32V8(source_info->line());
   WriteUInt32V7(source_info->column());

   source_info_ = source_info;
   return;
  }

  source_info_ = source_info;

  auto const line_delta = source_info->line() - source_info_->line();
  auto const column_delta = source_info->column() - source_info_->column();

  if (line_delta > 0) {
      DEBUG_PRINTF("Line Delta %d\n", line_delta);
      WriteFaslOp(FaslOp_LineAdd);
      WriteUInt32V4(line_delta);
      WriteUInt32V7(source_info->column());
      return;
  }

  if (line_delta < 0) {
      DEBUG_PRINTF("Line Delta %d\n", line_delta);
      WriteFaslOp(FaslOp_LineSub);
      WriteUInt32V4(-line_delta);
      WriteUInt32V7(source_info->column());
      return;
  }

  ASSERT(line_delta == 0);

  DEBUG_PRINTF("Column Delta %d\n", column_delta);

  if (column_delta >= 0) {
   WriteFaslOp(FaslOp_LineColAdd);
   WriteUInt32V4(column_delta);

  } else {
   WriteFaslOp(FaslOp_LineColSub);
   WriteUInt32V4(-column_delta);
  }
#endif
}

// TypeParam object is serialized within two steps:
//  1. Serialize name for creating TypeParam object.
//  2. Serialzie constraints
// Two step serialization is required for supporting self reference, e.g.
//  class Foo<T> where T : Comparable<T> { ... }
void FastWriter::WriteTypeParam(const TypeParam& typaram) {
  if (!typaram.is_realized()) {
    UnexpectedOperand(typaram);
    return;
  }

  WriteName(typaram.name());
  WriteFaslOp(FaslOp_TypeParam);
  Remember(typaram);

  auto num_constraints = 0;
  foreach (TypeParam::EnumConstraint, it, typaram) {
    WriteTypeRef(**it);
    ++num_constraints;
  }

  WriteTypeRef(typaram);
  WriteArrayFaslOp(num_constraints);
  WritePushInt32(typaram.is_newable());
  WriteFaslOp(FaslOp_RealizeTypeParam);
}

void FastWriter::WriteTypeRef(const Type& type) {
  if (WriteIfRemembered(type)) {
    return;
  }

  if (auto const arrty = type.DynamicCast<ArrayType>()) {
    WriteTypeRef(arrty->element_type());
    WritePushInt32(arrty->rank());
    WriteFaslOp(FaslOp_ArrayType);
    Remember(type);
    return;
  }

  if (auto const clazz = type.DynamicCast<ConstructedClass>()) {
    // Ref(GenericClass)
    // Array(Type+)
    // ConstructedClassRef
    WriteTypeRef(clazz->generic_class());
    auto num_type_args = 0;
    foreach (ConstructedClass::EnumTypeArg, type_args, *clazz) {
      auto& type_arg = *type_args.Get().GetValue();
      WriteTypeRef(type_arg);
      ++num_type_args;
    }
    ASSERT(num_type_args > 0);
    WriteArrayFaslOp(num_type_args);
    WriteFaslOp(FaslOp_ConstructedClassRef);
    Remember(type);
    return;
  }

  if (auto const clazz = type.DynamicCast<Class>()) {
    // Ref(ClassOrNamespace)
    // Name
    // ClassRef
    WriteClassOrNamespaceRef(clazz->outer());
    WriteName(clazz->name());
    WriteFaslOp(FaslOp_ClassRef);
    Remember(type);
    return;
  }

  if (auto const funty = type.DynamicCast<FunctionType>()) {
    WriteTypeRef(funty->params_type());
    WriteTypeRef(funty->return_type());
    WriteFaslOp(FaslOp_FunctionType);
    Remember(type);
    return;
  }

  if (auto const ptrty = type.DynamicCast<PointerType>()) {
    WriteTypeRef(ptrty->pointee_type());
    WriteFaslOp(FaslOp_PointerType);
    Remember(type);
    return;
  }

  if (auto const valsty = type.DynamicCast<ValuesType>()) {
    foreach (ValuesType::Enum, elmtys, valsty) {
      WriteTypeRef(*elmtys);
    }
    WriteArrayFaslOp(valsty->Count());
    WriteFaslOp(FaslOp_ValuesType);
    Remember(type);
    return;
  }

  // Note: TypeParam is remembered in WriteClassDef or WriteMethodDef.
    UnexpectedOperand(type);
}

#define DEFINE_WRITE_VARINT(kBits) \
  void FastWriter::WriteUInt32V ## kBits( \
      uint32 const value, \
      uint32 const msb) { \
    auto const kMsb = 1 << (kBits - 1); \
    auto const kMask = kMsb - 1; \
    if (value >= kMsb) { \
      WriteUInt32V ## kBits(value >> (kBits - 1), kMsb); \
    } \
    bit_writer_.Write ## kBits((value & kMask) | msb); \
  }

DEFINE_WRITE_VARINT(4)
DEFINE_WRITE_VARINT(5)
DEFINE_WRITE_VARINT(6)
DEFINE_WRITE_VARINT(7)
DEFINE_WRITE_VARINT(8)

void FastWriter::WriteUInt64V8(uint64 const value, uint32 msb) {
  auto const kMsb = 1 << 7;
  auto const kMask = kMsb - 1;
  if (value >= kMsb) {
    WriteUInt64V8(value >> 7, kMsb);
  }
  bit_writer_.Write8(static_cast<uint>(value & kMask) | msb);
}

} // Fasl
} // iL
