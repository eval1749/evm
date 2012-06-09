// @(#)$Id$
//
// Evita Il - IR - FrameReg.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_FrameReg_h)
#define INCLUDE_Il_Ir_FrameReg_h

#include "./Register.h"

namespace Il {
namespace Ir {

//  m_nCount
//      Number of TAGDEF instructions for computing size of TagsFrame.
class FrameReg
    : public DoubleLinkedItem_<FrameReg, Function>,
      public Register_<FrameReg>,
      public WorkListItem_<FrameReg> {

  CASTABLE_CLASS(FrameReg);

  public: typedef DoubleLinkedList_<FrameReg, Function> List;

  public: enum Kind {
      Kind_Catch,
      Kind_Finally,
  }; // Kind

  private: Kind const m_eKind;
  public:  uint  m_nCount;
  private: int m_ofs;
  private: FrameReg* m_pOuter;

  // ctor
  public: FrameReg(FrameReg* const pOuter, Kind const eKind)
      : Base(RegClass_Frame),
        m_eKind(eKind),
        m_nCount(0),
        m_ofs(0),
        m_pOuter(pOuter) {}

  // [G]
  public: Kind GetFrameKind() const { return m_eKind; }
  public: int GetLocation()  const { return m_ofs; }
  public: FrameReg* GetOuter()     const { return m_pOuter; }
  public: virtual char16 GetPrefixChar() const override { return 'x'; }

  // [I]
  public: bool IsCatch() const { return m_eKind == Kind_Catch; }
  public: bool IsFinally() const { return m_eKind == Kind_Finally; }

  // [S]
  public: int SetLocation(int ofs) { return m_ofs = ofs; }
  public: FrameReg* SetOuter(FrameReg* p) { return m_pOuter = p; }

  DISALLOW_COPY_AND_ASSIGN(FrameReg);
}; // FrameReg

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_FrameReg_h)
