// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_ValuesI_h)
#define INCLUDE_Il_Ir_Instructions_ValuesI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// VALUES ty %vd <= %sx*
class ValuesI : public Instruction_<ValuesI, Op_Values> {
  // ctor
  public: ValuesI();
  public: ValuesI(const Type&, const Values&);
  public: ValuesI(const Type&, const Values&, const Operand&);
  public: ValuesI(const Values&);
  public: ValuesI(const Values&, const Operand&);
  public: ValuesI(const Values&, const Operand&, const Operand&);
  public: ValuesI(const Values&, const ValuesI&);

  // [G]
  private: const ValuesType& GetValuesTy() const;

  // [U]
  public: void UpdateOutputType();

  DISALLOW_COPY_AND_ASSIGN(ValuesI);
}; // ValuesI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_ValuesI_h)
