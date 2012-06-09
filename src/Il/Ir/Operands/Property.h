// @(#)$Id$
//
// Evita Il Ir Property
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Operands_Property_h)
#define INCLUDE_Il_Ir_Operands_Property_h

#include "./Operand.h"

#include "../WithModifiers.h"

namespace Il {
namespace Ir {

using Common::Collections::HashMap_;

// This class represents property which contains set of methods for
// accessing and other functionaliy. Accessor methods are also called
// "getter" and "setter".
class Property
    : public Operand_<Property>,
      public WithModifiers,
      public WithSourceInfo {
  CASTABLE_CLASS(Property);

  private: typedef HashMap_<const Name*, const Method*> MemberMap;

  public: class EnumMember : public MemberMap::Enum {
    private: typedef MemberMap::Enum Base;
    public: EnumMember(const Property& r) : Base(r.member_map_) {}
  };

  public: static int const PropertyModifiers
      = Modifier_Abstract
      | Modifier_Extern
      | Modifier_Internal
      | Modifier_New
      | Modifier_Override
      | Modifier_Private
      | Modifier_Protected
      | Modifier_Public
      | Modifier_Final
      | Modifier_Static
      | Modifier_Virtual;

  public: static int const AccessorModifiers
      = PropertyModifiers
      | Modifier_SpecialName;

  private: MemberMap member_map_;
  private: const Name& name_;
  private: const Class& owner_class_;
  private: const Type& property_type_;

  // ctor
  public: Property(
    const Class&,
    int,
    const Type&,
    const Name&,
    const SourceInfo* = nullptr);

  public: virtual ~Property() {}

  // properties
  public: const Name& name() const { return name_; }

  public: Class& owner_class() const {
    return const_cast<Class&>(owner_class_);
  }

  public: const Type& property_type() const { return property_type_; }

  // [A]
  public: void Add(const Name&, const Method&);

  // [G]
  public: Method* Get(const Name&) const;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(Property);
}; // Property

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Operands_Property_h)
