// -*- Mode: C++ -*-
// @(#)$Id$
//
// Evita Compiler - Operators
//

#if !defined(DEFOPERATOR_ARITH)
    #define DEFOPERATOR_ARITH(mp_name, mp_str, mp_cat, mp_op) \
        DEFOPERATOR(mp_name, mp_str, mp_cat)
#endif // !defined(DEFOPERATOR_ARITH)

#if !defined(DEFOPERATOR_ASSIGN)
  #define DEFOPERATOR_ASSIGN(mp_name, mp_str, mp_binop) \
    DEFOPERATOR(mp_name, mp_str, AddI)
#endif // !defined(DEFOPERATOR_ARITH)

#if !defined(DEFOPERATOR_COMP)
    #define DEFOPERATOR_COMP(mp_name, mp_str, mp_cat, mp_op) \
        DEFOPERATOR_ARITH(mp_name, mp_str, mp_cat, mp_op)
#endif // !defined(DEFOPERATOR_COMP)

// C#4.0
// 7.6 Primary
// 7.7 Unary
// 7.8 Mul
// 7.8 Add
// 7.9 Shift
// 7.10 Relational and type testing
// 7.10 Equality
// 7.11 Logical AND
// 7.11 Logical XOR
// 7.11 Logical OR
// 7.12 Conditional AND
// 7.12 Conditional OR
// 7.13 Null-Coalescing
// 7.14 Conditional
// 7.15 Assign

// [A]
DEFOPERATOR_ARITH(  Add,            "+",    Add, AddI)
DEFOPERATOR_ASSIGN(AddEq, "+=", "+")
DEFOPERATOR(        Add1,           "++",   Unary)
DEFOPERATOR_ARITH(  LogAnd,         "&",    LogAnd, LogAndI)
DEFOPERATOR(        CondAnd,        "&&",   CondAnd)
DEFOPERATOR_ASSIGN(AndEq, "&=", "&")
DEFOPERATOR(        Assign,         "=",    Assign)

// [C]
DEFOPERATOR(        CloseBrace,     "}", None)
DEFOPERATOR(        CloseBracket,   "]", None)
DEFOPERATOR(        CloseParen,     ")", None)
DEFOPERATOR(        Colon,          ":", None)
DEFOPERATOR(        Comma,          ",", None)

// [D]
DEFOPERATOR_ARITH(  Div,            "/",    Mul, DivI)
DEFOPERATOR_ASSIGN(DivEq, "/=", "/")
DEFOPERATOR(        Dot,            ".",    None)

// [E]
DEFOPERATOR_COMP(   Eq,             "==", Equality, EqI)

// [G]
DEFOPERATOR_COMP(   Ge,             ">=",   Relational, GeI)
DEFOPERATOR_COMP(   Gt,             ">",    Relational, GtI)

// [L]
DEFOPERATOR_ARITH(  LogIor,         "|",    LogIor, LogIorI)
DEFOPERATOR_ASSIGN(LogIorEq, "|=", "|")
DEFOPERATOR(        LogNot,         "~",    None)
DEFOPERATOR_ARITH(  LogXor,         "^",    LogXor, LogXorI)
DEFOPERATOR_ASSIGN(LogXorEq, "^=", "^")
DEFOPERATOR_COMP(   Le,             "<=",   Relational, LeI)
DEFOPERATOR_COMP(   Lt,             "<",    Relational, LtI)

// [M]
DEFOPERATOR_ARITH(  Mul,            "*",    Mul, MulI)
DEFOPERATOR_ASSIGN(MulEq, "*=", "*")

// [N]
DEFOPERATOR(        Not,            "!",    None)
DEFOPERATOR_COMP(   Ne,             "!=",   Equality, NeI)

// [O]
DEFOPERATOR(        OpenBrace,      "{",    None)
DEFOPERATOR(        OpenBracket,    "[",    None)
DEFOPERATOR(        OpenParen,      "(",    None)
DEFOPERATOR(        CondOr,         "||",   CondOr)

// [Q]
DEFOPERATOR(        Question,       "?",    Cond)
DEFOPERATOR(        Question2,      "??",   NullOr) // NullCoalescing

//[R]
DEFOPERATOR_ARITH(  Rem,            "%",    Mul, RemI)
DEFOPERATOR_ASSIGN(RemEq, "%=", "%")

// [S]
DEFOPERATOR(        SemiColon,      ";",    None)
DEFOPERATOR_ARITH(  Shl,            "<<",   Shift, ShlI)
DEFOPERATOR_ASSIGN(ShlEq, "<<=", "<<")
DEFOPERATOR_ARITH(  Shr,            ">>",   Shift, ShrI)
DEFOPERATOR_ASSIGN(ShrEq, ">>=", ">>")
DEFOPERATOR_ARITH(  Sub,            "-",    Add, SubI)
DEFOPERATOR_ASSIGN(SubEq, "-=", "-")
DEFOPERATOR(        Sub1,           "--",   Unary)

#undef DEFOPERATOR
#undef DEFOPERATOR_ARITH
#undef DEFOPERATOR_ASSIGN
#undef DEFOPERATOR_COMP
