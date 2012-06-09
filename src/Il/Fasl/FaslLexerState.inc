// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita VM IL FASL Lexer State
//

#if !defined(DEFSTATE)
    #error "You must define DEFSTATE macro."
#endif // !defined(DEFSTATE)

DEFSTATE(None)

DEFSTATE(ArrayType_Type)
DEFSTATE(ArrayType_Type_Rank)

DEFSTATE(Char)

DEFSTATE(ClassRef_Outer)
DEFSTATE(ClassRef_Outer_Name)

DEFSTATE(FaslOp)
DEFSTATE(FaslOp_V4)

DEFSTATE(FieldRef)
DEFSTATE(FieldRef_Class)
DEFSTATE(FieldRef_Class_Name)

DEFSTATE(FunctionRef_Outer)
DEFSTATE(FunctionRef_Outer_Name)
DEFSTATE(FunctionRef_Outer_Name_Flavor)

DEFSTATE(FunctionType_ReturnType)
DEFSTATE(FunctionType_ReturnType_ParamsType)

DEFSTATE(GotOpCode)
DEFSTATE(GotToken)

DEFSTATE(I4)
DEFSTATE(I4x2)
DEFSTATE(I4x3)

DEFSTATE(Label_V4)

DEFSTATE(LineAdd_Delta)
DEFSTATE(LineAdd_Delta_Column)

DEFSTATE(LineColAdd_Delta)

DEFSTATE(LineSub_Delta)
DEFSTATE(LineSub_Delta_Column)

DEFSTATE(LineColSub_Delta)

DEFSTATE(LineInfo_FileName)
DEFSTATE(LineInfo_FileName_Line)
DEFSTATE(LineInfo_FileName_Line_Column)

DEFSTATE(Literal_Type)
DEFSTATE(Literal_Type_Value)

DEFSTATE(MethodRef_Class)
DEFSTATE(MethodRef_Class_Type)
DEFSTATE(MethodRef_Class_Type_Name)

DEFSTATE(Name64)

DEFSTATE(NamespaceRef_Outer)
DEFSTATE(NamespaceRef_Outer_Name)

DEFSTATE(OpCode)

DEFSTATE(PointerType_Type)

DEFSTATE(SetContext)

DEFSTATE(String_Length)
DEFSTATE(String_Length_Char)

DEFSTATE(V4)
DEFSTATE(V4_V4)
DEFSTATE(V7)
DEFSTATE(V7_V7)
DEFSTATE(V8)
DEFSTATE(V8_V8)

DEFSTATE(ValuesType)

#undef DEFSTATE
