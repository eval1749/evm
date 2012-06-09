// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Method_h)
#define INCLUDE_Il_Ir_Method_h

#include "./Operand.h"

#include "./FunctionType.h"
#include "../WithModifiers.h"
#include "../../../Common/Collections/Array_.h"

namespace Il {
namespace Ir {

class Class;
class MethodGroup;

class Method :
  public DoubleLinkedItem_<Method, MethodGroup>,
  public Operand_<Method>,
  public WorkListItem_<Method>,
  public WithModifiers {

  CASTABLE_CLASS(Method);

  public: typedef DoubleLinkedList_<Method, MethodGroup> List;
  public: typedef Array_<const Name*> ParamNames;

  public: class EnumParamName : public ParamNames::Enum {
    private: typedef ParamNames::Enum Base;
    public: EnumParamName(const Method& r) : Base(r.m_pParamNames) {}
    public: EnumParamName(const Method* p) : Base(p->m_pParamNames) {}
    DISALLOW_COPY_AND_ASSIGN(EnumParamName);
  }; // EnumParamName

  public: class EnumParamType : public FunctionType::EnumParamType {
    private: typedef FunctionType::EnumParamType Base;
    public: EnumParamType(const Method& r) : Base(r.function_type_) {}
    public: EnumParamType(const Method* p) : Base(p->function_type_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumParamType);
  }; // EnumParamType

  public: class ParamTypeScanner : public FunctionType::ParamTypeScanner {
    private: typedef FunctionType::ParamTypeScanner Base;
    public: ParamTypeScanner(const Method& r) : Base(r.function_type_) {}
    public: ParamTypeScanner(const Method* p) : Base(p->function_type_) {}
    DISALLOW_COPY_AND_ASSIGN(ParamTypeScanner);
  }; // ParamTypeScanner

  // m_pFunction is null when this method is external method.
  private: Function* m_pFunction;
  private: const FunctionType& function_type_;
  private: const MethodGroup& method_group_;
  private: ParamNames* const m_pParamNames;

  // ctor
  public: Method(
      const MethodGroup&,
      int const iModifiers,
      const FunctionType&);

  // properties
  public: Module& module() const;

  public: FunctionType& function_type() const {
    return const_cast<FunctionType&>(function_type_);
  }

  public: MethodGroup& method_group() const {
    return const_cast<MethodGroup&>(method_group_);
  }

  public: const Name& name() const;
  public: const Class& owner_class() const { return *GetOwnerClass(); }
  public: const ValuesType& params_type() const;
  public: const Type& return_type() const;

  // [C]
  // TODO(yosi) 2012-02-01 We should move Method::ComputeRestType to
  // another place.
  public: const ArrayType* ComputeRestType() const;
  public: int CountParams() const;

  // [G]
  private: const Class* GetClass() const { return GetOwnerClass(); }
  public: Function* GetFunction() const { return m_pFunction; }
  public: MethodGroup* GetMethodGroup() const { return &method_group(); }
  public: Namespace& GetNamespace() const;
  public: const Class* GetOwnerClass() const;

  // [I]
  public: bool IsMoreSpecific(const Method* const that) const;

  // [S]
  public: void SetFunction(Function&);
  public: void SetParamName(int const, const Name&);

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(Method);
}; // Method

template<class TSelf, class TParent = Method>
class Method_ :
  public WithCastable3_<
      TSelf,
      TParent,
      const MethodGroup&,
      int,
      const FunctionType&> {

  protected: typedef Method_<TSelf, TParent> Base;

  protected: Method_(
      const MethodGroup& method_group,
      int const iModifiers,
      const FunctionType& function_type)
      : WithCastable3_(method_group, iModifiers, function_type) {}

  public: virtual void Apply(Functor* const pFunctor) override {
      pFunctor->Process(static_cast<TSelf*>(this));
  } // Apply

  DISALLOW_COPY_AND_ASSIGN(Method_);
}; // Method_

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Method_h)
