#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Type
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./PrimitiveType.h"

#include "./Namespace.h"

namespace Il {
namespace Ir {

PrimitiveType::PrimitiveType(
    const Name& name,
    RegClass const reg_class,
    int const bit_width,
    bool const has_sign)
    : Base(
        *Namespace::Common,
        Modifier_Public | Modifier_Final,
        name),
      has_sign_(has_sign),
      reg_class_(reg_class),
      bit_width_(bit_width) {
}

} // Ir
} // Il
