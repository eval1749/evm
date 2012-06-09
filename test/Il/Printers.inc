// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita VM Printers
//

#if!defined(DEFINE_PRINTER2)
    #error "You must define DEFINE_PRINTER2"
#endif

#if!defined(DEFINE_PRINTER3)
    #error "You must define DEFINE_PRINTER3"
#endif

DEFINE_PRINTER2(Common, Object)
DEFINE_PRINTER2(Common, String)

DEFINE_PRINTER3(Il, Ir, BBlock)
DEFINE_PRINTER3(Il, Ir, Module)
DEFINE_PRINTER3(Il, Ir, Instruction)

DEFINE_PRINTER3(Il, Ir, Operand)
  DEFINE_PRINTER3(Il, Ir, ConstructedMethod)
  DEFINE_PRINTER3(Il, Ir, Function)
  DEFINE_PRINTER3(Il, Ir, GenericMethod)
  DEFINE_PRINTER3(Il, Ir, Literal)
    DEFINE_PRINTER3(Il, Ir, BooleanLiteral)
    DEFINE_PRINTER3(Il, Ir, CharLiteral)
    DEFINE_PRINTER3(Il, Ir, Float32Literal)
    DEFINE_PRINTER3(Il, Ir, Float64Literal)
    DEFINE_PRINTER3(Il, Ir, Int16Literal)
    DEFINE_PRINTER3(Il, Ir, Int32Literal)
    DEFINE_PRINTER3(Il, Ir, Int64Literal)
    DEFINE_PRINTER3(Il, Ir, Int8Literal)
    DEFINE_PRINTER3(Il, Ir, StringLiteral)
    DEFINE_PRINTER3(Il, Ir, UInt16Literal)
    DEFINE_PRINTER3(Il, Ir, UInt32Literal)
    DEFINE_PRINTER3(Il, Ir, UInt64Literal)
    DEFINE_PRINTER3(Il, Ir, UInt8Literal)
  DEFINE_PRINTER3(Il, Ir, Method)
  DEFINE_PRINTER3(Il, Ir, MethodGroup)
  DEFINE_PRINTER3(Il, Ir, Namespace)

DEFINE_PRINTER3(Il, Ir, Output)
  DEFINE_PRINTER3(Il, Ir, BoolOutput)
  DEFINE_PRINTER3(Il, Ir, Float)
  DEFINE_PRINTER3(Il, Ir, Register)
  DEFINE_PRINTER3(Il, Ir, SsaOutput)
  DEFINE_PRINTER3(Il, Ir, Values)
  DEFINE_PRINTER3(Il, Ir, VoidOutput)

DEFINE_PRINTER3(Il, Ir, Type)
  DEFINE_PRINTER3(Il, Ir, ArrayType)
  DEFINE_PRINTER3(Il, Ir, Class)
  DEFINE_PRINTER3(Il, Ir, ConstructedClass)
  DEFINE_PRINTER3(Il, Ir, FunctionType)
  DEFINE_PRINTER3(Il, Ir, GenericClass)
  DEFINE_PRINTER3(Il, Ir, PointerType)
  DEFINE_PRINTER3(Il, Ir, ValuesType)

#undef DEFINE_PRINTER2
#undef DEFINE_PRINTER3
