#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - CompilerInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../Instructions.h"

namespace Il {
namespace Ir {

namespace {
Instruction::Description s_instruction_descriptions[] = {
  { "Invalid", Instruction::Arity_0, nullptr },

  #define DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    { #mp_name, Instruction::Arity_ ## mp_arity, mp_name ## I :: New },

  #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    { #mp_name, Instruction::Arity_ ## mp_arity, nullptr },

  #define DEFINE_TARGET_INSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    { #mp_name, Instruction::Arity_ ## mp_arity, nullptr },

  #include "./instructions.inc"
};
} // namespace

const Instruction::Description& Instruction::GetDescription(Op const opcode) {
  ASSERT(opcode >= 0 && opcode < ARRAYSIZE(s_instruction_descriptions));
  return s_instruction_descriptions[opcode];
}

} // Ir
} // Il
