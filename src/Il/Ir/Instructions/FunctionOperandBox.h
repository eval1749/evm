// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_FunctionOperandBox_h)
#define INCLUDE_Il_FunctionOperandBox_h

#include "./OperandBox.h"

namespace Il {
namespace Ir {

/// <summary>
///   Function operand box used for tracking user(?)
//    Note: Do we really ned this?
/// </summary>
class FunctionOperandBox :
        public OperandBox,
        public ChildItem_<FunctionOperandBox, Function> {

    public: FunctionOperandBox(Function* const pFunction);

    DISALLOW_COPY_AND_ASSIGN(FunctionOperandBox);
}; // FunctionOperandBox

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_FunctionOperandBox_h)
