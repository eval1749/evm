// @(#)$Id$
//
// Evita Il - IR - Instructions - UseI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_UseI_h)
#define INCLUDE_Il_Ir_Instructions_UseI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   Pseudo instruction USE.
///   <example>
///     USE %rx
///   </example>
/// </summary>
class UseI : public Instruction_<UseI, Op_Use> {
    // ctor
    public: UseI();
    public: UseI(Output* pRx);

    // [I]
    public: virtual bool IsUseless() const override;

    DISALLOW_COPY_AND_ASSIGN(UseI);
}; // UseI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_UseI_h)
