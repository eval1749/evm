// -*- Mode: C++; -*-
// @(#)$Id$
//
// Modifiers Instructions
//

DEFMODIFIER(Abstract, abstract, 0)
DEFMODIFIER(Const, const, 1)
DEFMODIFIER(Extern, extern, 2)
DEFMODIFIER(Internal, internal, 3)
DEFMODIFIER(New, new, 4)
DEFMODIFIER(Override, override, 5)
DEFMODIFIER(Private, private, 6)
DEFMODIFIER(Protected, protected, 7)
DEFMODIFIER(Public, public, 8)
DEFMODIFIER(ReadOnly, readonly, 9)
DEFMODIFIER(Final, final, 10)
DEFMODIFIER(Static, static, 11)
DEFMODIFIER(Virtual, virtual, 12)
DEFMODIFIER(Volatile, volatile, 13)

DEFMODIFIER(Anonymous, anonymous, 14)
DEFMODIFIER(SpecialName, specialName, 15)
DEFMODIFIER(Partial, partial, 16)

// Virtual Modifier for Class means interface. This applies to Class,
// GenericClass, and ConstructedClass.

// 10.1.1 Class modifiers
//  abstract final static
//  internal private protected public
//  new
//
//  Note: Virtual Class == Interface
//  Note: Static Virtual Class == Struct

// 11.1 Struct modifiers
//  internal private protected public
//  new

// 13.1.1 Interface modifiers
//  internal private protected public
//  new

// 14.2 Enum modifiers
//  internal private protected public
//  new

#undef DEFMODIFIER
