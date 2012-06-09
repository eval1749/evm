// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Keywords
//

#if !defined(DEFKEYWORD)
    #define DEFKEYWORD(mp_name)
#endif

#if !defined(DEFMODIFIER)
    #define DEFMODIFIER(mp_name) DEFKEYWORD(mp_name)
#endif

#if !defined(DEFTYPE)
    #define DEFTYPE(mp_name, mp_Name) DEFKEYWORD(mp_name)
#endif

#if !defined(DEFTYPE_ALIAS)
    #define DEFTYPE_ALIAS(mp_name, mp_Type) DEFTYPE(mp_name, mp_Type)
#endif

// C# 1.0 keywords
#undef abstract
DEFMODIFIER(abstract)   // Modifier 0
#define abstract virtual
DEFKEYWORD(as)
DEFKEYWORD(base)
DEFTYPE(bool, Boolean)
DEFKEYWORD(break)
//DEFTYPE(byte)
DEFKEYWORD(case)
DEFKEYWORD(catch)
DEFTYPE(char, Char)
//DEFKEYWORD(checked)
DEFKEYWORD(class)
DEFMODIFIER(const)      // Modifier 1
DEFKEYWORD(continue)
//DEFKEYWORD(decimal)
DEFKEYWORD(default)
//DEFKEYWORD(delegate)
DEFKEYWORD(do)
//DEFKEYWORD(double)
DEFKEYWORD(else)
DEFKEYWORD(enum)
//DEFKEYWORD(event)
DEFKEYWORD(explicit)
DEFMODIFIER(extern)     // Modifier 2
DEFKEYWORD(false)
DEFKEYWORD(finally)
//DEFKEYWORD(fixed)
//DEFTYPE(float)
DEFKEYWORD(for)
//DEFKEYWORD(foreach)
DEFKEYWORD(goto)
DEFKEYWORD(if)
DEFKEYWORD(implicit)
DEFKEYWORD(in)
DEFTYPE_ALIAS(int, Int32)
DEFMODIFIER(internal)   // Modifier 3
#undef interface
DEFKEYWORD(interface)
#define interface struct
DEFKEYWORD(is)
//DEFKEYWORD(lock)
//DEFTYPE(long)
DEFKEYWORD(namespace)
DEFMODIFIER(new)        // Modifier 4
DEFKEYWORD(null)
//DEFKEYWORD(object)
DEFKEYWORD(operator)
//DEFKEYWORD(out)
DEFMODIFIER(override)   // Modifier 5
//DEFKEYWORD(params)
DEFMODIFIER(private)    // Modifier 6
DEFMODIFIER(protected)  // Modifier 7
DEFMODIFIER(public)     // Modifier 8
DEFMODIFIER(readonly)   // Modifier 9
//DEFKEYWORD(ref)
DEFKEYWORD(return)
//DEFTYPE(sbyte)
DEFMODIFIER(final)     // Modifier 10
//DEFTYPE(short)
DEFKEYWORD(sizeof)
//DEFKEYWORD(stackalloc)
DEFMODIFIER(static)     // Modifier 11
DEFKEYWORD(struct)
DEFKEYWORD(switch)
DEFKEYWORD(this)
DEFKEYWORD(throw)
DEFKEYWORD(true)
DEFKEYWORD(try)
DEFKEYWORD(typeof)
DEFTYPE_ALIAS(uint, UInt32)
//DEFTYPE(ulong)
//DEFKEYWORD(unchecked)
//DEFKEYWORD(unsafe)
//DEFTYPE(ushort)
DEFKEYWORD(using)
DEFMODIFIER(virtual)    // Modifier 12
DEFTYPE(void, Void)
DEFMODIFIER(volatile)   // Modifier 13
DEFKEYWORD(while)

// Evita specific
DEFTYPE(float32, Float32)
DEFTYPE(float64, Float64)

DEFTYPE(int16, Int16)
DEFTYPE(int32, Int32)
DEFTYPE(int64, Int64)
DEFTYPE(int8, Int8)

DEFTYPE(uint16, UInt16)
DEFTYPE(uint32, UInt32)
DEFTYPE(uint64, UInt64)
DEFTYPE(uint8, UInt8)

#undef DEFKEYWORD
#undef DEFMODIFIER
#undef DEFTYPE
#undef DEFTYPE_ALIAS
