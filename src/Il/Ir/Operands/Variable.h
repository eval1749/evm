// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Variable_h)
#define INCLUDE_Il_Ir_Variable_h

#include "./Operand.h"

namespace Il {
namespace Ir {

/// <summary>
//    Internal representation of lexical variable.
/// </summary>
class Variable
    : public Operand_<Variable, Operand>,
      public Extendable_<Variable>,
      public WithIndex,
      public WithWorkArea,
      public WorkListItem_<Variable> {

  CASTABLE_CLASS(Variable)

  public: enum Storage {
      Storage_Closed,
      Storage_Literal,
      Storage_Register,
      Storage_Stack,
  }; // Storage

  public:  uint m_cUpRefs;
  private: Storage m_eStorage;
  private: int m_iLocation;
  private: Instruction* m_pDefI;
  private: const Type* type_;
  private: const Name& name_;

  // ctor
  public: explicit Variable(const Name&);

  // for extened methods
  protected: Variable();

  public: const Name& name() const { return name_; }

  // [C]
  public: int CountUpRefs() const { return m_cUpRefs; }

  // [G]
  public: static Variable* Get(Operand*);

  public: Instruction* GetDefI() const { return m_pDefI; }
  public: int GetLocation() const { return m_iLocation; }
  public: Function* GetOwner() const;
  public: Register* GetRd() const;
  public: Storage GetStorage() const { return m_eStorage; }
  public: virtual const Type& GetTy() const override { return *type_; }

  // [H]
  public: bool HasLocation() const { return -1 != m_iLocation; }

  // [R]
  public: virtual void Realize(OperandBox* const) override;

  // [S]
  public: Instruction* SetDefI(Instruction* pI) { return m_pDefI = pI; }
  public: int SetLocation(int i) { return m_iLocation = i; }
  public: Storage  SetStorage(Storage e) { return m_eStorage = e; }
  public: void SetTy(const Type&);

  // [T]
  public: virtual String ToString() const override;

  // [U]
  public: virtual void Unrealize(OperandBox* const) override;

  DISALLOW_COPY_AND_ASSIGN(Variable);
}; // Variable

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Variable_h)
