// @(#)$Id$
//
// Evita Il - IR - Instructions - FieldPtrI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_FieldPtrI_h)
#define INCLUDE_Il_Ir_Instructions_FieldPtrI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class FieldPtrI : public Instruction_<FieldPtrI, Op_FieldPtr> {
  public: FieldPtrI();
  public: FieldPtrI(const Register&, const Operand&, const Field&);
  DISALLOW_COPY_AND_ASSIGN(FieldPtrI);
}; // FieldPtrI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_FieldPtrI_h)
