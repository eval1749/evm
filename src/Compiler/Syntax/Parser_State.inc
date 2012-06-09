// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Parser - State
//
#if !defined(DEFSTATE)
    #error "You must define DEFSTATE macro."
#endif // !defined(DEFSTATE)

DEFSTATE(Block)

DEFSTATE(Class)
DEFSTATE(Class_Member)
DEFSTATE(Class_Member_Modifier)
DEFSTATE(Class_Member_Type)
DEFSTATE(Class_Member_Type_Name)
DEFSTATE(Class_Member_Type_Operator)
DEFSTATE(Class_Name)
DEFSTATE(Class_Name_Colon)
DEFSTATE(Class_Name_Colon_Name)

DEFSTATE(CompilationUnit)
DEFSTATE(CompilationUnit_Attribute)
DEFSTATE(CompilationUnit_Member)

DEFSTATE(Enum)
DEFSTATE(Enum_Finish)
DEFSTATE(Enum_Member)
DEFSTATE(Enum_Member_Name)
DEFSTATE(Enum_Member_Name_Eq_Expr)
DEFSTATE(Enum_Name)
DEFSTATE(Enum_Name_Colon_Type)

DEFSTATE(BoolExpr)
DEFSTATE(BoolExpr_Expr)

DEFSTATE(Expr)
DEFSTATE(Expr_Expr)

DEFSTATE(Expr_Add)
DEFSTATE(Expr_Add_Mul)
DEFSTATE(Expr_Add_Mul_Op__Mul)

// 7.17 Assign operators
// assignment:
//  unary-expression   assignment-operator   expression
DEFSTATE(Expr_Assign)
DEFSTATE(Expr_Assign_Expr)
DEFSTATE(Expr_Assign_Expr_Compound_Expr)
DEFSTATE(Expr_Assign_Expr_Simple_Expr)

// 7.14 Conditional operator
DEFSTATE(Expr_Cond)
//DEFSTATE(Expr_Cond_Bool)
DEFSTATE(Expr_Cond_Bool_Op)
DEFSTATE(Expr_Cond_Bool_Op__Then)
DEFSTATE(Expr_Cond_Bool_Op__Then_Else)

// 7.12 Conditional logical operators
DEFSTATE(Expr_CondAnd)
DEFSTATE(Expr_CondAnd_LogIor)
DEFSTATE(Expr_CondAnd_LogIor_Op__LogIor)

// 7.12 Conditional logical operators
DEFSTATE(Expr_CondOr)
DEFSTATE(Expr_CondOr_CondAnd)
DEFSTATE(Expr_CondOr_CondAnd_Op_CondAnd)

DEFSTATE(Expr_Equality)
DEFSTATE(Expr_Equality_Relational)
DEFSTATE(Expr_Equality_Relational_Op__Relational)

DEFSTATE(Expr_LogAnd)
DEFSTATE(Expr_LogAnd_Equality)
DEFSTATE(Expr_LogAnd_Equality_Op__Equality)

DEFSTATE(Expr_LogIor)
DEFSTATE(Expr_LogIor_LogXor)
DEFSTATE(Expr_LogIor_LogXor_Op__LogXor)

// 7.7.4 Bitwise complement operator
DEFSTATE(Expr_LogNot_Unary)

DEFSTATE(Expr_LogXor)
DEFSTATE(Expr_LogXor_LogAnd)
DEFSTATE(Expr_LogXor_LogAnd_Op__LogAnd)

// 7.7.2 Unary minus operator
DEFSTATE(Expr_Minus_Unary)

DEFSTATE(Expr_Mul)
DEFSTATE(Expr_Mul_Unary)
DEFSTATE(Expr_Mul_Unary_Op__Mul)

// 7.7.3 Logical negation operator
DEFSTATE(Expr_Not_Unary)

// 7.13 The null coalescing operator
DEFSTATE(Expr_NullOr)
DEFSTATE(Expr_NullOr_Expr)
//DEFSTATE(Expr_NullOr_Expr_Op)
DEFSTATE(Expr_NullOr_Expr_Op__Expr)

// 7.7.1 Unary plus operator
DEFSTATE(Expr_Plus_Unary)

// 7.7.5 Prefix increment and decrement operators
DEFSTATE(Expr_Add1_Unary)
DEFSTATE(Expr_Sub1_Unary)

DEFSTATE(Expr_Primary)

DEFSTATE(Expr_Primary_Dot)

DEFSTATE(Expr_Primary_New)
DEFSTATE(Expr_Primary_New_ArrayType)
DEFSTATE(Expr_Primary_New_Type)
DEFSTATE(Expr_Primary_New_Type_Bracket)
DEFSTATE(Expr_Primary_New_Type_Bracket_Expr)
DEFSTATE(Expr_Primary_New_Type_Bracket_Expr_Bracket)
DEFSTATE(Expr_Primary_New_Type_Paren)
DEFSTATE(Expr_Primary_New_Type_Paren_Expr)

DEFSTATE(Expr_Primary_Name)

DEFSTATE(Expr_Primary_Paren_Expr)

DEFSTATE(Expr_Primary_Primary)
DEFSTATE(Expr_Primary_Primary_Bracket_Expr)
DEFSTATE(Expr_Primary_Primary_Paren)
DEFSTATE(Expr_Primary_Primary_Paren_Expr)

// 7.6.9 Postfix increment and decrement operators
DEFSTATE(Expr_Primary_Post)

DEFSTATE(Expr_Relational)
DEFSTATE(Expr_Relational_Shift)
DEFSTATE(Expr_Relational_Shift_Op__Shift)

DEFSTATE(Expr_Shift)
DEFSTATE(Expr_Shift_Expr)
DEFSTATE(Expr_Shift_Expr_Op__Expr)

DEFSTATE(Expr_Unary)

DEFSTATE(Field_Name_Eq)
DEFSTATE(Field_Name_Eq_Expr)

DEFSTATE(Indexer)
DEFSTATE(Indexer_Accessor)
DEFSTATE(Indexer_Accessor_Finish)
DEFSTATE(Indexer_Members)
DEFSTATE(Indexer_Param)
DEFSTATE(Indexer_Param_CloseBracket)
DEFSTATE(Indexer_Param_Type)
DEFSTATE(Indexer_Param_Type_Name)

//DEFSTATE(Interface)

////////////////////////////////////////////////////////////
//
// Method Definition
//
DEFSTATE(Method_Body)
DEFSTATE(Method_Param)
DEFSTATE(Method_Param_CloseParen)
DEFSTATE(Method_Param_Type)
DEFSTATE(Method_Param_Type_Name)

////////////////////////////////////////////////////////////
//
// Modifier
//
DEFSTATE(Modifier)
DEFSTATE(Modifier_Modifiers)

////////////////////////////////////////////////////////////
//
// Namespace Definition
//
DEFSTATE(Namespace)
DEFSTATE(Namespace_Body)
DEFSTATE(Namespace_Member)
DEFSTATE(Namespace_Name)

////////////////////////////////////////////////////////////
//
// Property
//
DEFSTATE(Property_Accessor)
DEFSTATE(Property_Accessor_Finish)
DEFSTATE(Property_Members)

////////////////////////////////////////////////////////////
//
// Statement
//
DEFSTATE(Stmt)

DEFSTATE(Stmt_Break)

DEFSTATE(Stmt_Brace)
DEFSTATE(Stmt_Brace_Stmt)

DEFSTATE(Stmt_Continue)

DEFSTATE(Stmt_Do)
DEFSTATE(Stmt_Do_Stmt)
DEFSTATE(Stmt_Do_Stmt_While)
DEFSTATE(Stmt_Do_Stmt_While_Expr)

DEFSTATE(Stmt_Expr)

DEFSTATE(Stmt_For)
DEFSTATE(Stmt_For_Finish)
DEFSTATE(Stmt_For_Paren)
DEFSTATE(Stmt_For_Paren_Expr)
DEFSTATE(Stmt_For_Paren_Init)
DEFSTATE(Stmt_For_Paren_Init_Cond)
DEFSTATE(Stmt_For_Paren_Init_Cond_Step)
DEFSTATE(Stmt_For_Paren_Var)
DEFSTATE(Stmt_For_Paren_Var_Name)
DEFSTATE(Stmt_For_Paren_Var_Name_Eq)
DEFSTATE(Stmt_For_Paren_Var_Name_Eq_Expr)

DEFSTATE(Stmt_ForEach)
DEFSTATE(Stmt_ForEach_EndLoop)
DEFSTATE(Stmt_ForEach_Finish)

DEFSTATE(Stmt_Goto)
DEFSTATE(Stmt_Goto_Case_Expr)
DEFSTATE(Stmt_Goto_Label)

DEFSTATE(Stmt_If)
DEFSTATE(Stmt_If_Expr)
DEFSTATE(Stmt_If_Expr_Then)
DEFSTATE(Stmt_If_Expr_Then_Else)

// For label definition
DEFSTATE(Stmt_Name)

DEFSTATE(Stmt_Return_Expr)

DEFSTATE(Stmt_Switch)
DEFSTATE(Stmt_Switch_Body)
DEFSTATE(Stmt_Switch_Case_Expr)
DEFSTATE(Stmt_Switch_Default)
DEFSTATE(Stmt_Switch_Expr)
DEFSTATE(Stmt_Switch_Expr_Paren)

DEFSTATE(Stmt_Throw_Expr)

DEFSTATE(Stmt_Try)
DEFSTATE(Stmt_Try_)
DEFSTATE(Stmt_Try_Block)
DEFSTATE(Stmt_Try_Block_Finally)
DEFSTATE(Stmt_Try_Block_Finally_Block)
DEFSTATE(Stmt_Try_Catch)
DEFSTATE(Stmt_Try_Catch_Block)
DEFSTATE(Stmt_Try_Catch_Paren)
DEFSTATE(Stmt_Try_Catch_Paren_Name)
DEFSTATE(Stmt_Try_Catch_Paren_Name_Name)
DEFSTATE(Stmt_Try_Catch_Paren_Name_Name_Paren)
DEFSTATE(Stmt_Try_Catch_Paren_Name_Paren)

DEFSTATE(Stmt_Type)
DEFSTATE(Stmt_Type_Name)
DEFSTATE(Stmt_Type_Name_Comma)
//DEFSTATE(Stmt_Type_Name_Eq)
DEFSTATE(Stmt_Type_Name_Eq_Expr)

DEFSTATE(Stmt_Using)
DEFSTATE(Stmt_Using_Expr)
DEFSTATE(Stmt_Using_Finish)
DEFSTATE(Stmt_Using_Paren)
DEFSTATE(Stmt_Using_Paren_Name)
DEFSTATE(Stmt_Using_Type)
DEFSTATE(Stmt_Using_Type_Name)
DEFSTATE(Stmt_Using_Type_Name_Eq_Expr)

DEFSTATE(Stmt_VarDef)
//DEFSTATE(Stmt_VarDef_Name)

DEFSTATE(Stmt_While)
DEFSTATE(Stmt_While_Expr)
DEFSTATE(Stmt_While_Expr_Stmt)

//DEFSTATE(Struct)

////////////////////////////////////////////////////////////
//
// Type Name Reference
//
DEFSTATE(Type)
//DEFSTATE(Type_Name)
DEFSTATE(Type_TypeName)

DEFSTATE(TypeDef)
DEFSTATE(TypeDef_Modifiers)

////////////////////////////////////////////////////////////
//
// Using
//
DEFSTATE(Using)
DEFSTATE(Using_DottedName)
DEFSTATE(Using_Name)
DEFSTATE(Using_Name_Eq)
DEFSTATE(Using_Name_Eq_Name)

#undef DEFSTATE
