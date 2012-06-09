// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_h)
#define INCLUDE_Il_Ir_Instructions_h

// [A]
#include "./Instructions/AddI.h"
#include "./Instructions/ArithmeticInstruction.h"

// [B]
#include "./Instructions/BoxI.h"
#include "./Instructions/BranchI.h"

// [C]
#include "./Instructions/CallI.h"
#include "./Instructions/CastInstruction.h"
#include "./Instructions/CatchI.h"
#include "./Instructions/CloseI.h"
#include "./Instructions/ClosureI.h"

// [D]
#include "./Instructions/DivI.h"

// [E]
#include "./Instructions/EltRefI.h"
#include "./Instructions/EntryI.h"
#include "./Instructions/EqI.h"
#include "./Instructions/ExitI.h"

// [G]
#include "./Instructions/GeI.h"
#include "./Instructions/GeneralInstruction.h"
#include "./Instructions/GtI.h"

// [F]
#include "./Instructions/FieldPtrI.h"
#include "./Instructions/FunctionOperandBox.h"

// [I]
#include "./Instructions/IfI.h"

// [J]
#include "./Instructions/JumpI.h"

// [L]
#include "./Instructions/LoadI.h"
#include "./Instructions/LogAndI.h"
#include "./Instructions/LogEqvI.h"
#include "./Instructions/LogIorI.h"
#include "./Instructions/LogXorI.h"
#include "./Instructions/LeI.h"
#include "./Instructions/LtI.h"

// [M]
#include "./Instructions/MulI.h"
#include "./Instructions/MvRestoreI.h"
#include "./Instructions/MvSaveI.h"

// [N]
#include "./Instructions/NameRefI.h"
#include "./Instructions/NeI.h"
#include "./Instructions/NewI.h"
#include "./Instructions/NewArrayI.h"
#include "./Instructions/NonLocalI.h"
#include "./Instructions/NullI.h"

// [O]
#include "./Instructions/OpenCatchI.h"
#include "./Instructions/OpenFinallyI.h"

// [P]
#include "./Instructions/PhiI.h"
#include "./Instructions/PhiOperandBox.h"
#include "./Instructions/PrologueI.h"

// [R]
#include "./Instructions/RelationalInstruction.h"
#include "./Instructions/RemI.h"
#include "./Instructions/RetI.h"
#include "./Instructions/RuntimeCastI.h"

// [S]
#include "./Instructions/ShlI.h"
#include "./Instructions/ShrI.h"
#include "./Instructions/SelectI.h"
#include "./Instructions/StaticCastI.h"
#include "./Instructions/StoreI.h"
#include "./Instructions/SubI.h"
#include "./Instructions/SwitchI.h"
#include "./Instructions/SwitchOperandBox.h"

// [T]
#include "./Instructions/ThrowI.h"

// [U]
#include "./Instructions/UnBoxI.h"
#include "./Instructions/UpVarBaseI.h"
#include "./Instructions/UpVarDefI.h"

// Compiler and IR optimizer don't emit USE instruction, but OPEN*
// instructions refer in IsUseless method.
#include "./Instructions/UseI.h"

// [V]
#include "./Instructions/ValuesI.h"
#include "./Instructions/VarDefI.h"


#endif // !defined(INCLUDE_Il_Ir_Instructions_h)
