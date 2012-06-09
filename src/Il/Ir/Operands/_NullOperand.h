// @(#)$Id$
//
// Evita Compiler - NullOperand
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_NullOperand_h)
#define INCLUDE_Il_Ir_NullOperand_h

#include "./Immediate.h"

#include "../../../Common/Collections.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

/// <summary>
///   Class of singleton object Null. Typed null is represent with RuntimCast
//    instruction in Parse pass.
/// </summary>
class NullOperand : public Operand_<NullOperand, Immediate> {
    public: static const char* Kind_() { return "NullOperand"; }

    private: static HashMap_<PtrKey_<Type>, NullOperand*>* s_pHashMap;
    private: Type* const m_pType;

    // ctor
    private: NullOperand(Type* const pType);

    // [G]
    public: override Type* GetTy() const;

    // [I]
    public: static NullOperand* Intern(Type* const pType);
}; // NullOperand

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_NullOperand_h)
