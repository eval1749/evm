// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Register_h)
#define INCLUDE_Il_Ir_Register_h

#include "./SsaOutput.h"

namespace Il {
namespace Ir {

/// <summary>
///   Nameic register of intermediate representation of program.
/// </summary>
class Register :
        public Operand_<Register, SsaOutput>,
        public Extendable_<Register>,
        public DoubleLinkedItem_<Register>,
        public WorkListItem_<Register> {

    CASTABLE_CLASS(Register)

    private: RegClass const  m_eRegClass;
    public:  Physical*  m_pPhysical;
    public:  StackSlot* m_pSpill;
    private: const Variable* m_pVariable;

    // for ctor
    friend class Module;

    // ctor
    protected: Register(
        RegClass const eClass = RegClass_Gpr,
        const Variable* const pVar = nullptr);

    // [G]
    public: RegClass GetRegClass() const { return m_eRegClass; }

    public: Variable* GetVariable() const {
      return const_cast<Variable*>(m_pVariable);
    }

    // [S]
    public: void SetVariable(const Variable&);

    DISALLOW_COPY_AND_ASSIGN(Register);
}; // Register


template<class TSelf, class TParent = Register>
class Register_ :
    public WithCastable1_<TSelf, TParent, RegClass> {

    private: typedef WithCastable1_<
        TSelf,
        TParent,
        RegClass> DirectBase;

    protected: typedef Register_<TSelf, TParent> Base;

    protected: Register_(RegClass const eRegClass) :
        DirectBase(eRegClass) {}

    public: virtual void Apply(Functor* const pFunctor) override {
      pFunctor->Process(static_cast<TSelf*>(this));
    } // Apply

    DISALLOW_COPY_AND_ASSIGN(Register_);
}; // Register_

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Register_h)
