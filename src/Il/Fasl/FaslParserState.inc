// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita VM IL FASL Parser State
//

#if !defined(DEFSTATE)
    #error "You must define DEFSTATE macro."
#endif // !defined(DEFSTATE)

DEFSTATE(None)

DEFSTATE(Class)
DEFSTATE(Class_Name)
DEFSTATE(Class_Name_Outer)
DEFSTATE(Class_Name_Outer_Modifiers)
DEFSTATE(Class_Name_Outer_Modifiers_BaseClass)
DEFSTATE(Class_Name_Outer_Modifiers_BaseClass_Interface)

DEFSTATE(Field)
DEFSTATE(Field_Name)
DEFSTATE(Field_Name_Modifiers)

DEFSTATE(FunctionDef)
DEFSTATE(FunctionDef_Ref)
DEFSTATE(FunctionDef_Ref_Type)
DEFSTATE(FunctionDef_Ref_Type_Variable)

DEFSTATE(Instruction)
DEFSTATE(Instruction_Continue)
DEFSTATE(Instruction_OpCode)
DEFSTATE(Instruction_OpCode_Type)
DEFSTATE(Instruction_OpCode_Type_Operands)

DEFSTATE(Method)
DEFSTATE(Method_Modifiers)
DEFSTATE(Method_Modifiers_FunctionType)

DEFSTATE(MethodGroup)
DEFSTATE(MethodGroup_Name)

DEFSTATE(Name_Char)
DEFSTATE(Name64)

DEFSTATE(Start)
DEFSTATE(Toplevel)

#undef DEFSTATE
