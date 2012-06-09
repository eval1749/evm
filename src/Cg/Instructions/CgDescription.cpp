#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - CompilerInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

// TODO(yosi) 2012-02-05 ExecutorMain should not include X86 files.

#include "../CgDefs.h"

namespace Il {
namespace Ir {

using namespace Il::Cg;

namespace {
Instruction::Description s_instruction_descriptions[] = {
  { "Invalid", Instruction::Arity_0, nullptr },

  #define DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    { #mp_name, Instruction::Arity_ ## mp_arity, mp_name ## I :: New },

  #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format)

  #define DEFINE_TARGET_INSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    { #mp_name, Instruction::Arity_ ## mp_arity, nullptr },

  #include "../../Il/Ir/Instructions/instructions.inc"
};
} // namespace

const Instruction::Description& Instruction::GetDescription(Op const opcode) {
  ASSERT(opcode >= 0 && opcode < ARRAYSIZE(s_instruction_descriptions));
  return s_instruction_descriptions[opcode];
}

} // Ir
} // Il
