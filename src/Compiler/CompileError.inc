// -*- Mode: C++ -*-
// @(#)$Id$
//
// Evita Compiler - Parser - Statement
//

#if !defined(DEFERROR)
    #error "You must define DEFERROR macro."
#endif // !defined(DEFERROR)

// Finalize
DEFERROR(Finalize, Field_StaticConst_NotConst)

// Host
DEFERROR(Host, File_NotFound)

// Instruction
DEFERROR(Instruction, Invalid)

// Lexer
DEFERROR(Lexer, Bad_BackSlash)
DEFERROR(Lexer, Bad_CharLiteral)
DEFERROR(Lexer, Bad_HexDigit)
DEFERROR(Lexer, Bad_Real)
DEFERROR(Lexer, Bad_RankSpec)
DEFERROR(Lexer, Expect_Digit)
DEFERROR(Lexer, Expect_IdStart)
DEFERROR(Lexer, Expect_SingleQuote)
DEFERROR(Lexer, Invalid_Char)
DEFERROR(Lexer, Invalid_Name)
DEFERROR(Lexer, Unclosed_BlockComment)
DEFERROR(Lexer, Unclosed_DoubleQuote)
DEFERROR(Lexer, Unclosed_RankSpec)
DEFERROR(Lexer, Unclosed_SingleQuote)
DEFERROR(Lexer, Unexpected_Eof)
DEFERROR(Lexer, Unexpected_Newline)

// Other
DEFERROR(Other, Error)

// Parse
DEFERROR(Parse, NYI)

DEFERROR(Parse, Class_Redefined_Class)
DEFERROR(Parse, Class_Redefined_Different)
DEFERROR(Parse, Class_BadBase)
DEFERROR(Parse, Class_BadMember)
DEFERROR(Parse, Class_BadModifier)
DEFERROR(Parse, Class_BadParam)
DEFERROR(Parse, Class_Ctor_HasType)
DEFERROR(Parse, Class_Ctor_Static)
DEFERROR(Parse, Class_ExpectBaseOrBody)
DEFERROR(Parse, Class_ExpectBaseName)
DEFERROR(Parse, Class_ExpectMember)
DEFERROR(Parse, Class_ExpectMemberName)
DEFERROR(Parse, Class_ExpectOperator)
DEFERROR(Parse, Class_Method_Modifier)

DEFERROR(Parse, Enum_BadModifier)
DEFERROR(Parse, Enum_Member)
DEFERROR(Parse, Enum_Type)

DEFERROR(Parse, Expect)
DEFERROR(Parse, Expect_BooleanExpr)
DEFERROR(Parse, Expect_LeftValue)
DEFERROR(Parse, ExpectName) // pToken
DEFERROR(Parse, ExpectNamespace)
DEFERROR(Parse, ExpectType)

DEFERROR(Parse, Expr_BadAfterDot)
DEFERROR(Parse, Expr_BadExpr)
DEFERROR(Parse, Expr_EltRef_Invalid)
DEFERROR(Parse, Expr_Name)
DEFERROR(Parse, Expr_New_Invalid)
DEFERROR(Parse, Expr_New_Name)
DEFERROR(Parse, Expr_NewArray_Invalid)
DEFERROR(Parse, Expr_NoBase)
DEFERROR(Parse, Expr_NoThis)

DEFERROR(Parse, Field_Const_NoValue)
DEFERROR(Parse, Field_Modifier_Invalid)
DEFERROR(Parse, Field_Value_Void)

DEFERROR(Parse, Indexer_Accessor_Invalid)
DEFERROR(Parse, Indexer_Member_None)
DEFERROR(Parse, Indexer_Member_Duplicate)
DEFERROR(Parse, Indexer_Member_Extern_Body)
DEFERROR(Parse, Indexer_Member_Interface_Body)
DEFERROR(Parse, Indexer_Member_Invalid)
DEFERROR(Parse, Indexer_Member_Missing_Body)
DEFERROR(Parse, Indexer_Modifier_Invalid)
DEFERROR(Parse, Indexer_Param_Duplicate)
DEFERROR(Parse, Indexer_Param_Invalid)
DEFERROR(Parse, Indexer_Param_Missing)
DEFERROR(Parse, Indexer_Param_Reserved)
DEFERROR(Parse, Indexer_Param_Void)
DEFERROR(Parse, Indexer_Value_Void)

DEFERROR(Parse, Interface_BadModifier)
DEFERROR(Parse, Interface_Ctor)
DEFERROR(Parse, Interface_Field)
DEFERROR(Parse, Interface_Method)
DEFERROR(Parse, Interface_Method_Modifier)
DEFERROR(Parse, Interface_Property)

DEFERROR(Parse, Internal_StackIsEmpty)
DEFERROR(Parse, Internal_StackNotEmpty)
DEFERROR(Parse, Internal_StateInStack)
DEFERROR(Parse, Internal_Unexpected)

DEFERROR(Parse, Label_Multiple)
DEFERROR(Parse, Label_Invalid)
DEFERROR(Parse, Label_NotDefined)
DEFERROR(Parse, Label_NotUsed)

DEFERROR(Parse, Member_Duplicate)

DEFERROR(Parse, Method_Block_Invalid)
DEFERROR(Parse, Method_ExpectBody)
DEFERROR(Parse, Method_Expect_Name)
DEFERROR(Parse, Method_Expect_TypeName)
DEFERROR(Parse, Method_Interface_Multiple)
DEFERROR(Parse, Method_Name_Invalid)
DEFERROR(Parse, Method_NoReturnValue)
DEFERROR(Parse, Method_Param_Duplicate)
DEFERROR(Parse, Method_Param_Invalid)
DEFERROR(Parse, Method_Param_Void)
DEFERROR(Parse, Method_TypeParam_Dup)
DEFERROR(Parse, Method_TypeParam_Invalid)

DEFERROR(Parse, Modifier_Duplicate)

DEFERROR(Parse, Name_Invalid)

DEFERROR(Parse, Namespace_BadDef)
DEFERROR(Parse, Namespace_BadMember)

DEFERROR(Parse, NeedNewModifier)

DEFERROR(Parse, Operator_ParenNotClosed)

DEFERROR(Parse, Property_Accessor_Invalid)
DEFERROR(Parse, Property_Duplicate)
DEFERROR(Parse, Property_Member_None)
DEFERROR(Parse, Property_Member_Duplicate)
DEFERROR(Parse, Property_Member_Extern_Body)
DEFERROR(Parse, Property_Member_Interface_Body)
DEFERROR(Parse, Property_Member_Invalid)
DEFERROR(Parse, Property_Member_Missing_Body)
DEFERROR(Parse, Property_Modifier_Invalid)
DEFERROR(Parse, Property_Value_Void)

DEFERROR(Parse, Stmt_Break)
DEFERROR(Parse, Stmt_Catch_Invalid)
DEFERROR(Parse, Stmt_Continue)

DEFERROR(Parse, Stmt_For_Init)

DEFERROR(Parse, Stmt_Goto)
DEFERROR(Parse, Stmt_Goto_Case)
DEFERROR(Parse, Stmt_Goto_Case_Expr)
DEFERROR(Parse, Stmt_Goto_Default)

DEFERROR(Parse, Stmt_Return_Expr)
DEFERROR(Parse, Stmt_Return_Void)

DEFERROR(Parse, Stmt_Switch_BadCase)
DEFERROR(Parse, Stmt_Switch_FallThrough)
DEFERROR(Parse, Stmt_Switch_MultipleDefault)
DEFERROR(Parse, Stmt_Switch_NotLabel)

DEFERROR(Parse, Stmt_Using_Expr)
DEFERROR(Parse, Stmt_Using_Name)
DEFERROR(Parse, Stmt_Using_Paren)
DEFERROR(Parse, Stmt_Using_Var)
DEFERROR(Parse, Stmt_Using_Void)

DEFERROR(Parse, Stmt_VarDef)

DEFERROR(Parse, Struct_BadModifier)

DEFERROR(Parse, Type_BadArray)
DEFERROR(Parse, Type_DuplicateName)
DEFERROR(Parse, Type_NotType)

DEFERROR(Parse, RedefineOther)
DEFERROR(Parse, RedefineSame)

DEFERROR(Parse, Unexpected_Eof)

DEFERROR(Parse, Using_BadUsing) // pToken
DEFERROR(Parse, UsingName_BadTarget)

DEFERROR(Parse, WarningUsingExisitng)

// Pass
DEFERROR(Pass, Abort)
DEFERROR(Pass, Fail)

// Resolve
DEFERROR(Resolve, NYI)
DEFERROR(Resolve, Unexpected)

DEFERROR(Resolve, ExpectClass) // name_ref
DEFERROR(Resolve, ExpectField)
DEFERROR(Resolve, ExpectInterface)
DEFERROR(Resolve, ExpectMethod)
DEFERROR(Resolve, ExpectNamespaceOrType) // name_ref
DEFERROR(Resolve, ExpectNamespace) // name_ref
DEFERROR(Resolve, ExpectNonGenericClass) // name_ref
DEFERROR(Resolve, ExpectType) // name_ref

DEFERROR(Resolve, Inaccessible) // name_ref

DEFERROR(Resolve, Alias_Conflict) // name
DEFERROR(Resolve, Alias_NotFound) // name

DEFERROR(Resolve, Class_BadConstructedClass) // nameref
DEFERROR(Resolve, Class_Conflict) // class class
DEFERROR(Resolve, Class_CyclicBase) // classDef
DEFERROR(Resolve, Class_MoreThanOneBase) // classDef
DEFERROR(Resolve, Class_NotClass)
DEFERROR(Resolve, Class_NotFound)
DEFERROR(Resolve, Class_NotGenericClass)

DEFERROR(Resolve, Field_InstanceField) // name class
DEFERROR(Resolve, Field_NotFound) // name class
DEFERROR(Resolve, Field_StaticField)

DEFERROR(Resolve, Member_Invalid) // pMember
DEFERROR(Resolve, Member_NotFound) // name type
DEFERROR(Resolve, Member_Static) // pMember

DEFERROR(Resolve, Method_Ambiguous)
DEFERROR(Resolve, Method_InstanceMethod) // method
DEFERROR(Resolve, Method_NamespaceInOperand) // ns
DEFERROR(Resolve, Method_NoApplicableMethod) // mtg funty
DEFERROR(Resolve, Method_StaticMethod) // method

DEFERROR(Resolve, Name_Ambiguous)
DEFERROR(Resolve, Name_Inaccessible) // NameRef
DEFERROR(Resolve, Name_Invalid) // NameRef
DEFERROR(Resolve, Name_IsArrayType) // Name
DEFERROR(Resolve, Name_IsClass) // Name
DEFERROR(Resolve, Name_IsGenericClass) // Name
DEFERROR(Resolve, Name_IsMethod) // Name
DEFERROR(Resolve, Name_IsNamespace) // Name
DEFERROR(Resolve, Name_IsType)
DEFERROR(Resolve, Name_NotFound) // Name
DEFERROR(Resolve, Name_NotClassOrNamespace) // Name

DEFERROR(Resolve, Namespace_Conflict) // namespace) namespace
DEFERROR(Resolve, Namespace_NotFound) // NamespaceDef
DEFERROR(Resolve, Namespace_NotNamespace) // NamespaceDef

DEFERROR(Resolve, Property_IsInstance)
DEFERROR(Resolve, Property_IsStatic)
DEFERROR(Resolve, Property_NoGetter)
DEFERROR(Resolve, Property_NoSetter)
DEFERROR(Resolve, Property_NotFound)

DEFERROR(Resolve, Type_Conflict) // Name ExpectedType ActualType
DEFERROR(Resolve, Type_Mismatch) // Type1 Type2
DEFERROR(Resolve, Type_NotFound) // NameRef
DEFERROR(Resolve, Type_NotType) // NameRef
DEFERROR(Resolve, Type_Unbound) // NameRef
DEFERROR(Resolve, Type_Unexpected)

DEFERROR(Resolve, UsingName_BadTarget) // NameRef) NameDef
DEFERROR(Resolve, UsingName_NotFound) // NameRef

#undef DEFERROR
