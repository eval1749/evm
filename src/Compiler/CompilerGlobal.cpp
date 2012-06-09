#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - CompilerInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CompilerGlobal.h"

#include "../Common/Char.h"
#include "../Common/GlobalMemoryZone.h"
#include "../Common/Int32.h"

#include "./Ir.h"

#include "./Syntax/Keyword.h"
#include "./Syntax/Operator.h"

#include "./Tasks/MethodResolver.h"

#include "../Il/Ir/Instructions.h"
#include "../Il/Ir/Name.h"
#include "../Il/Ir/Operands.h"

namespace Common {
void Init();
} // Common

namespace Compiler {

#define DEFKEYWORD(mp_name) \
    DEFSYMBOL(mp_name) \
    Keyword* K_ ## mp_name;

#define DEFOPERATOR(mp_name, mp_str, mp_cat) \
    DEFSYMBOL(mp_name) \
    Operator* Op__ ## mp_name;

#define DEFOPERATOR_ARITH(mp_name, mp_str, mp_cat, mp_op) \
    DEFOPERATOR(mp_name, mp_str, mp_cat);

#define DEFOPERATOR_COMP(mp_name, mp_str, mp_cat, mp_op) \
    DEFOPERATOR(mp_name, mp_str, mp_cat);

#define DEFSYMBOL(mp_name) const Name* Q_ ## mp_name;

#include "./Syntax/keywords.inc"
#include "./Syntax/operators.inc"
#include "./Syntax/symbols.inc"

// [I]
void CompilerGlobal::Init() {
    Common::Init();
    Il::Init();

    #define DEFOPERATOR(mp_name, mp_str, mp_cat) \
        Q_ ## mp_name = &Name::Intern(mp_str);

    #define DEFOPERATOR_ARITH(mp_name, mp_str, mp_cat, mp_op) \
        Q_ ## mp_name = &Name::Intern(mp_str);

    #define DEFKEYWORD(mp_name) \
        DEFSYMBOL(mp_name)

    #define DEFSYMBOL(mp_name) \
        Q_ ## mp_name = &Name::Intern(# mp_name);

    #include "./Syntax/keywords.inc"
    #include "./Syntax/operators.inc"
    #include "./Syntax/symbols.inc"

    Keyword::Init();
    Operator::Init();
    MethodResolver::Init();
}

} // Compiler

