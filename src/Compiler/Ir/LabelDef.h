// @(#)$Id$
//
// Evita Compiler - LabelDef.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_LabelDef_h)
#define INCLUDE_evc_LabelDef_h

#include "./NameDef.h"

#include "../../Common/Collections.h"

namespace Compiler {

class LabelDef : public NameDef_<LabelDef> {
  CASTABLE_CLASS(LabelDef)

  public: struct LabelRef {
    public: BBlock* m_pBBlock;
    public: FrameReg* m_pFrameReg;

    public: LabelRef() :
        m_pBBlock(nullptr),
        m_pFrameReg(nullptr) {}

    public: LabelRef(
        BBlock* const pBBlock,
        FrameReg* const pFrameReg) :
            m_pBBlock(pBBlock),
            m_pFrameReg(pFrameReg) {}
  }; // LabelRef

  public: class Enum : public ArrayList_<LabelRef>::Enum {
    private: typedef ArrayList_<LabelRef>::Enum Base;

    public: Enum(LabelDef* const p)
        : Base(&const_cast<LabelDef*>(p)->m_oLabelRefs) {}

    DISALLOW_COPY_AND_ASSIGN(Enum);
  };

  private: bool is_defined_;
  private: BBlock* const m_pBBlock;
  private: FrameReg* m_pFrameReg;
  private: ArrayList_<LabelRef> m_oLabelRefs;

  // ctor
  public: LabelDef(
      MethodDef* const pOwner,
      const Name& name,
      BBlock* const pBBlock,
      const SourceInfo&);

  // [A]
  public: void Add(
      BBlock* const pBBlock,
      FrameReg* const pFrameReg);

  // [D]
  public: void Define(FrameReg* const pFrameReg);

  // [G]
  public: BBlock* GetBBlock() const { return m_pBBlock; }
  public: FrameReg* GetFrameReg() const { return m_pFrameReg; }

  // [I]
  public: bool IsDefined() const;
  public: bool IsUsed() const;

  DISALLOW_COPY_AND_ASSIGN(LabelDef);
};

} // Compiler

namespace Common {

inline String Stringify(const Compiler::LabelDef::LabelRef& label_ref) {
  return String::Format("(LabelRef %s %s)",
    label_ref.m_pBBlock,
    label_ref.m_pFrameReg);
}

}

#endif // !defined(INCLUDE_evc_LabelDef_h)
