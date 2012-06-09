// @(#)$Id$
//
// Evita Il - IR - Instructions - CopyInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_Instructions_CopyInstruction_h)
#define INCLUDE_Il_Cg_Instructions_CopyInstruction_h

#include "./CgInstruction.h"

namespace Il {
namespace Cg {

class CopyInstruction : public CgInstructionFamily_<CopyInstruction> {
  CASTABLE_CLASS(CopyInstruction)

  // ctor
  protected: CopyInstruction(Op, const Type*, const Output*);
  protected: CopyInstruction(Op, const Type&, const Output&);

  // [I]
  public: virtual bool IsUseless() const override;

  DISALLOW_COPY_AND_ASSIGN(CopyInstruction);
}; // CopyInstruction

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_Instructions_CopyInstruction_h)
