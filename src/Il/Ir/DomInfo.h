// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Il_Ir_DomInfo_h)
#define INCLUDE_evm_Il_Ir_DomInfo_h

#include "../../Common/Collections/ArrayList_.h"

namespace Il {
namespace Ir {

using Common::Collections::ArrayList_;

// Dominance information. This class represents both dominance and
// post-dominance infomration of node.
template<class Node_, class Graph_>
class DomInfo_
    : public DoubleLinkedItem_<DomInfo_<Node_, Graph_>, Node_>,
      public LocalObject {

  public: typedef DomInfo_<Node_, Graph_> DomInfo;
  public: typedef DoubleLinkedList_<DomInfo, Node_> ChildList;
  public: typedef ArrayList_<Node_*> FrontierList;

  protected: ChildList children_;
  protected: uint depth_;
  protected: FrontierList frontiers_;
  protected: Node_& node_;
  protected: Node_* parent_;

  public: DomInfo_(Node_& r) :
      depth_(0), node_(r), parent_(nullptr) {}

  public: ChildList& GetChildren() const {
    return const_cast<DomInfo_*>(this)->children_;
  }

  public: FrontierList& GetFrontiers() const {
    return const_cast<DomInfo_*>(this)->frontiers_;
  }

  public: Node_* GetParent() const { return parent_; }
  public: uint GetDepth() const { return depth_; }
  public: uint SetDepth(uint depth) { return depth_ = depth; }

  public: void Reset() {
    children_.DeleteAll();
    frontiers_.Clear();
    parent_ = nullptr;
  } // Reset

  public: void SetParent(Node_* parent) { parent_ = parent; }

  public: class EnumChild : public ChildList::Enum {
    public: EnumChild(const DomInfo& r)
      : ChildList::Enum(&r.children_) {}

    public: Node_& operator*() const { return *Get(); }
    public: Node_* Get() const { return &ChildList::Enum::Get()->node_; }
    DISALLOW_COPY_AND_ASSIGN(EnumChild);
  };

  public: class EnumFrontier :  public FrontierList::Enum {
    public: EnumFrontier(const DomInfo& r) 
        : FrontierList::Enum(r.frontiers_) {}
    public: Node_& operator*() const { return *Get(); }
    DISALLOW_COPY_AND_ASSIGN(EnumFrontier);
  };

  DISALLOW_COPY_AND_ASSIGN(DomInfo_);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_evm_Il_Ir_DomInfo_h)
