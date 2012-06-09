// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_IrGraph_h)
#define INCLUDE_Il_IrGraph_h

#include "./Edge.h"
#include "./SccNode.h"

namespace Il {
namespace Ir {

template<typename Node_, typename TParent>
class Graph_ :
    public ChildList_<Node_, TParent>,
    public DoubleLinkedList_<Node_, Preorder>,
    public DoubleLinkedList_<Node_, Postorder>,
    public SccNode_<Node_, Graph_<Node_, TParent>>::List
{
    protected: typedef Graph_<Node_, TParent>              Graph;
    protected: typedef ChildList_<Node_, TParent>          LayoutList;
    protected: typedef DoubleLinkedList_<Node_, Preorder>  PreorderList;
    protected: typedef DoubleLinkedList_<Node_, Postorder> PostorderList;
    public:    typedef typename SccNode_<Node_, Graph>::List SccList;

    public: enum Cache
    {
        Cache_Dfs       = 1 << 0,   // 1
        Cache_Scc       = 1 << 1,   // 2
        Cache_Dom       = 1 << 2,   // 4
        Cache_PostDom   = 1 << 3,   // 8
    }; // Cache

    protected: mutable uint m_rgfCache;

    // ctor
    protected: Graph_() : m_rgfCache(0) {}

    // [C]
    public: void ComputeScc();

    // [E]
    public: class EnumScc : public SccList::Enum {
        public: EnumScc(const Graph* p) :
            SccList::Enum(static_cast<const SccList*>(p)) {}
    }; // EnumScc

    public: class EnumSccReverse : public SccList::EnumReverse {
        public: EnumSccReverse(const Graph* p) :
            SccList::EnumReverse(static_cast<const SccList*>(p)) {}
    }; // EnumSccReverse

    // [H]
    public: bool Has(Cache e) const
        { return 0 != (m_rgfCache & e); }

    // [M]
    public: void MarkCache(Cache e) const {
      ASSERT(!Has(e));
      m_rgfCache |= e;
    }

    // [P]
    private: void prepareDfsTraversal();

    protected: LayoutList* PrepareTraversal(LayoutList* p)
        { return p; }

    protected: PreorderList* PrepareTraversal(PreorderList* p)
        { prepareDfsTraversal(); return p; }

    protected: PostorderList* PrepareTraversal(PostorderList* p)
        { prepareDfsTraversal(); return p; }

    // [S]
    public: void SetChanged() { m_rgfCache = 0; }
}; // Graph_

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_IrGraph_h)
