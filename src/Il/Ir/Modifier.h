// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Modifier_h)
#define INCLUDE_Il_Ir_Modifier_h

namespace Il {
namespace Ir {

enum Modifier {
    Modifier_None,

    #define DEFMODIFIER(mp_Name, mp_name, mp_nth) \
        Modifier_ ## mp_Name = 1 << mp_nth,

    #include "./Modifiers.inc"

    Modifier_Mask_ClassVariation = Modifier_Override | Modifier_Virtual,
        Modifier_ClassVariation_Class = 0,
        Modifier_ClassVariation_Interface = Modifier_Virtual,
        Modifier_ClassVariation_Struct = Modifier_Override,
}; // Modifier

String ModifiersToString(int);

} // Ir
} // Modifier

#endif // !defined(INCLUDE_Il_Ir_Modifier_h)
