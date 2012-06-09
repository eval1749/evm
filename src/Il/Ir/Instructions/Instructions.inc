// -*- Mode: C++; -*-
// @(#)$Id$
//
// IR Instructions
//
#if !defined(DEFINSTRUCTION)
    #error "We must define DEFINSTRUCTION."
#endif // !defined(DEFINSTRUCTION)

// [A]
DEFINSTRUCTION(Add, 2, Arithmetic, "ADD ty %rd <= %sx %sy")

// [B]
DEFINSTRUCTION(Box, 1, General, "BOX ty %rd <= %sx")
DEFINSTRUCTION(Branch, 3, Last, "BRANCH %bx labely labelz")

// [C]
DEFINSTRUCTION(Call, 2, General, "CALL ty %d <= %sx %vy")
DEFINSTRUCTION(Catch, 3, General, "CATCH %fx type label")
DEFINSTRUCTION(Close, 1, General, "CLOSE %rx")
DEFINSTRUCTION(Closure, 2, General, "CLOSURE ty %fd <= %sx %vy")
//DEFINSTRUCTION(Convert, General, "CONVERT ty %rd <= %rx") // for function float

// [D]
DEFINSTRUCTION(Div, 2, Arithmetic, "DIV ty %rd <= %sx %sy")

// [E]
DEFINSTRUCTION(EltRef, 2, General, "ELTREF ty* %rd <= %rx %sy")
DEFINSTRUCTION(Entry, 0, General, "ENTRY")
DEFINSTRUCTION(Eq, 2, Relational, "EQ %bd <= %sx %sy")
DEFINSTRUCTION(Exit, 0, Last, "EXIT")

// [F]
DEFINSTRUCTION(FieldPtr, 2, General, "FIELDPTR ty %rd <= %rx field")
//DEFINSTRUCTION(Frame, 1, General, "FRAME ty %rd <= %rx")

// [G]
DEFINSTRUCTION(Ge, 2, Relational, "GE %bd <= %sx %sy")
//DEFINSTRUCTION(Go, 1, General, "GO %rx")
DEFINSTRUCTION(Gt, 2, Relational, "GT %bd <= %sx %sy")

// [I]
DEFINSTRUCTION(If, 3, General, "IF ty %rd <= %bx %sy %sz")

// [J]
DEFINSTRUCTION(Jump, 1, Last, "JUMP label")

// [L]
DEFINSTRUCTION(Le, 2, Relational, "LE %bd <= %sx %sy")
DEFINSTRUCTION(Load, 1, General, "LOAD ty %rd <= %rx")
//DEFINSTRUCTION(LoadFun, General, "LOADFUN ty %rd <= fn")
DEFINSTRUCTION(LogAnd, 2, Arithmetic, "LOGAND ty %rd <= %sx %sy")
DEFINSTRUCTION(LogEqv, 2, Arithmetic, "LOGEQV ty %rd <= %sx %sy")
DEFINSTRUCTION(LogIor, 2, Arithmetic, "LOGIOR ty %rd <= %sx %sy")
DEFINSTRUCTION(LogXor, 2, Arithmetic, "LOGXOR ty %rd <= %sx %sy")
DEFINSTRUCTION(Lt, 2, Relational, "LT %bd <= %sx %sy")

// [M]
DEFINSTRUCTION(Mul, 2, Arithmetic, "MUL ty %rd <= %sx %sy")
DEFINSTRUCTION(MvRestore, 1, General, "MVRESTORE %vd <= %sx")
DEFINSTRUCTION(MvSave, 1, General, "MVSAVE %sd <= %vx")

// [N]
DEFINSTRUCTION(NameRef, 2, General, "NAMEREF ty %rd <= %sx %nameref") // for Parser
DEFINSTRUCTION(Ne, 2, Relational, "NE %bd <= %sx %sy")
DEFINSTRUCTION(New, 0, General, "NEW ty %rd <=")
DEFINSTRUCTION(NewArray, 1, General, "NEWARRAY ty %rd <= %vx")
DEFINSTRUCTION(NonLocal, 0, General, "NONLOCAL %rd <=")
DEFINSTRUCTION(Null, 0, General, "NULL ty %rd")

// [O]
DEFINSTRUCTION(OpenCatch, 0, Open, "OPENCATCH %fd <=")
DEFINSTRUCTION(OpenFinally, 2, Open, "OPENFINALLY %fd <= %fn %vy")

// [P]
DEFINSTRUCTION(Phi, MoreThanZero, General, "PHI ty %d <= (label %sx)...")
DEFINSTRUCTION(Prologue, 0, General, "PROLOGUE ty %vd <=")

// [R]
DEFINSTRUCTION(Rem, 2, Arithmetic, "REM ty %rd <= %sx %sy")
DEFINSTRUCTION(Ret, 1, Terminate, "RET %sx")
//DEFINSTRUCTION(ReturnFrom, General, "RETURNFROM %rx %vy")
DEFINSTRUCTION(RuntimeCast, 1, Cast, "RUNTIMECAST ty %rd <= %rx")

// [S]
DEFINSTRUCTION(Select, 2, General, "SELECT ty %rd <= %vx nth")
DEFINSTRUCTION(Shl, 2, Arithmetic, "SHL ty %rd <= %sx %sy")
//DEFINSTRUCTION(ShouldBe, General, "SHOULDBE ty %rd <= %sx")
DEFINSTRUCTION(Shr, 2, Arithmetic, "SHR ty %rd <= %sx %sy")
DEFINSTRUCTION(StaticCast, 1, Cast, "STATICCAST ty %rd <= %rx")
DEFINSTRUCTION(Store, 2, General, "STORE %rx %sy")
DEFINSTRUCTION(Sub, 2, Arithmetic, "SUB ty %rd <= %sx %sy")
DEFINSTRUCTION(Switch, MoreThanZero, Last, "SWITCH %sx label (label lit)...")

// [T]
//DEFINSTRUCTION(TagDef, 2, General, "TAGDEF ty %rd <= %fx label")
DEFINSTRUCTION(Throw, 1, Last, "THROW %sx")
//DEFINSTRUCTION(Tlv, 1, General, "TLV ty %rd <= tlvrec")
//DEFINSTRUCTION(Typep, 2, General, "TYPEP bool %bd <= %sx type")

// [U]
DEFINSTRUCTION(UnBox, 1, General, "UNBOX ty %rd <= %sx")
//DEFINSTRUCTION(Unreachable, 0, Last, "UNREACHABLE")
DEFINSTRUCTION(UpVarBase, 1, General, "UPVARBASE %rd <= fn")
DEFINSTRUCTION(UpVarDef, 1, General, "UPVARDEF ty %rd <= var")
DEFINSTRUCTION(Use, 1, General, "USE %rx")

// [V]
DEFINSTRUCTION(Values, Any, General, "VALUES ty %vd <= %sx...")
DEFINSTRUCTION(VarDef, 2, General, "VARDEF ty %rd <= var %sy")

#if defined(NEED_ALL_OPCODES)
 #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
   DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format)

 #define DEFINE_TARGET_INSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
   DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format)
#endif

#if defined(DEFCGINSTRUCTION)
  #include "../../../Cg/Instructions/CgInstructions.inc"
#endif

#if defined(DEFINE_TARGET_INSTRUCTION)
  #include "../../../Targets/TargetInstructions.inc"
#endif

#undef DEFCGINSTRUCTION
#undef DEFINSTRUCTION
#undef DEFINE_TARGET_INSTRUCTION
#undef NEED_ALL_OPCODES
