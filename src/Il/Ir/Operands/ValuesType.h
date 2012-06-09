// @(#)$Id$
//
// Evita IL - IR - ValuesType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_ValuesType_h)
#define INCLUDE_Il_Ir_ValuesType_h

#include "./ArrayType.h"
#include "./Type.h"

#include "../../../Common/Collections.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

/// <summary>
///   Values type.
/// </summary>
class ValuesType : public Type_<ValuesType> {
  CASTABLE_CLASS(ValuesType);

  public: class Enum : public Type::Collection::Enum {
    private: typedef Type::Collection::Enum Base;
    public: Enum(const ValuesType& r) : Base(r.types_) {}
    public: Enum(const ValuesType* p) : Base(p->types_) {}
    public: const Type& operator*() const { return Get(); }
    public: const Type& Get() const { return *Base::Get(); }
    DISALLOW_COPY_AND_ASSIGN(Enum);
  };

  public: class ElementScanner {
    private: int m_cRequireds;
    private: const ArrayType* const rest_type_;
    private: Enum m_oEnum;

    public: ElementScanner(const ValuesType& r)
        : m_oEnum(r),
          rest_type_(r.ComputeRestType()) {
      m_cRequireds = !rest_type_ ? r.Count() : r.Count() - 1;
    }

    public: bool IsRequired() const {
      return m_cRequireds > 0;
    }

    public: const Type& Get() const {
      ASSERT(this->IsRequired());
      return this->IsRequired() 
          ? m_oEnum.Get()
          : *rest_type_;
    }

    public: const ArrayType* GetRestType() const {  return rest_type_; }

    public: void Next() {
      ASSERT(this->IsRequired());
      m_oEnum.Next();
      m_cRequireds--;
    } // Next

    DISALLOW_COPY_AND_ASSIGN(ElementScanner);
  }; // ElementScanner

  private: Type::Collection types_;

  private: explicit ValuesType(const Type::Collection&);
  public: virtual ~ValuesType() {}

  // [C]
  public: virtual const Type& ComputeBoundType() const override;
  public: virtual int ComputeHashCode() const override;
  public: const ArrayType* ComputeRestType() const;
  public: virtual const Type& Construct(const TypeArgs&) const override;
  public: int Count() const;

  // [G]
  public: const Type& Get(int) const;

  // [I]
  public: static const ValuesType& LibExport Intern();
  public: static const ValuesType& LibExport Intern(const Type::Collection&);
  public: static const ValuesType& LibExport Intern(const Type&);
  public: static const ValuesType& LibExport Intern(const Type&, const Type&);

  public: virtual bool IsBound() const override;
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [M]
  public: static void LibExport MapAll(Functor&);

  // [T]
  public: virtual String ToString() const override;

  // [U]
  public: void Unintern();

  DISALLOW_COPY_AND_ASSIGN(ValuesType);
}; // ValuesType

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_ValuesType_h)
