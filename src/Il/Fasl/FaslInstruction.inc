// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita VM IL FASL Instruction Set
//
DEFFASL(ArrayType, (Type, Rank))
DEFFASL(Class, (Name, Modifiers, Outer, BaseClass, Interfaces))
DEFFASL(ConstructedClass, (Name, Modifiers, Outer, GenericClass, TypeArgs))
DEFFASL(GenericClass, (Name, Modifiers, Outer, BaseClass, Interfaces, TypeParams))
DEFFASL(Field, (Name, Modifiers, Type))
DEFFASL(Function, (Name, FunctionType, Method, BBlockIndexBits Instructions))
DEFFASL(Instruction, (Opcode, Type, Output, Operands))

DEFFASL(PointerType, (Type))

// Encode new() constraint into modifier
DEFFASL(TypeParam, (Name, Modifiers, Outer, BaseClass, Interfaces))


DEFFASL(Literal, (Type, Char_Float_Integer_String))
DEFFASL(VarInt4, (Bit4))
DEFFASL(VarInt5, (Bit5))
DEFFASL(VarInt6, (Bit6))
DEFFASL(VarInt7, (Bit7))
DEFFASL(VarInt8, (Bit8))
DEFFASL(VarUint4, (Bit4))
DEFFASL(VarUint5, (Bit5))
DEFFASL(VarUint6, (Bit6))
DEFFASL(VarUint7, (Bit7))
DEFFASL(VarUInt8, (Bit8))
DEFFASL(Method, (Modifier, FunctionType, Names, Module))
DEFFASL(MethodGroup, (Name, Methods))
DEFFASL(Name64, (Bit6))  // End with 0
DEFFASL(String, (LengthFormat, Length, UTF_8s))
DEFFASL(Float32, (Sign, Exponent, Significand))
DEFFASL(Float64, (Sign, Exponent, Significand))
DEFFASL(Ref4, VarUint4)
DEFFASL(Ref5, VarUint4)
DEFFASL(Ref6, VarUint4)
DEFFASL(Ref7, VarUint4)
DEFFASL(Ref8, VarUint4)
DEFFASL(BBlockRef, VarUInt8)
// Number of operands are around 40.
// OP_Limit=67
// FaslOp_Limit=45

// Common.Int32, Common.String => Assign short code.

// Op VarUint3 => 140 = 8bit = 3 3 3 3 = 12
// Op VarUint4 => 140 = 8bit = 4 4 4 = 12
//  0 Ref4

//    16/4 32/5 64/6   128/7 256/8 512/9 1024/10 2048/11 4096/12
// V3 2 6  3  9  3  9   4 12  4 12   5 15  5 15    6 18    6 18
// V4 2 8  2  8  2  8   3 12  3 12   3 12  4 16    4 16    4 16
// V5 1 5  2 10  2 10   2 10  2 10   3 15  3 15    3 15    3 15
// V6 1 5  2 10  2 10   2 10  2 10   3 15  3 15    3 15    3 15
// V8 1 7  1  7  1  7   1  7  2 14   2 14  2 14    2 14    2 14
