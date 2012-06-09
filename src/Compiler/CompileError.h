// @(#)$Id$
//
// Evita Parser
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_CompileError_Parse_h)
#define INCLUDE_evc_CompileError_Parse_h

#include "../Il/VmError.h"

namespace Compiler {

using namespace Il;

enum CompileError {
    CompileError_NoError,

    CompileError_Base = VmError_Max_1 - 1,

    #define DEFERROR(mp_cat, mp_name) \
        CompileError_ ## mp_cat ## _ ## mp_name,

    #include "./CompileError.inc"

    CompileError_Max_1,
    CompileError_Min = CompileError_Base + 1,
}; // CompileError

} // Compiler

#endif // !defined(INCLUDE_evc_CompileError_Parse_h)
