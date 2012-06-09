// @(#)$Id$
//
// Evita Il - IR - Operands
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Operands_h)
#define INCLUDE_Il_Ir_Operands_h

// [A]
#include "./Operands/ArrayType.h"

// [B]
#include "./Operands/BoolOutput.h"
#include "./Operands/BoolType.h"

// [C]
#include "./Operands/Class.h"
#include "./Operands/ConstructedClass.h"
#include "./Operands/ConstructedMethod.h"

// [F]
#include "./Operands/Field.h"
#include "./Operands/FrameReg.h"
#include "./Operands/Function.h"
#include "./Operands/FunctionType.h"

// [F]
#include "./Operands/Float.h"

// [G]
#include "./Operands/GenericClass.h"
#include "./Operands/GenericMethod.h"

// [I]
#include "./Operands/Immediate.h"

// [L]
#include "./Operands/Label.h"
#include "./Operands/Literal.h"

// [M]
#include "./Operands/Method.h"
#include "./Operands/MethodGroup.h"
#include "./Operands/MethodGroupRef.h"
#include "./Operands/MethodRef.h"

// [N]
#include "./Operands/NameRef.h"

// Note: Don't use Namespace as instruction operand. Namespace class is
// derived from Class class for ease of supporting outer class.
#include "./Operands/Namespace.h"

// [O]
#include "./Operands/Operand.h"
#include "./Operands/Output.h"

// [P]
#include "./Operands/PointerType.h"
#include "./Operands/PrimitiveType.h"
#include "./Operands/Property.h"
#include "./Operands/PseudoOutput.h"

// [R]
#include "./Operands/Register.h"

// [S]
#include "./Operands/SsaOutput.h"

// [T]
#include "./Operands/Type.h"
#include "./Operands/TypeVar.h"

// [U]
#include "./Operands/UnresolvedType.h"
#include "./Operands/UnresolvedConstructedType.h"

// [V]
#include "./Operands/Variable.h"
#include "./Operands/Values.h"
#include "./Operands/ValuesType.h"
#include "./Operands/ValuesTypeBuilder.h"
#include "./Operands/VoidOutput.h"

#endif // !defined(INCLUDE_Il_Ir_Operands_h)
