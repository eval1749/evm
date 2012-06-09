// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_PseudoOutput_h)
#define INCLUDE_Il_Ir_PseudoOutput_h

#include "./SsaOutput.h"

namespace Il {
namespace Ir {

class PseudoOutput :
        public Operand_<PseudoOutput, SsaOutput> {

    CASTABLE_CLASS(PseudoOutput)

    // ctor
    public: PseudoOutput() {}

    // [G]
    public: virtual char16 GetPrefixChar() const override { return 'q'; }

    DISALLOW_COPY_AND_ASSIGN(PseudoOutput);
}; // PseudoOutput

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_PseudoOutput_h)
