// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Defs_h)
#define INCLUDE_Compiler_Defs_h

#include "../Common/CommonDefs.h"
using namespace Common;

#include "../Common.h"
#include "../Il.h"

#if 0
namespace Il {
namespace Ir {

#define DEFINSTRUCTION(mp_op, mp_arity, mp_family, mp_desc) \
    class mp_op ## I;

#include "../Il/Ir/Instructions/instructions.inc"
} // Ir
} // Il
#endif

namespace Compiler {

#if 0
using Il::Ir::BBlock;
using Il::Ir::BoolOutput;
using Il::Ir::Class;
using Il::Ir::Instruction;
using Il::Ir::Literal;
using Il::Ir::Module;

using Il::Ir::Modifier;
    using Il::Ir::Modifier_Abstract;
    using Il::Ir::Modifier_Const;
    using Il::Ir::Modifier_Extern;
    using Il::Ir::Modifier_Internal;
    using Il::Ir::Modifier_New;
    using Il::Ir::Modifier_Override;
    using Il::Ir::Modifier_Private;
    using Il::Ir::Modifier_Protected;
    using Il::Ir::Modifier_Public;
    using Il::Ir::Modifier_ReadOnly;
    using Il::Ir::Modifier_Final;
    using Il::Ir::Modifier_Static;
    using Il::Ir::Modifier_Virtual;
    using Il::Ir::Modifier_Volatile;

using Il::Ir::NameRef;
using Il::Ir::Namespace;
using Il::Ir::Output;
using Il::Ir::SourceInfo;
using Il::Ir::Name;
using Il::Ir::Register;
using Il::Ir::Type;
using Il::Ir::Variable;
using Il::Ir::Void;
#else
    using namespace Il::Ir;
#endif

#define DEFTYPE(mp_nc, mp_mod, mp_ns, mp_name, ...) \
    using Il::Ir::Q_ ## mp_name; \
    using Il::Ir::Ty_ ## mp_name;

#include "../Il/Ir/Types.inc"

#define DEFINSTRUCTION(mp_op, mp_arity, mp_family, mp_desc) \
    using Il::Ir::mp_op ## I;

#include "../Il/Ir/Instructions/instructions.inc"

class CompileSession;
class Token;

} // Compiler

#include "./Compiler.h"
#include "./CompilerGlobal.h"
#include "./Functor.h"
#include "./IrDefs.h"

#endif // !defined(INCLUDE_Compiler_Defs_h)
