// @(#)$Id$
//
// Evita Il - Cg - Operands
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Il_Cg_Operands_FunLit_h)
#define INCLUDE_Evita_Il_Cg_Operands_FunLit_h

#include "../Il/Ir/Operands.h"
#include "./Operands/CgOperand.h"
#include "./Operands/Physical.h"

namespace Il {
namespace Cg {

/// <summary>
///   Represents function literal for function being compiled in IR.
///   X86LowerTask uses FunLit for OPENFINALLY instruction.
/// </summary>
class FunLit : public CgOperand_<FunLit, CgImmediate> {
  CASTABLE_CLASS(FunLit)

  private: const Function& value_;

  // ctor
  public: explicit FunLit(const Function& value) : value_(value) {}

  // properties
  public: const Function& value() const { return value_; }

  // [G]
  public: virtual const Type& GetTy() const override { return *Ty_VoidPtr; }

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(FunLit);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_Evita_Il_Cg_Operands_FunLit_h)
