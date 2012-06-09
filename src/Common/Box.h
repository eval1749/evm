// @(#)$Id$
//
// Evita Common - Box
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Box_h)
#define INCLUDE_Common_Box_h

#include "./Object.h"

#include "./String.h"

namespace Common {

// Boxing primitive types.
class Box : public Object_<Box> {
  CASTABLE_CLASS(Box);

  public: enum Type {
    Type_I32,
    Type_I64,
    Type_Null,
    Type_ObjectPtr,
    Type_String,
    Type_U32,
    Type_U64,
    Type_VoidPtr,
  };

  private: union {
    int32 i32;
    int64 i64;
    const Object* obj;
    const void* ptr;
    uint32 u32;
    uint64 u64;
  } datum_;

  private: String str_;
  private: Type type_;

  // ctor
  public: Box() : type_(Type_Null) {}
  public: Box(const char* s) : type_(Type_String), str_(s) {}
  public: Box(const char16* s) : type_(Type_String), str_(s) {}
  public: Box(int32 i32) : type_(Type_I32) { datum_.i64 = i32; }
  public: Box(int64 i64) : type_(Type_I64) { datum_.i64 = i64; }
  public: Box(const Object* p) : type_(Type_ObjectPtr) { datum_.obj = p; }
  public: Box(const Object& r) : type_(Type_ObjectPtr) { datum_.obj = &r; }
  public: Box(const String& s) : type_(Type_String) { str_ = s; }
  public: Box(uint32 u32) : type_(Type_U32) { datum_.u64 = u32; }
  public: Box(uint64 u64) : type_(Type_U64) { datum_.u64 = u64; }
  public: Box(const void* p) : type_(Type_VoidPtr) { datum_.ptr = p; }
  public: Box(const Box&);
  public: Box& operator=(const Box&);

  // operators
  public: bool operator==(const Box&) const;
  public: bool operator!=(const Box& r) const { return !operator==(r); }
  public: bool operator<(const Box& r) const  { return !operator>=(r); }
  public: bool operator<=(const Box&) const;
  public: bool operator>(const Box& r) const  { return !operator<=(r); }
  public: bool operator>=(const Box&) const;

  // properties
  public: int32 i32() const {
    ASSERT(type_ == Type_I32);
    return datum_.i32;
  }

  public: int64 i64() const {
    ASSERT(type_ == Type_I64);
    return datum_.i64;
  }

  public: const Object* obj() const {
    ASSERT(type_ == Type_ObjectPtr);
    return datum_.obj;
  }

  public: const void* ptr() const {
    ASSERT(type_ == Type_VoidPtr);
    return datum_.ptr;
  }

  public: const String& str() const {
    ASSERT(type_ == Type_String);
    return str_;
  }

  public: Type type() const { return type_; }

  public: uint32 u32() const {
    ASSERT(type_ == Type_U32);
    return datum_.u32;
  }

  public: uint64 u64() const {
    ASSERT(type_ == Type_U64);
    return datum_.u64;
  }

  // [C]
  public: int ComputeHashCode() const;

  // [T]
  public: virtual String ToString() const override;
}; // Box

inline String Stringify(const Box& box) { return box.ToString(); }

} // Common

#endif // !defined(INCLUDE_Common_Box_h)
