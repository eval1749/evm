// @(#)$Id$
//
// Evita Il - IR - Operands - MethodGroupRef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_MethodGroupRef_h)
#define INCLUDE_Il_Ir_MethodGroupRef_h

#include "./Immediate.h"

namespace Il {
namespace Ir {

class MethodGroupRef :
        public Operand_<MethodGroupRef, Immediate> {

    CASTABLE_CLASS(MethodGroupRef)

    private: Class* const m_pClass;
    private: Name* const m_pName;

    // ctor
    public: MethodGroupRef(
        Class* const pClass,
        const Name& name);

    // [E]
    public: virtual bool Equals(const Operand&) const override;

    // [G]
    public: Class* GetClass() const { return m_pClass; }
    public: Name* name() const { return m_pName; }

    DISALLOW_COPY_AND_ASSIGN(MethodGroupRef);
}; // MethodGroupRef

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_MethodGroupRef_h)
