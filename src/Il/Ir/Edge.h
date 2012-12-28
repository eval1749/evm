// @(#)$Id$
//
// Evita Il - IR - Edge
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Il_Ir_Edge_h)
#define INCLUDE_evm_Il_Ir_Edge_h

namespace Il {
namespace Ir {

template<class Item_, class Node_>
class Edge_
    : public DoubleLinkedItem_<Item_, Backward>,
      public DoubleLinkedItem_<Item_, Forward> {
  public: enum Kind {
    Kind_Normal,

    Kind_Exit,
    Kind_Nonlocal,
    Kind_Pseudo,
  }; // Kind

  private: Kind m_eKind;
  private: bool m_fBackward;
  private: uint m_nCount;
  private: Node_* m_pFrom;
  private: Node_* m_pTo;

  // ctor
  public: Edge_(Node_* pIn, Node_* pOut, Kind eKind)
    : m_eKind(eKind),
      m_fBackward(false),
      m_nCount(0),
      m_pFrom(pIn),
      m_pTo(pOut) {}

  // [G]
  public: uint GetCount() const { return m_nCount; }
  public: Node_* GetFrom()  const { return m_pFrom; }
  public: Kind GetEdgeKind() const { return m_eKind; }
  public: Node_* GetTo() const { return m_pTo; }

  // [I]
  public: bool IsBackward() const { return m_fBackward; }

  // [S]
  public: bool SetBackward(bool f) { return m_fBackward = f; }
  public: uint SetCount(uint n) { return m_nCount = n; }
  public: Node_* SetFrom(Node_* p) { return m_pFrom = p; }
  public: Kind SetEdgeKind(Kind e) { return m_eKind = e; }
  public: Node_* SetTo(Node_* p) { return m_pTo = p; }

  DISALLOW_COPY_AND_ASSIGN(Edge_);
}; // Edge_

} // Ir
} // Il

#endif // !defined(INCLUDE_evm_Il_Ir_Edge_h)
