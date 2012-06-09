// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_MethodRef_h)
#define INCLUDE_Il_Ir_MethodRef_h

#include "./Operand.h"

namespace Il {
namespace Ir {

class Class;

class MethodRef :
    public Operand_<MethodRef> {

    CASTABLE_CLASS(MethodRef)

    private: int const modifiers_;
    private: FunctionType* const m_pFunctionType;
    private: MethodGroup* const m_pMethodGroup;

    public: MethodRef(
        MethodGroup* const pMethodGroup,
        int const iModifiers,
        FunctionType* const pFunctionType,
        const SourceInfo* const pSourceInfo = nullptr);

    // [G]
    public: Class* GetClass() const;
    public: FunctionType* GetFunctionType() const { return m_pFunctionType; }
    public: MethodGroup* GetMethodGroup() const { return m_pMethodGroup; }
    public: int GetModifiers() const { return modifiers_; }
    public: Name* name() const;
    public: Namespace* GetNamespace() const;

    DISALLOW_COPY_AND_ASSIGN(MethodRef);
}; // MethodRef

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_MethodRef_h)
