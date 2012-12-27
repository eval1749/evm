#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- NameRef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NameRef.h"
#include "../Name.h"

namespace Il {
namespace Ir {

namespace {
static Vector_<const NameRef::Item*> MakeSimpleNameItems(
    const Name& name,
    const SourceInfo& source_info) {
  Vector_<const NameRef::Item*> items(1);
  items[0] = new NameRef::SimpleName(name, source_info);
  return items;
}

} // namespace

int NameRef::CompositeName::ComputeHashCode() const {
  auto hash_code = name().ComputeHashCode();
  foreach (Enum, it, *this) {
    hash_code = Common::ComputeHashCode(
        (*it).ComputeHashCode(),
        hash_code);
  }
  return hash_code;
}

bool NameRef::CompositeName::Equals(const Item& another) const {
  if (!another.Is<CompositeName>()) {
    return false;
  }

  Enum enum1(*this);
  Enum enum2(*another.StaticCast<CompositeName>());
  while (!enum1.AtEnd() && !enum2.AtEnd()) {
    if (*enum1 != *enum2) {
      return false;
    }
    enum1.Next();
    enum2.Next();
  }
  return enum1.AtEnd() && enum2.AtEnd();
}

String NameRef::CompositeName::ToString() const {
  StringBuilder builder;
  builder.Append(name());
  builder.Append('<');
  auto comma = "";
  foreach (Enum, it, *this) {
    auto& item = *it;
    builder.Append(comma);
    comma = ",";
    builder.Append(item.ToString());
  }
  builder.Append('>');
  return builder.ToString();
}

NameRef::Item::Item(
    const Name& name,
    const SourceInfo& source_info)
    : name_(name), source_info_(source_info) {}

int NameRef::SimpleName::ComputeHashCode() const {
  return Common::ComputeHashCode('S', name().ComputeHashCode());
}

bool NameRef::SimpleName::Equals(const Item& another) const {
  return another.Is<SimpleName>() && name() == another.name();
}

String NameRef::SimpleName::ToString() const {
  return name().ToString();
}

// ctor
NameRef::NameRef(const Name& name, const SourceInfo& source_info)
    : WithSourceInfo(&source_info),
      items_(MakeSimpleNameItems(name, source_info)) {}

NameRef::NameRef(const Vector_<const Item*>& items)
    : WithSourceInfo(&items[0]->source_info()),
      items_(items) {
  DEBUG_PRINTF("%p\n", this);
}

NameRef::NameRef(
    const Vector_<const Item*>& items,
    const Vector_<int>& ranks)
    : WithSourceInfo(&items[0]->source_info()),
      items_(items),
      ranks_(ranks) {
  DEBUG_PRINTF("%p\n", this);
}

NameRef::~NameRef() {
  DEBUG_PRINTF("%p\n", this);
}

// operators
bool NameRef::operator ==(const NameRef& another) const {
  if (ranks_ != another.ranks_) {
    return false;
  }

  Enum enum1(*this);
  Enum enum2(another);
  while (!enum1.AtEnd() && !enum2.AtEnd()) {
    if (*enum1 != *enum2) {
      return false;
    }
    enum1.Next();
    enum2.Next();
  }
  return enum1.AtEnd() && enum2.AtEnd();
}

bool NameRef::operator !=(const NameRef& another) const {
  return !operator ==(another);
}

// properties
const NameRef& NameRef::arg(int const index) const {
  return items_[items_.size() - 1]->arg(index);
}

const Name& NameRef::name() const {
  return items_[0]->name();
}

const NameRef& NameRef::qualifier() const {
  ASSERT(items_.size() >= 2);
  return *new NameRef(
      Vector_<const Item*>(items_.begin() + 1, items_.end()),
      Vector_<int>(0));
}

// [C]
int NameRef::ComputeHashCode() const {
  auto hash_code = static_cast<int>('N');
  for (auto const rank: ranks_) {
    hash_code = Common::ComputeHashCode(rank, hash_code);
  }

  foreach (Enum, it, *this) {
    auto& item = *it;
    hash_code = Common::ComputeHashCode(
        item.ComputeHashCode(),
        hash_code);
  }
  return hash_code;
}

// [E]
bool NameRef::Equals(const Operand& another) const {
  if (auto const name_ref = another.DynamicCast<NameRef>()) {
    return *this == *name_ref;
  }
  return false;
}

// [G]
const NameRef::SimpleName* NameRef::GetSimpleName() const {
  return items_.size() == 1 && !ranks_.size()
      ? items_[0]->DynamicCast<SimpleName>()
      : nullptr;
}

// [S]
bool NameRef::StartsWith(const NameRef& pattern) const {
  Enum patterns(pattern);
  Enum sources(*this);

  while (!patterns.AtEnd() && !sources.AtEnd()) {
    if (*patterns != *sources) {
      return false;
    }
    patterns.Next();
    sources.Next();
  }

  return patterns.AtEnd();
}

// [T]
String NameRef::ToString() const {
  StringBuilder builder;
  auto dot = "";
  foreach (Enum, it, *this) {
    auto& item = *it;
    builder.Append(dot);
    dot = ".";
    builder.Append(item.ToString());
  }

  for (auto const rank: ranks_) {
    builder.Append('[');
    for (auto i = 1; i < rank; ++i) {
      builder.Append(',');
    }
    builder.Append(']');
  }

  return builder.ToString();
}

} // Ir
} // Il
