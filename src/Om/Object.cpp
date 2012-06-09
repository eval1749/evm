#include "precomp.h"
// @(#)$Id$
//
// Evita Om - Object
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Object.h"
#include "./StaticObjects.h"

namespace Om {

#define DEFCLASS(mp_name, mp_base, mp_meta) \
Om::Type* mp_name :: GetStaticType() { \
  return static_cast<Om::Type*>(Ty_ ## mp_name); \
}

#include "./StaticClass.inc"

// [C]
size_t ArrayType::ComputeSize(const int* const pDimensions) const {
  auto const pStart = pDimensions;
  auto const pEnd = pStart + rank_;
  auto totalLength = 1;
  for (auto pRunner = pStart; pRunner < pEnd; pRunner++) {
      totalLength *= *pRunner;
  }

  auto const cb = RoundUp(
      totalLength * element_size_ + sizeof(Array),
      size_t(align_));

  return cb;
}

size_t Object::ComputeSize() const {
  auto const meta_type = GetType()->GetType();

  if (meta_type == Ty_VectorType) {
      auto& vector_type = meta_type->StaticCast<VectorType>();
      auto& vector = StaticCast<Vector>();
      return vector_type.ComputeSize(vector.length());
  }

  if (meta_type == Ty_ArrayType) {
      auto& array_type = meta_type->StaticCast<ArrayType>();
      auto& array = StaticCast<Array>();
      return array_type.ComputeSize(array.dimensions());
  }

  return size_t(type().fixed_size_);
}

size_t VectorType::ComputeSize(int const iLength) const {
  auto const cbElts = iLength * element_size_;

  auto const cbObj =
      Ceiling(
        size_t(cbElts),
        sizeof(Val)) * sizeof(Val) + GetFixedSize();

  auto const cb = RoundUp(cbObj, size_t(align_));
  return cb;
}

// [G]
const int* Array::dimensions() const {
  return reinterpret_cast<const int*>(this + 1);
}

#if 0
int ArrayType::GetElementSize() const {
  auto const cbExtra = m_pElementType->GetType() == Ty_ValueType
      ? sizeof(Object)
      : 0;

  auto const cbElt = m_pElementType->GetFixedSize() - cbExtra;

  return cbElt;
}
#endif

int Array::length() const {
  auto& array_type = GetType()->StaticCast<ArrayType>();
  auto const pStart = dimensions();
  auto const pEnd = pStart + array_type.rank_;
  auto totalLength = 1;
  for (auto pRunner = pStart; pRunner < pEnd; pRunner++) {
      totalLength *= *pRunner;
  }
  return totalLength;
}

} // Om
