#include "precomp.h"
// @(#)$Id$
//
// Evita Common - Formatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Box.h"

#include "./String.h"

namespace Common {

Box::Box(const Box& box) {
  *this = box;
}

Box& Box::operator=(const Box& box) {
  type_ = box.type_;
  switch (type_) {
    case Type_I32:
    case Type_I64:
      datum_.i64 = box.datum_.i64;
      break;

    case Type_Null:
      datum_.i64 = 0;
      break;

    case Type_ObjectPtr:
      datum_.ptr = box.datum_.ptr;
      break;

    case Type_String:
      str_ = box.str_;
      break;

    case Type_U32:
    case Type_U64:
      datum_.u64 = box.datum_.u64;
      break;

    case Type_VoidPtr:
      datum_.ptr= box.datum_.ptr;
      break;

    default:
      CAN_NOT_HAPPEN();
  }
  return *this;
}

// [C]
int Box::ComputeHashCode() const {
  switch (type_) {
    case Type_I32:
      return datum_.i32;

    case Type_I64:
      return static_cast<int>(datum_.i64);

    case Type_Null:
      return 1;

    case Type_ObjectPtr:
      return reinterpret_cast<int>(datum_.obj);

    case Type_String:
      return str_.ComputeHashCode();

    case Type_U32:
      return datum_.u32;

    case Type_U64:
      return static_cast<int>(datum_.u64);

    case Type_VoidPtr:
      return reinterpret_cast<int>(datum_.ptr);

    default:
      CAN_NOT_HAPPEN();
  }
}

bool Box::operator==(const Box& r) const {
  switch (type_) {
    case Type_I32:
      return datum_.i32 == r.datum_.i32;

    case Type_I64:
      return datum_.i64 == r.datum_.i64;

    case Type_Null:
      return true;

    case Type_ObjectPtr:
      return datum_.obj == datum_.obj;

    case Type_String:
      return str_ == r.str_;

    case Type_U32:
      return datum_.u32 == r.datum_.u32;

    case Type_U64:
      return datum_.u64 == r.datum_.u64;

    case Type_VoidPtr:
      return datum_.ptr == r.datum_.ptr;

    default:
      CAN_NOT_HAPPEN();
  }
}

bool Box::operator<=(const Box& r) const {
  switch (type_) {
    case Type_I32:
      return datum_.i32 <= r.datum_.i32;

    case Type_I64:
      return datum_.i64 <= r.datum_.i64;

    case Type_Null:
      return true;

    case Type_ObjectPtr:
      return datum_.obj <= datum_.obj;

    case Type_String:
      return str_ <= r.str_;

    case Type_U32:
      return datum_.u32 <= r.datum_.u32;

    case Type_U64:
      return datum_.u64 <= r.datum_.u64;

    case Type_VoidPtr:
      return datum_.ptr <= r.datum_.ptr;

    default:
      CAN_NOT_HAPPEN();
  }
}

bool Box::operator>=(const Box& r) const {
  switch (type_) {
    case Type_I32:
      return datum_.i32 >= r.datum_.i32;

    case Type_I64:
      return datum_.i64 >= r.datum_.i64;

    case Type_Null:
      return true;

    case Type_ObjectPtr:
      return datum_.obj >= datum_.obj;

    case Type_String:
      return str_ >= r.str_;

    case Type_U32:
      return datum_.u32 >= r.datum_.u32;

    case Type_U64:
      return datum_.u64 >= r.datum_.u64;

    case Type_VoidPtr:
      return datum_.ptr >= r.datum_.ptr;

    default:
      CAN_NOT_HAPPEN();
  }
}
// [T]
String Box::ToString() const {
  char buf[20];
  switch (type_) {
    case Type_I32:
      ::wsprintfA(buf, "%d", datum_.i32);
      break;

    case Type_I64:
      ::wsprintfA(buf, "%d", static_cast<int>(datum_.i64));
      break;

    case Type_Null:
      return String("null");

    case Type_ObjectPtr:
      return datum_.obj->ToString();

    case Type_String:
      return str_;

    case Type_U32:
      ::wsprintfA(buf, "%u", datum_.u32);
      break;

    case Type_U64:
      ::wsprintfA(buf, "%u", static_cast<int>(datum_.i64));
      break;

    case Type_VoidPtr:
      ::wsprintfA(buf, "%p", datum_.ptr);
      break;

    default:
      CAN_NOT_HAPPEN();
  }

  return String(buf);
}

} // Common
