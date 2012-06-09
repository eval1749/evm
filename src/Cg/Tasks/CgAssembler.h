// @(#)$Id$
//
// Evita Ee - Cg
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_CgAsm_h)
#define INCLUDE_Il_Cg_CgAsm_h

#include "../../Ee/CodeDesc.h"

namespace Il {
namespace Cg {

typedef Common::LocalMemoryZone Mm;
typedef Common::Allocable LocalObject;
typedef Common::Object* Val;

class Assembler : protected LocalMemoryZone {
    protected: struct Annot :
            public DoubleLinkedItem_<Annot>,
            public LocalObject {

        public: enum Kind {
            Kind_None,

            Kind_AbsLabel,
            Kind_Callee,
            Kind_ClosedLit,
            Kind_ClosedVar,
            Kind_FunRef,
            Kind_Literal,
            Kind_LocalCallee,
            Kind_Method,
            Kind_MethodRef,
            Kind_RelLabel,
            Kind_SymSetf,
            Kind_SymFun,
            Kind_SymVal,
            Kind_TlvOfs,

            Kind_Limit,
        }; // AnnotKind

        Kind m_eKind;
        int m_nAddr;
        int m_ofs;
        intptr_t m_value;

        // ctor
        Annot(
            Kind const eKind,
            int const iAddr,
            int const ofs,
            intptr_t const val) :
                m_eKind(eKind),
                m_nAddr(iAddr),
                m_ofs(ofs),
                m_value(val) {}

        // [G]
        int GetAddr()   const { return m_nAddr; }
        Kind GetKind()   const { return m_eKind; }
        int GetOffset() const { return m_ofs; }
    }; // Annot

    protected: typedef DoubleLinkedList_<Annot> Annots;

    protected: class BBlockExt : public LocalObject {
        public: int m_nAddr;
        public: int m_ofs;

        public: int GetAddr()   const { return m_nAddr; }
        public: int GetOffset() const { return m_ofs; }
    }; // BBlockExt

    protected: class Span :
        public Castable_<Span>,
        public DoubleLinkedItem_<Span>,
        public LocalObject,
        public WorkListItem_<Span> {

        public:  int m_nAddr;
        public:  int m_ofs;

        // ctor
        protected: Span() {}

        // [G]
        public: int GetAddr() const { return m_nAddr; }
        public: int GetOffset() const { return m_ofs; }

        // [I]
        public: bool IsCrossing(int nAddr) { 
          return this->GetAddr() >= nAddr; 
        }

        DISALLOW_COPY_AND_ASSIGN(Span);
    }; // Span

    protected: typedef DoubleLinkedList_<Span> Spans;

    protected: template<class T, class B = Span> class Span_ 
        : public WithCastable_<T, B> {

        protected: typedef Span_<T, B> Base;

        protected: Span_(int iAddr, int ofs) {
            m_nAddr = iAddr;
            m_ofs   = ofs;
        } // Span_

        DISALLOW_COPY_AND_ASSIGN(Span_);
    }; // Span_

    protected: struct FunExt : public LocalObject {
        Annots m_oAnnots;
    }; // FunExt

    // Member variables
    protected: Il::Ir::Function* m_pFun;

    protected: Assembler() {}


    protected: static intptr_t FixnumEncode(const void* const pv) {
        return reinterpret_cast<intptr_t>(pv);
    } // FixnumEncode

    protected: static intptr_t FixnumEncode(int const iValue) {
        return iValue;
    } // FixnumEncode

    protected: static bool isS8(int const i) {
        return i >= -128 && i <= 127;
    } // isS8

    DISALLOW_COPY_AND_ASSIGN(Assembler);
}; // Assembler

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_CgAsm_h)
