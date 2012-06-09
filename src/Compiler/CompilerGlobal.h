// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_Compiler_CompilerGlobal_h)
#define INCLUDE_evc_Compiler_CompilerGlobal_h

namespace Compiler {

class ClassDef;
class CastOperator; // Token
class NamespaceDef;
class Keyword; // Token
class Operator; // Token
class TypeArgList; // Token
class TypeDef;

class CompilerGlobal {
    public: static void LibExport Init();
}; // CompilerGlobal

#define DEFKEYWORD(mp_name) \
    DEFSYMBOL(mp_name) \
    extern Keyword* K_ ## mp_name;

#define DEFOPERATOR(mp_name, mp_str, mp_cat) \
    DEFSYMBOL(mp_name) \
    extern Operator* Op__ ## mp_name;

#define DEFTYPE(mp_name, mp_Name) \
    DEFKEYWORD(mp_name) \
    extern ClassDef* TypeDef_ ## mp_Name;

#define DEFSYMBOL(mp_name) \
    extern const Name* Q_ ## mp_name;

#include "./Syntax/keywords.inc"
#include "./Syntax/operators.inc"
#include "./Syntax/symbols.inc"

} // Compiler

#endif // !defined(INCLUDE_evc_Compiler_CompilerGlobal_h)
