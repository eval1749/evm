#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Module
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Module.h"

#include "./Operands.h"

namespace Il {
namespace Ir {

//ctor
Module::Module()
    : num_bblocks_(0),
      num_functions_(0),
      num_outputs_(0) {
  DEBUG_PRINTF("%p\n", this);
}

Module::~Module() {
  DEBUG_PRINTF("%p\n", this);
  LayoutList::DeleteAll();
}

// [C]
Module& Module::Clone(const TypeArgs& type_args) const {
  ScopedPtr_<Module> module(new Module());
  auto& zone = module->zone();

  HashMap_<const Function*, Function*> fn_map;

  for (auto& templ_fn: functions()) {
    auto& fn = module->NewFunction(
        nullptr,
        templ_fn.flavor(),
        templ_fn.name());
    fn_map.Add(&templ_fn, &fn);
    if (auto const method = templ_fn.GetMethod()) {
      fn.SetMethod(*method);
    }
  }

  for (auto& templ_fn: functions()) {
    auto& fn = *fn_map.Get(&templ_fn);
    HashMap_<const BBlock*, BBlock*> bb_map;
    foreach (Function::EnumBBlock, bblocks, templ_fn) {
      auto& templ_bb = *bblocks.Get();
      auto& bb = NewBBlock();
      fn.AppendBBlock(&bb);
      bb_map.Add(&templ_bb, &bb);
    }

    HashMap_<const Output*, const Output*> out_map;
    foreach (Function::EnumBBlock, bblocks, templ_fn) {
      auto& templ_bb = *bblocks.Get();
      auto& bb = *bb_map.Get(&templ_bb);
      foreach (BBlock::EnumI, insts, templ_bb) {
        auto& templ_inst = *insts.Get();
        auto& inst = Instruction::New(zone, templ_inst.GetOp());
        if (templ_inst.output_type() != *Ty_Void) {
          auto& outy = templ_inst.output_type().Construct(type_args);
          inst.set_output_type(outy);
          auto& out = module->NewOutput(outy);
          inst.set_output(out);
          out_map.Add(templ_inst.GetOutput(), &out);
        }

        foreach (Instruction::EnumOperand, operands, templ_inst) {
          auto& operand = *operands.Get();
          if (auto const templ = operand.DynamicCast<Function>()) {
            if (auto const present = fn_map.Get(templ)) {
              inst.AppendOperand(present);
            } else {
              inst.AppendOperand(&operand);
            }

          } else if (auto const templ_label = operand.DynamicCast<Label>()) {
            auto& bb = *bb_map.Get(templ_label->GetBB());
            inst.AppendOperand(bb.label());

          } else if (auto const templ_out = operand.DynamicCast<Output>()) {
            auto& out = *out_map.Get(templ_out);
            inst.AppendOperand(&out);

          } else if (auto const type_param =
                        operand.DynamicCast<TypeParam>()) {
            inst.AppendOperand(type_param->Construct(type_args));

          } else {
            inst.AppendOperand(&operand);
          }
        }

        bb.AppendI(inst);
      }
    }
  }

  return *module.Detach();
}

Module& Module::Clone() const {
  TypeArgs type_args;
  return Clone(type_args);
}

// [N]
BBlock& Module::NewBBlock() const {
  return *new(zone_) BBlock(++num_bblocks_);
}

const BoolOutput& Module::NewBoolOutput() const {
  auto& out = *new(zone_) BoolOutput();
  out.set_name(++num_outputs_);
  return out;
}

Function& Module::NewFunction(
    Function* const outer,
    Function::Flavor const flavor,
    const Name& name) {
  auto& fun = *new(zone_) Function(outer, flavor, name);
  fun.SetIndex(++num_functions_);
  LayoutList::Append(&fun);
  return fun;
}

const Literal& Module::NewLiteral(bool value) const {
  return NewLiteral(*Ty_Boolean, value);
}

const Literal& Module::NewLiteral(
    const Type& type,
    int64 const value) const {
  if (type == *Ty_Boolean) {
    BooleanLiteral temp(value != 0);
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) BooleanLiteral(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_Char) {
    CharLiteral temp(static_cast<char16>(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) CharLiteral(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_Float32) {
    Float32Literal temp(Float32Literal::FromUInt32(static_cast<uint32>(value)));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) Float32Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_Float64) {
    Float64Literal temp(Float64Literal::FromUInt64(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) Float64Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_Int16) {
    Int16Literal temp(static_cast<int16>(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) Int16Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_Int32) {
    Int32Literal temp(static_cast<int32>(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) Int32Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_Int64) {
    Int64Literal temp(static_cast<int64>(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) Int64Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_Int8) {
    Int8Literal temp(static_cast<int8>(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) Int8Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_UInt16) {
    UInt16Literal temp(static_cast<uint16>(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) UInt16Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_UInt32) {
    UInt32Literal temp(static_cast<uint32>(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) UInt32Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_UInt64) {
    UInt64Literal temp(static_cast<uint64>(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) UInt64Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  if (type == *Ty_UInt8) {
    UInt8Literal temp(static_cast<uint8>(value));
    if (auto const p = literal_set_.Get(&temp)) {
      return *p;
    }
    auto& lit = *new(zone_) UInt8Literal(temp.value());
    literal_set_.Add(&lit);
    return lit;
  }

  CAN_NOT_HAPPEN();
}

const Output& Module::NewOutput(const Type& ty) const {
  if (&ty == Ty_Bool) {
    return NewBoolOutput();
  }

  if (&ty == Ty_Float32 || &ty == Ty_Float64) {
    auto& out = *new(zone_) Float();
    out.set_name(++num_outputs_);
    return out;
  }

  if (ty.Is<ValuesType>()) {
    auto& out = *new(zone_) Values();
    out.set_name(++num_outputs_);
    return out;
  }

  if (&ty == Ty_Void) {
    return *Void;
  }

  return NewRegister();
}

const Register& Module::NewRegister() const {
  auto& reg = *new(zone_) Register();
  reg.set_name(++num_outputs_);
  return reg;
}

const Values& Module::NewValues() const {
  auto& reg = *new(zone_) Values();
  reg.set_name(++num_outputs_);
  return reg;
}

// [R]
void Module::RemoveFunction(Function& fun) {
  fun.MakeVoid();
  LayoutList::Delete(&fun);
}

} // Ir
} // Il
