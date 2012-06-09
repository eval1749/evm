// -*- Mode: C++; -*-
// @(#)$Id$
//
// Vm Error Codes
//

#if !defined(DEFERROR)
    #error "You must define DEFERROR."
#endif !defined(DEFERROR)

// VerifyTask
DEFERROR(BBlock, Empty)

// Compiler
DEFERROR(Compiler, InternalError)

// Fasl
DEFERROR(Fasl, Error)

DEFERROR(Optimize, InternalError)

// VerifyTask, X86AsmTask
DEFERROR(Instruction, Invalid)

// ResolveMethodCallTask
DEFERROR(Method, NoApplicableMethod)

DEFERROR(Pass, UnexpectedInstruction)

DEFERROR(Type, NotMatched)

#undef DEFERROR
