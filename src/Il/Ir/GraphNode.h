// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_GraphNode_h)
#define INCLUDE_Il_Ir_GraphNode_h

#include "./DomInfo.h"
#include "./SccNode.h"

namespace Il {
namespace Ir {

template<class TParent, class Node_, class Edge_>
class GraphNode_
    : public ChildItem_<Node_, TParent>,
      public DoubleLinkedItem_<Node_, Preorder>,
      public DoubleLinkedItem_<Node_, Postorder>,
      public SccNode_<Node_, TParent> {

  public: typedef DoubleLinkedItem_<Edge_, Backward> InEdge;
  public: typedef DoubleLinkedItem_<Edge_, Forward> OutEdge;

  protected: typedef DoubleLinkedList_<Edge_, Backward> InEdges;
  protected: typedef DoubleLinkedList_<Edge_, Forward> OutEdges;

  protected: typedef ChildItem_<Node_, TParent> LayoutNode;
  protected: typedef DoubleLinkedItem_<Node_, Preorder> PreorderNode;
  protected: typedef DoubleLinkedItem_<Node_, Postorder> PostorderNode;

  private: typedef GraphNode_<TParent, Node_, Edge_> GraphNode;

  public: typedef DomInfo_<Node_, TParent> DomInfo;
  public: typedef SccNode_<Node_, TParent> SccNode;

  private: uint preorder_;
  private: uint postorder_;
  protected: InEdges m_oInEdges;
  protected: OutEdges m_oOutEdges;
  protected: DomInfo* m_pDomInfo;
  protected: DomInfo* m_pPostDomInfo;

  // ctor
  public: GraphNode_()
      : preorder_(0),
        postorder_(0),
        m_pDomInfo(nullptr),
        m_pPostDomInfo(nullptr) {}

  public: ~GraphNode_() {
    delete m_pDomInfo;
    delete m_pPostDomInfo;
  }

  // [A]
  public: Edge_* AddEdge(Node_* const pTo) {
    auto const pEdge = new Edge_(static_cast<Node_*>(this), pTo);
    m_oOutEdges.Append(pEdge);
    pTo->m_oInEdges.Append(pEdge);
    return pEdge;
  }

  // [C]
  public: int CountInEdges() const {
    auto nCount = 0;
    foreach (EnumInEdge, oEnum, this) {
      nCount += 1;
    }
    return nCount;
  }

  public: int CountOutEdges() const {
    auto nCount = 0;
    foreach (EnumOutEdge, oEnum, this) {
      nCount += 1;
    }
    return nCount;
  }

  // [E]
  public: class EnumChild : public DomInfo::EnumChild {
    public: EnumChild(const Node_* const p)
        : DomInfo::EnumChild(*p->m_pDomInfo) {}

    public: EnumChild(const Node_& r)
        : DomInfo::EnumChild(*r.m_pDomInfo) {}

    public: Node_& operator*() const { return *Get(); }

    DISALLOW_COPY_AND_ASSIGN(EnumChild);
  };

  public: class EnumFrontier : public DomInfo::EnumFrontier {
    public: EnumFrontier(const Node_* const p)
        : DomInfo::EnumFrontier(*p->m_pDomInfo) {}

    public: EnumFrontier(const Node_& r)
        : DomInfo::EnumFrontier(*r.m_pDomInfo) {}

    public: Node_& operator*() const { return *Get(); }

    DISALLOW_COPY_AND_ASSIGN(EnumFrontier);
  };

  public: class EnumInEdge : public InEdges::Enum {
    public: EnumInEdge(const GraphNode_* const p)
        : EnumInEdge::Enum(&p->m_oInEdges) {}

    public: EnumInEdge(const GraphNode_& r)
        : EnumInEdge::Enum(&r.m_oInEdges) {}

    public: Edge_& operator*() const { return *Get(); }
    public: Node_* GetNode() const { return Get()->GetFrom(); }

    DISALLOW_COPY_AND_ASSIGN(EnumInEdge);
  };

  public: class EnumOutEdge : public OutEdges::Enum {
    public: EnumOutEdge(const GraphNode_* const p)
        : EnumOutEdge::Enum(&p->m_oOutEdges) {}

    public: EnumOutEdge(const GraphNode_& r)
        : EnumOutEdge::Enum(&r.m_oOutEdges) {}

    public: Edge_& operator*() const { return *Get(); }
    public: Node_* GetNode() const { return Get()->GetTo(); }

    DISALLOW_COPY_AND_ASSIGN(EnumOutEdge);
  };

  public: class EnumPostChild : public DomInfo::EnumChild {
    public: EnumPostChild(const Node_* const p)
        : DomInfo::EnumChild(*p->m_pPostDomInfo) {}

    public: EnumPostChild(const Node_& r)
        : DomInfo::EnumChild(*r.m_pPostDomInfo) {}

    public: Node_& operator*() const { return *Get(); }

    DISALLOW_COPY_AND_ASSIGN(EnumPostChild);
  };

  public: class EnumPostFrontier : public DomInfo::EnumFrontier {
    public: EnumPostFrontier(const Node_* const p)
        : DomInfo::EnumFrontier(*p->m_pPostDomInfo) {}

    public: EnumPostFrontier(const Node_& r)
        : DomInfo::EnumFrontier(*r.m_pPostDomInfo) {}

    public: Node_& operator*() const { return *Get(); }

    DISALLOW_COPY_AND_ASSIGN(EnumPostFrontier);
  };

  public: class EnumPred : public EnumInEdge {
    public: EnumPred(const GraphNode_* const p) : EnumInEdge(*p) {}
    public: EnumPred(const GraphNode_& r) : EnumInEdge(r) {}
    public: Node_& operator*() const { return *Get(); }
    public: Node_* Get() const { return GetNode(); }
    DISALLOW_COPY_AND_ASSIGN(EnumPred);
  };

  public: class EnumSucc : public EnumOutEdge {
    public: EnumSucc(const GraphNode_* const p) : EnumOutEdge(*p) {}
    public: EnumSucc(const GraphNode_& r) : EnumOutEdge(r) {}
    public: Node_& operator*() const { return *Get(); }
    public: Node_* Get() const { return GetNode(); }
    DISALLOW_COPY_AND_ASSIGN(EnumSucc);
  };

  // [F]
  public: Edge_* FindEdgeFrom(Node_* const pFrom) const {
    foreach (EnumInEdge, oEnum, this) {
      auto const pEdge = oEnum.Get(); 
      if (pEdge->GetFrom() == pFrom) {
        return pEdge;
      }
    }
    return nullptr;
  }

  public: Edge_* FindEdgeTo(Node_* const pTo) const {
    foreach (EnumOutEdge, oEnum, this) {
      auto const pEdge = oEnum.Get();
      if (pEdge->GetTo() == pTo) {
        return pEdge;
      }
    }
    return nullptr;
  }

  // [G]
  public: Node_* GetCommonDominator(Node_* b) const {
    Node_* a = static_cast<Node_*>(const_cast<GraphNode*>(this));

    while (a->m_pDomInfo->GetDepth() > b->m_pDomInfo->GetDepth()) {
      a = a->m_pDomInfo->GetParent();
    }

    while (b->m_pDomInfo->GetDepth() > a->m_pDomInfo->GetDepth()) {
      b = b->m_pDomInfo->GetParent();
    }

    while (a != b) {
      a = a->m_pDomInfo->GetParent();
      b = b->m_pDomInfo->GetParent();
    }

    return a;
  }

  public: Node_* GetImmeidateDominator() const {
    ASSERT(m_pDomInfo != nullptr);
    return m_pDomInfo->GetParent();
  }

  public: Edge_* GetEdge(Node_* pTo) const {
    auto const pEdge = FindEdgeTo(pTo);
    ASSERT(nullptr != pEdge);
    return pEdge;
  }

  public: uint GetPreorder() const { return preorder_; }
  public: uint GetPostorder() const { return postorder_; }
  protected: TParent* GetGraphParent() const { return m_pParent; }

  public: Node_* GetPostImmeidateDominator() const {
    ASSERT(m_pPostDomInfo != nullptr);
    return m_pPostDomInfo->GetParent();
  }

  public: SccNode* GetSccNode() const {
    return const_cast<SccNode*>(static_cast<const SccNode*>(this));
  }

  // [H]
  public: bool HasInEdge() const {
    return m_oInEdges.GetFirst() != nullptr;
  }

  public: bool HasMoreThanOnePred() const {
    EnumPred oEnum(this);
    if (oEnum.AtEnd()) {
      return false;
    }
    oEnum.Next();
    return !oEnum.AtEnd();
  }

  public: bool HasMoreThanOneSucc() const {
    EnumSucc oEnum(this);
    if (oEnum.AtEnd()) {
      return false;
    }
    oEnum.Next();
    return !oEnum.AtEnd();
  }

  public: bool HasOnlyOnePred() const {
    EnumPred oEnum(this);
    if (oEnum.AtEnd()) {
      return false;
    }
    oEnum.Next();
    return oEnum.AtEnd();
  }

  public: bool HasOutEdge() const {
    return m_oOutEdges.GetFirst() != nullptr;
  }

  // [R]
  public: void RemoveEdge(Edge_* pEdge) {
    auto const pTo = pEdge->GetTo();
    pTo->m_oInEdges.Delete(pEdge);
    m_oOutEdges.Delete(pEdge);
    GetGraphParent()->SetChanged();
  }

  protected: void ReplaceEdgeFrom(Node_* const pNew, Node_* const pOld) {
    auto const pEdge = pOld->GetEdge(static_cast<Node_* const>(this));
    pOld->m_oOutEdges.Delete(pEdge);
    pEdge->SetFrom(pNew);
    pNew->m_oOutEdges.Append(pEdge);

    GetGraphParent()->SetChanged();
  }

  protected: void ReplaceEdgeTo(Node_* const pNew, Node_* const pOld) {
    auto const pEdge = GetEdge(pOld);
    pOld->m_oInEdges.Delete(pEdge);
    pEdge->SetTo(pNew);
    pNew->m_oInEdges.Append(pEdge);
    GetGraphParent()->SetChanged();
  }

  public: void RemoveOutEdge(Node_* const pTo) { RemoveEdge(GetEdge(pTo)); }

  // [S]
  protected: TParent* setParent(TParent* const p) { return m_pParent = p; }
  public: uint SetPreorder(uint n) { return preorder_ = n; }
  public: uint SetPostorder(uint n) { return postorder_ = n; }
};

}  // Ir
}  // Il

#endif // !defined(INCLUDE_Il_Ir_GraphNode_h)
