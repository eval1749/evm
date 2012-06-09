// -*- Mode: C++) -*-
// @(#)$Id$
//
// Evita VM IL IR Symbols
//
#if !defined(DEFSYMBOL)
    #error "You must define DEFSYMBOL macro."
#endif

#if !defined(DEFOPERATORSYMBOL)
    #define DEFOPERATORSYMBOL(name, text) DEFSYMBOL(operator_ ## name)
#endif

DEFSYMBOL(Dispose)       // Disposable
DEFSYMBOL(GetEnumerator) // Enumerable
DEFSYMBOL(get_Current)   // Enuerator
DEFSYMBOL(get_Length)    // Array, String
DEFSYMBOL(get_Item)      // Indexer
DEFSYMBOL(HasMore)       // Enuerator
DEFSYMBOL(Item)          // Indexer
DEFSYMBOL(MoveNext)      // Enuerator
DEFSYMBOL(set_Item)      // Indexer
DEFSYMBOL(value)         // ClosedCell<T>

DEFOPERATORSYMBOL(Add, "+")
DEFOPERATORSYMBOL(AddAssign, "+=")
DEFOPERATORSYMBOL(Bracket, "[]")
DEFOPERATORSYMBOL(CondAnd, "&&")
DEFOPERATORSYMBOL(CondNot, "!")
DEFOPERATORSYMBOL(CondOr, "||")
DEFOPERATORSYMBOL(Decrement, "--")
DEFOPERATORSYMBOL(Equal, "==")
DEFOPERATORSYMBOL(Increment, "++")
DEFOPERATORSYMBOL(Divide, "/")
DEFOPERATORSYMBOL(DivideAssign, "/=")
DEFOPERATORSYMBOL(GreaterThan, ">")
DEFOPERATORSYMBOL(GreaterThanOrEqual, ">=")
DEFOPERATORSYMBOL(LessThan, "<")
DEFOPERATORSYMBOL(LessThanOrEqual, "<=")
DEFOPERATORSYMBOL(LogAnd, "&")
DEFOPERATORSYMBOL(LogAndAssign, "&=")
DEFOPERATORSYMBOL(LogIor, "|")
DEFOPERATORSYMBOL(LogIorAssign, "|=")
DEFOPERATORSYMBOL(LogNot, "~")
DEFOPERATORSYMBOL(LogXor, "^")
DEFOPERATORSYMBOL(LogXorAssign, "^=")
DEFOPERATORSYMBOL(Subtract, "-")
DEFOPERATORSYMBOL(SubtractAssign, "-=")
DEFOPERATORSYMBOL(Multiply, "*")
DEFOPERATORSYMBOL(MultiplyAssign, "*=")
DEFOPERATORSYMBOL(NotEqual, "!=")
DEFOPERATORSYMBOL(Remainder, "%")
DEFOPERATORSYMBOL(RemainderAssign, "%=")
DEFOPERATORSYMBOL(ShiftLeft, "<<")
DEFOPERATORSYMBOL(ShiftLeftAssign, "<<=")
DEFOPERATORSYMBOL(ShiftRight, ">>")
DEFOPERATORSYMBOL(ShiftRightAssign, ">>=")

#undef DEFSYMBOL
#undef DEFOPERATORSYMBOL
