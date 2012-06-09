// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_FunctionType_h)
#define INCLUDE_Il_Ir_FunctionType_h

#include "./ValuesType.h"

#include "../../../Common/Collections/HashMap_.h"

namespace Il {
namespace Ir {

using Common::Collections::HashMap_;

class FunctionType : public Type_<FunctionType> {
  CASTABLE_CLASS(FunctionType);
  // For Dumper.
  public: class EnumParamType : public ValuesType::Enum {
    public: EnumParamType(const FunctionType& r)
        : ValuesType::Enum(r.params_type_) {}

    public: EnumParamType(const FunctionType* const p)
        : ValuesType::Enum(p->params_type_) {}

    DISALLOW_COPY_AND_ASSIGN(EnumParamType);
  }; // EnumParamType

  public: class ParamTypeScanner : public ValuesType::ElementScanner {
    public: ParamTypeScanner(const FunctionType& r)
        : ValuesType::ElementScanner(r.params_type_) {}

    public: ParamTypeScanner(const FunctionType* const p)
        : ValuesType::ElementScanner(p->params_type_) {}

    DISALLOW_COPY_AND_ASSIGN(ParamTypeScanner);
  }; // ParamTypeScanner

  private: const ValuesType& params_type_;
  private: const Type& return_type_;

  // ctor
  private: FunctionType(const Type&, const ValuesType&);
  public: virtual ~FunctionType() {}

  // properties
  public: const ValuesType& params_type() const { return params_type_; }
  public: const Type& return_type() const { return return_type_; }

  // [C]
  public: virtual Type& ComputeBoundType() const override;
  public: virtual int ComputeHashCode() const override;
  public: const ArrayType* ComputeRestType() const;
  public: virtual const Type& Construct(const TypeArgs&) const override;
  public: int CountParams() const;

  // [I]
  public: static FunctionType& LibExport Intern(
      const Type&,
      const ValuesType&);

  public: virtual bool IsBound() const override;
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [M]
  public: static void LibExport MapAll(Functor&);

  // [T]
  public: virtual String ToString() const override;

  // [U]
  public: void Unintern();

  DISALLOW_COPY_AND_ASSIGN(FunctionType);
}; // FunctionType

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_FunctionType_h)
