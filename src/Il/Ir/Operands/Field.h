// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Field_h)
#define INCLUDE_Il_Ir_Field_h

#include "./Operand.h"

#include "../WithModifiers.h"
#include "../WithSourceInfo.h"

namespace Il {
namespace Ir {

class Field :
    public DoubleLinkedItem_<Field, Class>,
    public Operand_<Field>,
    public WithModifiers,
    public WithSourceInfo {
  CASTABLE_CLASS(Field);

  public: typedef DoubleLinkedList_<Field, Class> List;

  // TODO(yosi) 2012-01-09 What is Filed.operand_?
  private: int index_;
  private: int offset_;
  private: const Name& name_;
  private: const Operand* operand_;
  private: Class& owner_class_;
  private: const Type& storage_type_;

  // ctor
  public: Field(
      Class& owner_class,
      int const modifiers,
      const Type& storage_type,
      const Name& name,
      const SourceInfo* const source_info = nullptr);

  // properties
  public: const Type& field_type() const;
  public: int index() const { return index_; }
  public: const Name& name() const { return name_; }
  public: int offset() const { return offset_; }
  public: Class& owner_class() const { return owner_class_; }
  public: const Type& storage_type() const { return storage_type_; }

  public: void set_offset(int x) { SetOffset(x); }
  public: void set_operand(const Operand* x) { SetOperand(x); }

  // [G]
  public: int GetIndex() const { ASSERT(index_ >= 0); return index_; }
  public: int GetOffset() const { return offset_; }
  public: const Operand* GetOperand() const { return operand_; }
  public: virtual const Type& GetTy() const override;

  // [S]
  public: void SetOffset(int const offset);
  public: void SetOperand(const Operand*);

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(Field);
}; // Field

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Field_h)
