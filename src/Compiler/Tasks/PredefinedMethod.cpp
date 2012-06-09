#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Predefined Method
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (eval1749@)

#include "./PredefinedMethod.h"

namespace Compiler {

PredefinedMethod::PredefinedMethod(
    const MethodGroup& method_group,
    int const modifiers,
    const FunctionType& function_type,
    Op opcode,
    RewriteRule rewrite_rule)
    : Base(method_group, modifiers, function_type),
      opcode_(opcode),
      rewrite_rule_(rewrite_rule) {}

} // Compiler
