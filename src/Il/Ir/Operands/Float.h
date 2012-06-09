// @(#)$Id$
//
// Evita Il - IR - Operands - Float
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Float_h)
#define INCLUDE_Il_Ir_Float_h

#include "./Register.h"

namespace Il {
namespace Ir {

/// <summary>
///   Symbolic register of intermediate representation of program.
/// </summary>
class Float : public Register_<Float> {
    CASTABLE_CLASS(Float)

    // ctor
    public: Float();
    public: explicit Float(Variable* const pVar);

    protected: virtual char16 GetPrefixChar() const override { return 'f'; }

    DISALLOW_COPY_AND_ASSIGN(Float);
}; // Float

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Float_h)
