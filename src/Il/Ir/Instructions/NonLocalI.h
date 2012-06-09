// @(#)$Id$
//
// Evita Il - IR - Instructions - NonLocalI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_NonLocalI_h)
#define INCLUDE_Il_Ir_Instructions_NonLocalI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class NonLocalI : public Instruction_<NonLocalI, Op_NonLocal> {
  public: NonLocalI();
  public: NonLocalI(const Type&, const Values&);
  DISALLOW_COPY_AND_ASSIGN(NonLocalI);
}; // NonLocalI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_NonLocalI_h)
