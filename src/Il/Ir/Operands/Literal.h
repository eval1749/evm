// @(#)$Id$
//
// Evita Compiler - Literal
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Literal_h)
#define INCLUDE_Il_Ir_Literal_h

#include "./Immediate.h"

#include "./PrimitiveType.h"

namespace Il {
namespace Ir {

class Literal : public Operand_<Literal, Immediate> {
  CASTABLE_CLASS(Literal);

  protected: Literal() {}
  public: virtual ~Literal() {}

  public: bool operator==(const Literal& r) const { return Equals(r); }
  public: bool operator!=(const Literal& r) const { return !operator==(r); }

  // [C]
  public: virtual bool CanBeInt32() const { return false; }
  public: virtual bool CanBeUInt32() const { return false; }
  public: virtual int ComputeHashCode() const = 0;

  // [G]
  public: virtual int32 GetInt32() const { CAN_NOT_HAPPEN(); }

  // [N]
  public: static bool NeedInt64(int64);
  public: static bool NeedUInt64(uint64);

  DISALLOW_COPY_AND_ASSIGN(Literal);
};

template<class TSelf, class TBase = Literal>
class Literal_ : public Operand_<TSelf, TBase> {
  protected: typedef Literal_<TSelf, TBase> Base;
  protected: Literal_() {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(Literal_);
};

class BooleanLiteral : public Literal_<BooleanLiteral> {
  CASTABLE_CLASS(BooleanLiteral);
  private: bool const value_;
  public: BooleanLiteral(bool);
  public: virtual ~BooleanLiteral() {}
  public: virtual bool value() const { return value_; }
  public: virtual bool CanBeInt32() const { return true; }
  public: virtual bool CanBeUInt32() const { return true; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual int32 GetInt32() const override { return value_ ? 1 : 0; }
  public: virtual const Type& GetTy() const override { return *Ty_Boolean; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(BooleanLiteral);
};

class CharLiteral : public Literal_<CharLiteral> {
  CASTABLE_CLASS(CharLiteral);
  private: char16 const code_;
  public: CharLiteral(char16);
  public: virtual ~CharLiteral() {}
  public: virtual char16 value() const { return code_; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual const Type& GetTy() const override { return *Ty_Char; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(CharLiteral);
};

class Float32Literal : public Literal_<Float32Literal> {
  CASTABLE_CLASS(Float32Literal);
  private: float32 const value_;
  public: Float32Literal(float32);
  public: virtual ~Float32Literal() {}
  public: virtual float32 value() const { return value_; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: static float32 FromUInt32(uint32);
  public: virtual const Type& GetTy() const override { return *Ty_Float32; }
  public: virtual String ToString() const override;
  public: uint32 ToUInt32() const { return ToUInt32(value_); }
  public: static uint32 ToUInt32(float32 const);
  DISALLOW_COPY_AND_ASSIGN(Float32Literal);
};

class Float64Literal : public Literal_<Float64Literal> {
  CASTABLE_CLASS(Float64Literal);
  private: float64 const value_;
  public: Float64Literal(float64);
  public: virtual ~Float64Literal() {}
  public: virtual float64 value() const { return value_; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: static float64 FromUInt64(uint64);
  public: virtual const Type& GetTy() const override { return *Ty_Float64; }
  public: virtual String ToString() const override;
  public: uint64 ToUInt64() const { return ToUInt64(value_); }
  public: static uint64 ToUInt64(float64);
  DISALLOW_COPY_AND_ASSIGN(Float64Literal);
};

class Int16Literal : public Literal_<Int16Literal> {
  CASTABLE_CLASS(Int16Literal);
  private: int16 const value_;
  public: Int16Literal(int16);
  public: virtual ~Int16Literal() {}
  public: virtual int16 value() const { return value_; }
  public: virtual bool CanBeInt32() const override { return true; }
  public: virtual bool CanBeUInt32() const override { return true; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual int32 GetInt32() const override { return value_; }
  public: virtual const Type& GetTy() const override { return *Ty_Int16; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(Int16Literal);
};

class Int32Literal : public Literal_<Int32Literal> {
  CASTABLE_CLASS(Int32Literal);
  private: int32 const value_;
  public: Int32Literal(int32);
  public: virtual ~Int32Literal() {}
  public: virtual int32 value() const { return value_; }
  public: virtual bool CanBeInt32() const override { return true; }
  public: virtual bool CanBeUInt32() const override { return true; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual int32 GetInt32() const override { return value_; }
  public: virtual const Type& GetTy() const override { return *Ty_Int32; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(Int32Literal);
};

class Int64Literal : public Literal_<Int64Literal> {
  CASTABLE_CLASS(Int64Literal);
  private: int64 const value_;
  public: Int64Literal(int64);
  public: virtual ~Int64Literal() {}
  public: virtual int64 value() const { return value_; }
  public: virtual bool CanBeInt32() const override;
  public: virtual bool CanBeUInt32() const override ;
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual int32 GetInt32() const override;
  public: virtual const Type& GetTy() const override { return *Ty_Int64; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(Int64Literal);
};

class Int8Literal : public Literal_<Int8Literal> {
  CASTABLE_CLASS(Int8Literal);
  private: int8 const value_;
  public: Int8Literal(int8);
  public: virtual ~Int8Literal() {}
  public: virtual int8 value() const { return value_; }
  public: virtual bool CanBeInt32() const override { return true; }
  public: virtual bool CanBeUInt32() const override { return true; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual int32 GetInt32() const override { return value_; }
  public: virtual const Type& GetTy() const override { return *Ty_Int8; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(Int8Literal);
};

class UInt16Literal : public Literal_<UInt16Literal> {
  CASTABLE_CLASS(UInt16Literal);
  private: uint16 const value_;
  public: UInt16Literal(uint16);
  public: virtual ~UInt16Literal() {}
  public: virtual uint16 value() const { return value_; }
  public: virtual bool CanBeInt32() const override { return true; }
  public: virtual bool CanBeUInt32() const override { return true; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual int32 GetInt32() const override { return value_; }
  public: virtual const Type& GetTy() const override { return *Ty_UInt16; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(UInt16Literal);
};

class UInt32Literal : public Literal_<UInt32Literal> {
  CASTABLE_CLASS(UInt32Literal);
  private: uint32 const value_;
  public: UInt32Literal(uint32);
  public: virtual ~UInt32Literal() {}
  public: virtual uint32 value() const { return value_; }
  public: virtual bool CanBeInt32() const override;
  public: virtual bool CanBeUInt32() const override { return true; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual int32 GetInt32() const override;
  public: virtual const Type& GetTy() const override { return *Ty_UInt32; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(UInt32Literal);
};

class UInt64Literal : public Literal_<UInt64Literal> {
  CASTABLE_CLASS(UInt64Literal);
  private: uint64 const value_;
  public: UInt64Literal(uint64);
  public: virtual ~UInt64Literal() {}
  public: virtual uint64 value() const { return value_; }
  public: virtual bool CanBeInt32() const override;
  public: virtual bool CanBeUInt32() const override;
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual int32 GetInt32() const override;
  public: virtual const Type& GetTy() const override { return *Ty_UInt64; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(UInt64Literal);
};

class UInt8Literal : public Literal_<UInt8Literal> {
  CASTABLE_CLASS(UInt8Literal);
  private: uint8 const value_;
  public: UInt8Literal(uint8);
  public: virtual ~UInt8Literal() {}
  public: virtual uint8 value() const { return value_; }
  public: virtual bool CanBeInt32() const override { return true; }
  public: virtual bool CanBeUInt32() const override { return true; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual int32 GetInt32() const override { return value_; }
  public: virtual const Type& GetTy() const override { return *Ty_UInt8; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(UInt8Literal);
};

class StringLiteral : public Literal_<StringLiteral> {
  CASTABLE_CLASS(StringLiteral);
  private: String const string_;
  public: StringLiteral(const String&);
  public: virtual ~StringLiteral() {}
  public: virtual String value() const { return string_; }
  public: virtual int ComputeHashCode() const override;
  public: virtual bool Equals(const Operand&) const override;
  public: virtual const Type& GetTy() const override { return *Ty_String; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(StringLiteral);
};

// Size of Literal is
//  32bit 64bit field
//  +0    +0    void* vtable
//  +28   +48
static_assert(
  sizeof(Literal) < sizeof(void*) == 4 ? 4 : 8,
  "Literal is too large");

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Literal_h)
