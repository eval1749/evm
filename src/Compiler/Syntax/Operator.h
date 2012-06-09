// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_Operator_h)
#define INCLUDE_evc_Operator_h

#include "../Ir/Entity.h"

namespace Compiler {

class Operator : public Entity_<Operator> {
  CASTABLE_CLASS(Operator);

  public: typedef Instruction* (*MakeInstructionFn)(const Module&);

  public: enum Category {
    Category_None,

    Category_Add,
    Category_Assign,
    Category_Cond,
    Category_CondAnd,
    Category_CondOr,
    Category_Equality,
    Category_LogAnd,
    Category_LogIor,
    Category_LogXor,
    Category_Mul,
    Category_NullOr, // NullCoalescing
    Category_Relational,
    Category_Shift,
    Category_Unary,
  }; // Category

  // Binary operator name for compound assignment.
  private: const Name& binop_name_;
  private: Category const m_eCategory;
  private: Literal* const m_pCallee;
  private: MakeInstructionFn const m_pMakeInstruction;
  private: bool const is_bool_output_;
  private: const Name& name_;

  // ctor
  public: Operator(
      Category eCategory,
      const Name&,
      const Name&,
      bool,
      MakeInstructionFn pMakeInstruction);

  public: const Name& binary_operator_name() const { return binop_name_; }

  // [C]
  public: int ComputeHashCode() const;

  // [F]
  public: static Operator* Find(const Name&);

  // [G]
  public: Literal* GetCallee() const { return m_pCallee; }
  public: Category GetCategory() const { return m_eCategory; }
  public: bool is_bool_output() const { return is_bool_output_; }
  public: const Name& name() const { return name_; }

  // [I]
  public: static void Init();

  // [M]
  public: Instruction* MakeInstruction(const Module& module) const {
    return m_pMakeInstruction(module);
  }

  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(Operator);
}; // Operator

inline int ComputeHashCode(const Operator* const p) {
  ASSERT(p != nullptr);
  return p->ComputeHashCode();
}

} // Compiler

#endif // !defined(INCLUDE_evc_Operator_h)
