// @(#)$Id$
//
// Evita Compiler - IR Class Definition.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Syntax_Ir_ClassDef_h)
#define INCLUDE_Compiler_Syntax_Ir_ClassDef_h

#include "./FieldDef.h"
#include "./TypeDef.h"

namespace Compiler {

/// <summary>
///  Class ClassDef represent class definition in source code.
/// </summary>
class ClassDef : public TypeDef_<ClassDef> {
  CASTABLE_CLASS(ClassDef);

  public: class EnumBaseSpec : public Type::List::Enum {
    private: typedef Type::List::Enum Base;
    public: EnumBaseSpec(const ClassDef& r) : Base(&r.base_specs_) {}
    public: const Type& operator*() const { return *Get(); }
    DISALLOW_COPY_AND_ASSIGN(EnumBaseSpec);
  }; // EnumBaseSpec

  public: class EnumTypeParam : public ArrayList_<TypeParamDef*>::Enum {
    private: typedef ArrayList_<TypeParamDef*>::Enum Base;
    public: EnumTypeParam(const ClassDef& r) : Base(&r.type_params_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumTypeParam);
  }; // EnumTypeParam

  // List of base specification. The first one must be class or interface.
  // Reset of them must be interface.
  // class-base ::= class-type (interface-type)*
  //             |= interface-type+
  private: Type::List base_specs_;

  // Containing namespace body.
  private: NamespaceBody& namespace_body_;

  // List of type parameters for generic class.
  private: ArrayList_<TypeParamDef*> type_params_;

  // ctor
  public: ClassDef(
      NamespaceBody&,
      ClassOrNamespaceDef&,
      int const,
      const Name&,
      const SourceInfo&);

  public: NamespaceBody& namespace_body() const { return namespace_body_; }

  public: Collection_<TypeParamDef*> type_params() const {
    return Collection_<TypeParamDef*>(type_params_);
  }

  // [A]
  public: void AddBaseSpec(const Type&);
  public: void AddTypeParam(TypeParamDef&);

  // [C]
  public: int ComputeHashCode() const;

  // [G]
  public: Class& GetClass() const;

  // [H]
  public: bool HasInterface(const Type&) const;

  // [I]
  public: bool IsClass() const;
  public: bool IsInterface() const;
  public: bool IsStruct() const;

  // [S]
  public: void SetClass();

  // [T]
  public: virtual String ToString() const;

  DISALLOW_COPY_AND_ASSIGN(ClassDef);
}; // ClassDef

} // Compiler

#endif // !defined(INCLUDE_Compiler_Syntax_Ir_ClassDef_h)
