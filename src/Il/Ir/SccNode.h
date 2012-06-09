// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_IrSccNode_h)
#define INCLUDE_Il_IrSccNode_h

namespace Il {
namespace Ir {

template<typename Node_, typename TParent>
class SccNode_ :
        public DoubleLinkedItem_<Node_, Scc> {

    public: typedef DoubleLinkedList_<Node_, Scc> List;

    public: uint    m_nSccNum;
    public: Node_*  m_pSccId;
    public: Node_*  m_pSccNext;

    public: SccNode_()
        { ResetScc(); }

    // [E]
    public: class EnumSccMember
    {
        private: Node_* m_pRunner;

        public: EnumSccMember(Node_* p) :
            m_pRunner(p) {}

        public: bool AtEnd() const
            { return nullptr == m_pRunner; }

        public: Node_* Get() const
            { ASSERT(!AtEnd()); return m_pRunner; }

        public: void Next()
        {
            ASSERT(!AtEnd());
            m_pRunner = m_pRunner->GetSccNode()->m_pSccNext;
        } // Next
    }; // EnumSccMember

    // [I]
    private: bool IsSccLeader() const
        { return m_pSccId == this; }

    // [R]
    public: void ResetScc(Node_* pSccId = nullptr)
    {
        m_nSccNum  = 0;
        m_pSccId   = pSccId;
        m_pSccNext = nullptr;
    } // ResetScc
}; // SccNode_

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_IrSccNode_h)
