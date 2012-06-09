// @(#)$Id$
//
// Evita Il - Cg - Operands
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_Operands_h)
#define INCLUDE_Il_Cg_Operands_h

#include "../Il/Ir/Operands.h"
#include "./Operands/CgOperand.h"
#include "./Operands/FunLit.h"
#include "./Operands/Physical.h"

namespace Il {
namespace Cg {

/// <summary>
///   Represents closed variable in IR.
/// </summary>
class ClosedMarker : public CgOperand_<ClosedMarker, CgPseudoOutput> {
  CASTABLE_CLASS(ClosedMarker);

  private: uint const m_nNth;

  // ctor
  public: explicit ClosedMarker(uint const nNth) : m_nNth(nNth) {}

  // [G]
  public: uint GetNth() const { return m_nNth; }
  public: virtual char16 GetPrefixChar() const override { return 'c'; }

  public: virtual const Type& GetTy() const override {
    return *Ty_Object; 
  }

  DISALLOW_COPY_AND_ASSIGN(ClosedMarker);
}; // ClosedMarker

/// <summary>
///   Represents memory location in IR.
/// </summary>
class MemSlot : public CgOperand_<MemSlot, CgOutput> {
  CASTABLE_CLASS(MemSlot)

  protected: RegClass const reg_class_;
  private: int const offset_;
  private: int const size_;

  // ctor
  public: MemSlot(
    RegClass const reg_class,
    int const offset,
    int const size)
    : reg_class_(reg_class), offset_(offset), size_(size) {}

  public: int offset() const { return offset_; }
  public: int size() const { return size_; }

  // [G]
  public: RegClass GetClass() const { return reg_class_; }

  DISALLOW_COPY_AND_ASSIGN(MemSlot);
};

template<class TSelf, class TBase = MemSlot>
class MemSlot_ : public Operand3_<TSelf, TBase, RegClass, int, int> {
  public: MemSlot_(RegClass reg_class, int const offset, int const size)
      : Operand3_(reg_class, offset, size) {}

  public: virtual void Apply(CgFunctor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(MemSlot_);
};


class StackSlot
    : public MemSlot_<StackSlot> {
  CASTABLE_CLASS(StackSlot)

  private: int size_;
  private: Variable* m_pVar;

  // ctor
  public: StackSlot(
      RegClass const reg_class,
      int const offset,
      int const size,
      Variable* const pVar = nullptr)
      : MemSlot_(reg_class, offset, size), m_pVar(pVar) {}

  // [G]
  protected: virtual char16 GetPrefixChar() const override { return 's'; }
  public: Variable* GetVar() const { return m_pVar; }

  // [T]
  // Implemented in target.
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(StackSlot);
};

class ThreadSlot : public MemSlot_<ThreadSlot> {
  CASTABLE_CLASS(ThreadSlot)

  // ctor
  public: ThreadSlot(
      RegClass const reg_class,
      int const offset,
      int const size)
      : MemSlot_(reg_class, offset, size) {}

  protected: virtual char16 GetPrefixChar() const override { return 't'; }

  DISALLOW_COPY_AND_ASSIGN(ThreadSlot);
};

#if 0
/// <summary>
///   Represents memory location in IR.
/// </summary>
class TlvOffset : public CgOperand_<TlvOffset, Immediate> {
  CASTABLE_CLASS(TlvOffset)

  private: Val m_tlvrec;

  // ctor
  public: TlvOffset(Val tlvrec) :
      m_tlvrec(tlvrec) { ASSERT(tlv_record_p(tlvrec)); }

  // [G]
  public: Val GetTlvRec() const { return m_tlvrec; }

  DISALLOW_COPY_AND_ASSIGN(TlvOffset);
}; // TlvOffset
#endif

// Used for using spilled memory slot as variable slot.
// See VarDefI processing in SubPassAssign.
class VarHome : public MemSlot_<VarHome> {
  CASTABLE_CLASS(VarHome)

  private: Variable*  m_pVar;

  // ctor
  public: VarHome(
      Variable* pVar,
      RegClass reg_class,
      int const offset,
      int const size)
      : MemSlot_(reg_class, offset, size), m_pVar(pVar) {}

  // [G]
  public: Variable* GetVar() const { return m_pVar; }

  DISALLOW_COPY_AND_ASSIGN(VarHome);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_Operands_h)
