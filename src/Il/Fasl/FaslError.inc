// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita VM IL FASL Operation Code
//

#if !defined(DEFERROR)
    #error "You must define DEFERROR macro."
#endif // !defined(DEFERROR)

DEFERROR(Context, Class)
DEFERROR(Context, Property)

DEFERROR(Duplicate, Field)
DEFERROR(Duplicate, Method)
DEFERROR(Duplicate, MethodGroup)

DEFERROR(Expect, Array)
DEFERROR(Expect, Class)
DEFERROR(Expect, FileMagic)
DEFERROR(Expect, Function)
DEFERROR(Expect, FunctionType)
DEFERROR(Expect, GenericClass)
DEFERROR(Expect, GenericMethod)
DEFERROR(Expect, Int32)
DEFERROR(Expect, Label)
DEFERROR(Expect, Method)
DEFERROR(Expect, MethodGroup)
DEFERROR(Expect, Name)
DEFERROR(Expect, Namespace)
DEFERROR(Expect, PhiI)
DEFERROR(Expect, Operand)
DEFERROR(Expect, SsaOutput)
DEFERROR(Expect, SourceInfo)
DEFERROR(Expect, Type)
DEFERROR(Expect, TypeParam)
DEFERROR(Expect, Variable)
DEFERROR(Expect, ValuesType)

DEFERROR(Invalid, ArrayType)
DEFERROR(Invalid, BBlock)
DEFERROR(Invalid, Char)
DEFERROR(Invalid, ConstructedClass)
DEFERROR(Invalid, Context)
DEFERROR(Invalid, FaslOp)
DEFERROR(Invalid, Field)
DEFERROR(Invalid, Function)
DEFERROR(Invalid, Instruction)
DEFERROR(Invalid, Label)
DEFERROR(Invalid, Literal)
DEFERROR(Invalid, Method)
DEFERROR(Invalid, MethodParam)
DEFERROR(Invalid, NameRef)
DEFERROR(Invalid, NamespaceRef)
DEFERROR(Invalid, PhiI)
DEFERROR(Invalid, Property)
DEFERROR(Invalid, Operand)
DEFERROR(Invalid, Output)
DEFERROR(Invalid, Reference)
DEFERROR(Invalid, SourceInfo)
DEFERROR(Invalid, State)
DEFERROR(Invalid, Stack)
DEFERROR(Invalid, SwitchI)
DEFERROR(Invalid, ValuesType)   // ValuesType contains ValuesType.
DEFERROR(Invalid, Variable)

DEFERROR(NotFound, Class)
DEFERROR(NotFound, Method)
DEFERROR(NotFound, MethodGroup)
DEFERROR(NotFound, Namespace)

DEFERROR(Unexpected, Operand)

#undef DEFERROR
