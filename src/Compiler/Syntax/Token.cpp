#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Parsre - Token
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Token.h"

#include "./Keyword.h"
#include "./Operator.h"

namespace Compiler {

CharToken::CharToken(
    const SourceInfo& source_info,
    char16 const code)
    : Token_(source_info), code_(code) {}

Literal& CharToken::ToLiteral(MemoryZone& zone) const {
  return *new(zone) CharLiteral(code_);
}

const Type& CharToken::literal_type() const {
  return *Ty_Char;
}

String CharToken::ToString() const {
  return code_ >= ' ' && code_ <= 0x7E
    ? code_ == '\'' || code_ == '\\'
        ? String::Format("'\\%c'", code_)
        : String::Format("'%c'", code_)
    : String::Format("U+%04X", code_);
}

QualifiedNameToken::QualifiedNameToken(
    const Collection_<const NameToken*>& names)
    : Token_(names.Get(0)->source_info()),
      names_(names) {
  ASSERT(names_.Count() >= 2);
  foreach (NameTokens::Enum, names, names_) {
    names.Get()->AddRef();
  }
}

QualifiedNameToken::~QualifiedNameToken() {
  foreach (NameTokens::Enum, names, names_) {
    names.Get()->Release();
  }
}

const NameRef& QualifiedNameToken::ToNameRef() const {
  Vector_<const NameRef::Item*> items(names_.Count());
  auto index = 0;
  foreach (Collection_<const NameToken*>::Enum, names, names_) {
    auto& name_token = *names.Get();
    items[index] = new NameRef::SimpleName(
        name_token.name(),
        name_token.source_info());
    ++index;
  }
  return *new NameRef(items);
}

String QualifiedNameToken::ToString() const {
  StringBuilder builder;
  auto dot = "";
  foreach (Collection_<const NameToken*>::Enum, names, names_) {
    builder.Append(dot);
    builder.Append(names.Get()->name());
    dot = ".";
  }
  return builder.ToString();
}

Float32Token::Float32Token(
    const SourceInfo& source_info,
    float32 const f32)
    : Token_(source_info), f32_(f32) {}

const Type& Float32Token::literal_type() const {
  return *Ty_Float32;
}

Literal& Float32Token::ToLiteral(MemoryZone& zone) const {
  return *new(zone) Float32Literal(f32_);
}

String Float32Token::ToString() const {
  union { float32 f32; uint32 u32; } x;
  x.f32 = f32_;
  return String::Format("Float32(%X)", x.u32);
}

Float64Token::Float64Token(
    const SourceInfo& source_info,
    float64 const f64)
    : Token_(source_info), f64_(f64) {}

const Type& Float64Token::literal_type() const {
  return *Ty_Float64;
}

Literal& Float64Token::ToLiteral(MemoryZone& zone) const {
  return *new(zone) Float64Literal(f64_);
}

String Float64Token::ToString() const {
  union { float64 f64; uint64 u64; } x;
  x.f64 = f64_;
  return String::Format("Float64(%X)", x.u64);
}

IntToken::IntToken(
    const SourceInfo& source_info,
    const Type& int_type,
    int64 const i64)
    : Token_(source_info), i64_(i64), int_type_(int_type) {}

const Type& IntToken::literal_type() const {
  return int_type_;
}

Literal& IntToken::ToLiteral(MemoryZone& zone) const {
  if (int_type_ == *Ty_Int16) {
    return *new(zone) Int16Literal(static_cast<int16>(i64_));
  }

  if (int_type_ == *Ty_Int32) {
    return *new(zone) Int32Literal(static_cast<int32>(i64_));
  }

  if (int_type_ == *Ty_Int64) {
    return *new(zone) Int64Literal(static_cast<int64>(i64_));
  }

  if (int_type_ == *Ty_Int8) {
    return *new(zone) Int8Literal(static_cast<int8>(i64_));
  }

  if (int_type_ == *Ty_UInt16) {
    return *new(zone) UInt16Literal(static_cast<uint16>(i64_));
  }

  if (int_type_ == *Ty_UInt32) {
    return *new(zone) UInt32Literal(static_cast<uint32>(i64_));
  }

  if (int_type_ == *Ty_UInt64) {
    return *new(zone) UInt64Literal(static_cast<uint64>(i64_));
  }

  if (int_type_ == *Ty_UInt8) {
    return *new(zone) UInt8Literal(static_cast<uint8>(i64_));
  }

  CAN_NOT_HAPPEN();
}

String IntToken::ToString() const {
  return int_type_ == *Ty_Int32
    ? String::Format("%d", static_cast<int32>(i64_))
    : int_type_.IsUInt()
        ? String::Format("%s(%u)", int_type_, static_cast<uint64>(i64_))
        : String::Format("%s(%d)", int_type_, i64_);
}

LiteralToken::LiteralToken(const SourceInfo& source_info)
  : Token_(source_info) {}

KeywordToken::KeywordToken(
    const SourceInfo& source_info,
    const Keyword& keyword)
    : Token_(source_info), keyword_(keyword) {}

bool KeywordToken::Is(const Keyword& keyword) const {
  return keyword_ == keyword;
}

String KeywordToken::ToString() const {
  return String::Format("Keyword(%s)", keyword_.ToString());
}

NameToken::NameToken(
    const SourceInfo& source_info,
    const Name& name)
    : Token_(source_info), name_(name) {}

bool NameToken::Is(const Name& name) const {
  return name_ == name;
}

const NameRef& NameToken::ToNameRef() const {
  return *new NameRef(name_, source_info());
}

String NameToken::ToString() const {
  return name_.ToString();
}

NameRefToken::NameRefToken(const NameRef& name_ref)
    : Token_(name_ref.source_info()), name_ref_(name_ref) {}

String NameRefToken::ToString() const {
  return name_ref_.ToString();
}

OperatorToken::OperatorToken(
    const SourceInfo& source_info,
    const Operator& op)
    : Token_(source_info), operator_(op) {}

bool OperatorToken::Is(const Operator& op) const {
  return &operator_ == &op;
}

String OperatorToken::ToString() const {
  return operator_.ToString();
}

RankSpecToken::RankSpecToken(
    const SourceInfo& source_info,
    int const rank)
    : Token_(source_info), rank_(rank) {
  ASSERT(rank_ >= 1);
}

String RankSpecToken::ToString() const {
  StringBuilder builder;
  builder.Append('[');
  for (auto i = 1; i < rank_; ++i) {
    builder.Append(',');
  }
  builder.Append(']');
  return builder.ToString();
}

RawNameToken::RawNameToken(
    const SourceInfo& source_info,
    const Name& name)
    : WithCastable2_(source_info, name) {}

String RawNameToken::ToString() const {
  return String::Format("@%s", name());
}

StringToken::StringToken(
    const SourceInfo& source_info,
    const String& string)
    : Token_(source_info), string_(string) {}

Literal& StringToken::ToLiteral(MemoryZone& zone) const {
  return *new(zone) StringLiteral(string_);
}

String StringToken::ToString() const {
  StringBuilder builder;
  builder.Append('"');
  foreach (String::EnumChar, chars, string_) {
    auto const ch = chars.Get();
    switch (ch) {
      case '"':
      case '\\':
        builder.Append("\\%c", ch);
        break;

      case '\n':
        builder.Append("\\n");
        break;

      case '\t':
        builder.Append("\\t");
        break;

      default:
        if (ch >= ' ' && ch <= 0x7E) {
          builder.Append(static_cast<char16>(ch));
        } else {
          builder.Append("\u%04X", ch);
        }
        break;
     }
  }
  builder.Append('"');
  return builder.ToString();
}

TypeToken::TypeToken(
    const SourceInfo& source_info,
    const Type& type)
    : Token_(source_info), type_(type) {}

String TypeToken::ToString() const {
  return type_.ToString();
}

TypeListToken::TypeListToken(
    const SourceInfo& source_info,
    const Type::Collection& types)
    : Token_(source_info), types_(types) {}

String TypeListToken::ToString() const {
  StringBuilder builder;
  builder.Append('<');
  auto comma = "";
  foreach (Type::Collection::Enum, en, types_) {
    builder.Append(comma);
    comma = ",";
    builder.Append(en.Get()->ToString());
  }
  builder.Append('>');
  return builder.ToString();
}

} // Compiler
