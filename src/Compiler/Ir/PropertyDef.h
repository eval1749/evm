// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_PropertyDef_h)
#define INCLUDE_evc_PropertyDef_h

#include "./VariableOwner.h"

namespace Compiler {

class PropertyDef
    : public NameDef_<PropertyDef, VariableOwner>,
      public WithModifiers {
  CASTABLE_CLASS(PropertyDef)
  private: typedef HashMap_<const Name*, const MethodDef*> MemberMap;

  public: class EnumMember : public MemberMap::Enum {
    private: typedef MemberMap::Enum Base;
    public: EnumMember(const PropertyDef& r) : Base(r.member_map_) {}
  };

  private: MemberMap member_map_;
  private: NamespaceBody& namespace_body_;
  private: const Type& property_type_;
  private: const NameRef& qualified_name_;

  // ctor
  public: PropertyDef(
      NamespaceBody&,
      ClassDef&,
      int,
      const Type&,
      const NameRef&);

  // properties
  public: NamespaceBody& namespace_body() const { return namespace_body_; }
  public: const Type& property_type() const { return property_type_; }
  public: const NameRef& qualified_name() const { return qualified_name_; }

  // [A]
  public: void Add(MethodParamDef&);
  public: void AddMember(const Name&, const MethodDef&);

  // [F]
  public: MethodDef* FindMember(const Name&) const;

  DISALLOW_COPY_AND_ASSIGN(PropertyDef);
};

} // Compiler

#endif // !defined(INCLUDE_evc_PropertyDef_h)
