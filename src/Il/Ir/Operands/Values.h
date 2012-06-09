// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Values_h)
#define INCLUDE_Il_Ir_Values_h

#include "./SsaOutput.h"

namespace Il {
namespace Ir {

/// <summary>
///   Values operand
/// </summary>
class Values :
        public Operand_<Values, SsaOutput> {

    CASTABLE_CLASS(Values)

    // ctor
    public: Values();

    // [G]
    public: virtual char16 GetPrefixChar() const override { return 'v'; }

    DISALLOW_COPY_AND_ASSIGN(Values);
}; // Values

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Values_h)
