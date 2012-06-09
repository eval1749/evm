#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Parsre - Operator
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Operator.h"

#include "../Ir/NamespaceDef.h"
#include "../../Il/Ir.h"

namespace Compiler {

namespace {
typedef HashMap_<const Name*, Operator*> OperatorTable;
static OperatorTable* s_OperatorTable;

#define DEFOPERATOR(mp_name, mp_str, mp_cat)

#define DEFOPERATOR_ARITH(mp_name, mp_str, mp_cat, mp_op) \
  static Instruction* MakeInstruction_ ## mp_op(const Module& module) { \
    return new mp_op( \
        Ty_Object, \
        &const_cast<Register&>(module.NewRegister()), \
        Void, \
        Void); \
  }

#define DEFOPERATOR_COMP(mp_name, mp_str, mp_cat, mp_op) \
  static Instruction* MakeInstruction_ ## mp_op(const Module& m) { \
    return new mp_op(m.NewBoolOutput(), *Void, *Void); \
  }

#include "./operators.inc"

static Instruction* MakeInstruction_Invalid(const Module&) {
    CAN_NOT_HAPPEN();
} // MakeInstruction_Invalid

} // namespace

Operator::Operator(
    Category eCategory,
    const Name& name,
    const Name& binary_operator_name,
    bool const is_bool_output,
    MakeInstructionFn const pMakeInstruction)
    : binop_name_(binary_operator_name),
      m_eCategory(eCategory),
      m_pCallee(new StringLiteral(name)),
      is_bool_output_(is_bool_output),
      name_(name),
      m_pMakeInstruction(pMakeInstruction) {
}

// [C]
int Operator::ComputeHashCode() const {
  return name_.ComputeHashCode();
}

// [F]
Operator* Operator::Find(const Name& name) {
  return s_OperatorTable->Get(&name);
}

// [I]
void Operator::Init() {
  #define DEFOPERATOR(mp_name, mp_str, mp_cat) \
      Op__ ## mp_name = new Operator( \
          Operator::Category_ ## mp_cat, \
          Name::Intern(mp_str), \
          Name::Intern(mp_str), \
          false, \
          MakeInstruction_Invalid); \
      s_OperatorTable->Add(Q_ ## mp_name, Op__ ## mp_name);

  #define DEFOPERATOR_ARITH(mp_name, mp_str, mp_cat, mp_op) \
      Op__ ## mp_name = new Operator( \
          Operator::Category_ ## mp_cat, \
          Name::Intern("operator " mp_str), \
          Name::Intern("operator " mp_str), \
          false, \
          MakeInstruction_ ## mp_op); \
      s_OperatorTable->Add(Q_ ## mp_name, Op__ ## mp_name);

  #define DEFOPERATOR_ASSIGN(mp_name, mp_str, mp_binop) \
      Op__ ## mp_name = new Operator( \
          Operator::Category_Assign, \
          Name::Intern(mp_str), \
          Name::Intern(mp_binop), \
          false, \
          MakeInstruction_Invalid); \
      s_OperatorTable->Add(Q_ ## mp_name, Op__ ## mp_name);

  #define DEFOPERATOR_COMP(mp_name, mp_str, mp_cat, mp_op) \
      Op__ ## mp_name = new Operator( \
          Operator::Category_ ## mp_cat, \
          Name::Intern("operator " mp_str), \
          Name::Intern("operator " mp_str), \
          true, \
          MakeInstruction_ ## mp_op); \
      s_OperatorTable->Add(Q_ ## mp_name, Op__ ## mp_name);

  s_OperatorTable = new OperatorTable();
  #include "./operators.inc"
}

// [T]
String Operator::ToString() const {
  return name_.ToString();
}

} // Compiler
