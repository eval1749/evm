// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Immediate_h)
#define INCLUDE_Il_Ir_Immediate_h

#include "./Operand.h"

namespace Il {
namespace Ir {

class Immediate : public Operand_<Immediate> {
    CASTABLE_CLASS(Immediate);

    protected: Immediate() {}

    DISALLOW_COPY_AND_ASSIGN(Immediate);
}; // Immediate

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Immediate_h)
