// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita IL Applyable IR Objects.
//

#if !defined(FUNCTOR_METHOD_2_)
    #error "You must define FUNCTOR_METHOD_2_"
#endif // !defined(FUNCTOR_METHOD_2_)

FUNCTOR_METHOD_2_(BBlock, IrObject)
FUNCTOR_METHOD_2_(Module, IrObject)

// Note: Namespace isn't "real" Class. So, we don't call Process(Class*).
FUNCTOR_METHOD_2_(ClassOrNamespace, IrObject)
FUNCTOR_METHOD_2_(Namespace, IrObject)

////////////////////////////////////////////////////////////
//
// Instructions
//
#define DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_desc) \
    FUNCTOR_METHOD_2_(mp_name ## I, mp_family ## Instruction)

#include "./Ir/Instructions/Instructions.inc"

FUNCTOR_METHOD_2_(ArithmeticInstruction, Instruction)
FUNCTOR_METHOD_2_(CastInstruction, Instruction)
FUNCTOR_METHOD_2_(GeneralInstruction, Instruction)
FUNCTOR_METHOD_2_(LastInstruction, Instruction)
FUNCTOR_METHOD_2_(OpenInstruction, Instruction)
FUNCTOR_METHOD_2_(OpenExitPointInstruction, OpenInstruction)
FUNCTOR_METHOD_2_(RelationalInstruction, Instruction)
FUNCTOR_METHOD_2_(TerminateInstruction, Instruction)

FUNCTOR_METHOD_2_(OperandBox, IrObject)
FUNCTOR_METHOD_2_(FunctionOperandBox, OperandBox)
FUNCTOR_METHOD_2_(PhiOperandBox, OperandBox)
FUNCTOR_METHOD_2_(SwitchOperandBox, OperandBox)

////////////////////////////////////////////////////////////
//
// Operands
//
FUNCTOR_METHOD_2_(ArrayType, Type)
FUNCTOR_METHOD_2_(BoolType, Type)
FUNCTOR_METHOD_2_(BoolOutput, SsaOutput)
FUNCTOR_METHOD_2_(Class, Type)
FUNCTOR_METHOD_2_(ConstructedClass, Type)
FUNCTOR_METHOD_2_(ConstructedMethod, Method)
FUNCTOR_METHOD_2_(Field, Operand)
FUNCTOR_METHOD_2_(Float, Register)
FUNCTOR_METHOD_2_(FrameReg, SsaOutput)
FUNCTOR_METHOD_2_(Function, Operand)
FUNCTOR_METHOD_2_(FunctionType, Type)
FUNCTOR_METHOD_2_(GenericClass, Class)

FUNCTOR_METHOD_2_(Immediate, Operand)
FUNCTOR_METHOD_2_(Label, Immediate)
FUNCTOR_METHOD_2_(Literal, Immediate)
//FUNCTOR_METHOD_2_(NullOperand, Immediate)

FUNCTOR_METHOD_2_(Method, Operand)
FUNCTOR_METHOD_2_(MethodGroup, Operand)
FUNCTOR_METHOD_2_(NameRef, Operand)
FUNCTOR_METHOD_2_(Output, Operand)
FUNCTOR_METHOD_2_(PseudoOutput, SsaOutput)
FUNCTOR_METHOD_2_(PointerType, Type)
FUNCTOR_METHOD_2_(Register, SsaOutput)
FUNCTOR_METHOD_2_(SsaOutput, Output)
FUNCTOR_METHOD_2_(Type, Operand)
FUNCTOR_METHOD_2_(TypeParam, Type)
FUNCTOR_METHOD_2_(TypeVar, Type)
FUNCTOR_METHOD_2_(UnresolvedType, Type)
FUNCTOR_METHOD_2_(Values, SsaOutput)
FUNCTOR_METHOD_2_(ValuesType, Type)
FUNCTOR_METHOD_2_(Variable, Operand)
FUNCTOR_METHOD_2_(VoidOutput, Output)

#undef FUNCTOR_METHOD_1_
#undef FUNCTOR_METHOD_2_

