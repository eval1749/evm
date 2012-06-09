#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Literal
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Literal.h"

#include "./Function.h"
#include "./PointerType.h"
#include "./PrimitiveType.h"

namespace Il {
namespace Ir {

// [N]
bool Literal::NeedInt64(int64 const i64) {
  auto const iHigh = static_cast<int32>(i64 >> 32);
  return !(iHigh == 0 || iHigh == -1);
}

bool Literal::NeedUInt64(uint64 const u64) {
  auto const iHigh = static_cast<uint32>(u64 >> 32);
  return iHigh != 0;
}

// Bool
BooleanLiteral::BooleanLiteral(bool const value) : value_(value) {}

int BooleanLiteral::ComputeHashCode() const {
  return Common::ComputeHashCode('b', value_);
}

bool BooleanLiteral::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<BooleanLiteral>();
  return that && that->value_ == value_;
}

String BooleanLiteral::ToString() const {
  return value_ ? "true" : "false";
}

// Char
CharLiteral::CharLiteral(char16 const code) : code_(code) {}

int CharLiteral::ComputeHashCode() const {
  return Common::ComputeHashCode('c', code_);
}

bool CharLiteral::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<CharLiteral>();
  return that && that->code_ == code_;
}

String CharLiteral::ToString() const {
  return code_ < ' ' || code_ > 0x7E || code_ == '\'' || code_ == '\\'
      ? String::Format("'\\u%04X'", code_)
      : String::Format("'c'", code_);
}

// Float32
Float32Literal::Float32Literal(float32 const value) : value_(value) {}

int Float32Literal::ComputeHashCode() const {
  return Common::ComputeHashCode('f', ToUInt32());
}

bool Float32Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<Float32Literal>();
  return that && that->value_ == value_;
}

float32 Float32Literal::FromUInt32(uint32 const value) {
  union { float32 f32; uint32 u32; } fu;
  fu.u32 = value;
  return fu.f32;
}

String Float32Literal::ToString() const {
  return String::Format("float32(0x%08X)", ToUInt32());
}

uint32 Float32Literal::ToUInt32(float32 const value) {
  union { float32 f32; uint32 u32; } fu;
  fu.f32 = value;
  return fu.u32;
}

// Float64
Float64Literal::Float64Literal(float64 const value) : value_(value) {}

int Float64Literal::ComputeHashCode() const {
  auto const u64 = ToUInt64();
  int hash_code = 'd';
  hash_code = Common::ComputeHashCode(hash_code, static_cast<uint32>(u64));
  hash_code = Common::ComputeHashCode(hash_code,
                                      static_cast<uint32>(u64 >> 32));
  return hash_code;
}

bool Float64Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<Float64Literal>();
  return that && that->value_ == value_;
}

float64 Float64Literal::FromUInt64(uint64 const value) {
  union { float64 f64; uint64 u64; } fu;
  fu.u64 = value;
  return fu.f64;
}

String Float64Literal::ToString() const {
  return String::Format("float64(0x%016X)", ToUInt64());
}

uint64 Float64Literal::ToUInt64(float64 const value) {
  union { float64 f64; uint64 u64; } fu;
  fu.f64 = value;
  return fu.u64;
}

// Int16
Int16Literal::Int16Literal(int16 const value) : value_(value) {}

int Int16Literal::ComputeHashCode() const {
  return Common::ComputeHashCode('w', value_);
}

bool Int16Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<Int16Literal>();
  return that && that->value_ == value_;
}

String Int16Literal::ToString() const {
  return String::Format("int16(%d)", value_);
}

// Int32
Int32Literal::Int32Literal(int32 const value) : value_(value) {}

int Int32Literal::ComputeHashCode() const {
  return Common::ComputeHashCode('d', value_);
}

bool Int32Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<Int32Literal>();
  return that && that->value_ == value_;
}

String Int32Literal::ToString() const {
  return String::Format("%d", value_);
}

// Int64
Int64Literal::Int64Literal(int64 const value) : value_(value) {}

int Int64Literal::ComputeHashCode() const {
  int hash_code = 'q';
  hash_code = Common::ComputeHashCode(
      hash_code,
      static_cast<uint32>(value_ & 0xFFFFFFFF));
  hash_code = Common::ComputeHashCode(
      hash_code,
      static_cast<uint32>((value_>> 32) & 0xFFFFFFFF));
  return hash_code;
}

bool Int64Literal::CanBeInt32() const {
  return !NeedInt64(value_);
}

bool Int64Literal::CanBeUInt32() const {
  return !NeedUInt64(value_);
}

bool Int64Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<Int64Literal>();
  return that && that->value_ == value_;
}

int32 Int64Literal::GetInt32() const {
  ASSERT(CanBeInt32());
  return static_cast<int32>(value_);
}

String Int64Literal::ToString() const {
  return String::Format("int64(0x%016X)", value_);
}

// Int8
Int8Literal::Int8Literal(int8 const value) : value_(value) {}

int Int8Literal::ComputeHashCode() const {
  return Common::ComputeHashCode('B', value_);
}

bool Int8Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<Int8Literal>();
  return that && that->value_ == value_;
}

String Int8Literal::ToString() const {
  return String::Format("int8(%d)", value_);
}

// UInt16
UInt16Literal::UInt16Literal(uint16 const value) : value_(value) {}

int UInt16Literal::ComputeHashCode() const {
  return Common::ComputeHashCode('W', value_);
}

bool UInt16Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<UInt16Literal>();
  return that && that->value_ == value_;
}

String UInt16Literal::ToString() const {
  return String::Format("uint16(%u)", value_);
}

// UInt32
UInt32Literal::UInt32Literal(uint32 const value) : value_(value) {}

bool UInt32Literal::CanBeInt32() const {
  return !NeedInt64(value_);
}

int UInt32Literal::ComputeHashCode() const {
  return Common::ComputeHashCode('D', value_);
}

bool UInt32Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<UInt32Literal>();
  return that && that->value_ == value_;
}

int32 UInt32Literal::GetInt32() const {
  ASSERT(CanBeInt32());
  return static_cast<int32>(value_);
}

String UInt32Literal::ToString() const {
  return String::Format("uint32(%u)", value_);
}

// UInt64
UInt64Literal::UInt64Literal(uint64 const value) : value_(value) {}

bool UInt64Literal::CanBeInt32() const {
  return !NeedInt64(value_);
}

bool UInt64Literal::CanBeUInt32() const {
  return !NeedUInt64(value_);
}

int UInt64Literal::ComputeHashCode() const {
  int hash_code = 'Q';
  hash_code = Common::ComputeHashCode(
      hash_code,
      static_cast<uint32>(value_ & 0xFFFFFFFF));
  hash_code = Common::ComputeHashCode(
      hash_code,
      static_cast<uint32>((value_>> 32) & 0xFFFFFFFF));
  return hash_code;
}

bool UInt64Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<UInt64Literal>();
  return that && that->value_ == value_;
}
int32 UInt64Literal::GetInt32() const {
  ASSERT(CanBeInt32());
  return static_cast<int32>(value_);
}

String UInt64Literal::ToString() const {
  return String::Format("uint64(0x%016X)", value_);
}

// UInt8
UInt8Literal::UInt8Literal(uint8 const value) : value_(value) {}

int UInt8Literal::ComputeHashCode() const {
  return Common::ComputeHashCode('B', value_);
}

bool UInt8Literal::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<UInt8Literal>();
  return that && that->value_ == value_;
}

String UInt8Literal::ToString() const {
  return String::Format("uint8(%u)", value_);
}

// String
StringLiteral::StringLiteral(const String& string) : string_(string) {}

int StringLiteral::ComputeHashCode() const {
  return string_.ComputeHashCode();
}

bool StringLiteral::Equals(const Operand& another) const {
  auto const that = another.DynamicCast<StringLiteral>();
  return that && that->string_ == string_;
}

String StringLiteral::ToString() const {
  StringBuilder builder;
  builder.Append('"');
  foreach (String::EnumChar, chars, string_) {
    auto const code = chars.Get();
    switch (code) {
      case '"':
      case '\\':
        builder.Append("\\%c", code);
        break;

      default:
        if (code < ' ' || code > 0x7E) {
          builder.AppendFormat("\\u%04X", code);
        } else {
          builder.Append(code);
        }
        break;
    }
  }
  builder.Append('"');
  return builder.ToString();
}

} // Ir
} // Il
