// @(#)$Id$
//
// Evita Om - Memory Manager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Om_Object_h)
#define INCLUDE_Om_Object_h

#include "./StaticObjects.h"

namespace Om {

class Datum : public AsInt {
};

template<class TSelf, typename TLayout, class TBase = Object>
class Object_ : public TLayout {
  // operator
  public: void* operator new(size_t, void* pv) { return pv; }

  // properties
  public: const Om::Type& type() const { return *type_; }

  // [D]
  public: template<typename T> const T* DynamicCast() {
      return Is<T>() ? &StaticCast<T>() : nullptr;
  }

  public: template<typename T> const T* DynamicCast() const {
      return Is<T>() ? &StaticCast<T>() : nullptr;
  }

  // [G]
  public: int GetFixedSize() const { return sizeof(TLayout); }
  public: const Om::Type* GetType() const { return type_; }

  // [I]
  public: template<typename T> bool Is() const {
      for (
          auto pRunner = this->GetType();
          nullptr != pRunner;
          pRunner = pRunner->GetType()) {
          if (pRunner == T::GetStaticType()) {
              return true;
          } // if
      } // for
      return false;
  }

  // [S]
  public: template<typename T> T& StaticCast() {
    ASSERT(Is<T>());
    return *reinterpret_cast<T*>(reinterpret_cast<TBase*>(this));
  }

  public: template<typename T> const T& StaticCast() const {
    ASSERT(Is<T>());
    return *reinterpret_cast<const T*>(reinterpret_cast<const TBase*>(this));
  }
};

class Object : public Object_<Object, Om::Layout::Object, Datum> {
  // ctor
  public: Object(const Om::Type& type) {
    type_ = &type;
  }

  // [C]
  public: size_t ComputeSize() const;

  // [G]
  public: static Om::Type* GetStaticType();
};

static_assert(
    sizeof(Object) == sizeof(Om::Layout::Object),
    "sizeof(Object) must equal to sizeof(Om::Layout::Object)");

template<class TSelf, typename TLayout, class TBase = Type>
class Type_ : public Object_<TSelf, TLayout, TBase> {
    public: int GetFixedSize() const { return fixed_size_; }
};

class Type : public Type_<Type, Om::Layout::Type, Object> {
  public: int align() const { return align_; }
  public: static Om::Type* GetStaticType();
};

class ArrayType : public Type {
  // [C]
  public: size_t ComputeSize(const int*) const;

  // [G]
  public: static Om::Type* GetStaticType();
};

class VectorType : public Type {
  // [C]
  public: size_t ComputeSize(int) const;

  // [G]
  public: static Om::Type* GetStaticType();
};

class Array : public Object_<Array, Om::Layout::Array> {
  public: int length() const;
  // [G]
  public: const int* dimensions() const;
  public: static Om::Type* GetStaticType();
};

class Class : public Type {
  public: size_t ComputeSize() const { return size_t(fixed_size_); }
  public: static Om::Type* GetStaticType();
};

static_assert(
    sizeof(Object) == sizeof(void*),
    "sizeof(Object) == sizeof(void*)");

class CodeObject : public Object_<CodeObject, Om::Layout::CodeObject> {
  // [G]
  public: static Om::Type* GetStaticType();
};

class PrimitiveType : public Type {
    public: static Om::Type* GetStaticType();
};

class StringType : public Type {
    // [C]
    public: static size_t ComputeSize(int const iLength);

    // [G]
    public: static Om::Type* GetStaticType();
};

class ValueType : public Type {
    public: static Om::Type* GetStaticType();
};

class Value : public Object_<Value, Om::Layout::Value, Object> {
    public: static Om::Type* GetStaticType();
};

template<class TSelf, typename TLayout, class TBase = Value>
class Value_ : public Object_<TSelf, TLayout, TBase> {
    public: int GetFixedSize() const { return m_fixedSize; }
};

class Boolean : public Value_<Boolean, Om::Layout::Boolean> {
    public: static Om::Type* GetStaticType();
};

class Char : public Value_<Char, Om::Layout::Char> {
    public: static Om::Type* GetStaticType();
};

class Float32 : public Value_<Float32, Om::Layout::Float32> {
    public: static Om::Type* GetStaticType();
};

class Float64 : public Value_<Float64, Om::Layout::Float64> {
    public: static Om::Type* GetStaticType();
};

class Int16 : public Value_<Int16, Om::Layout::Int16> {
    public: static Om::Type* GetStaticType();
};

class Int32 : public Value_<Int32, Om::Layout::Int32> {
    public: static Om::Type* GetStaticType();
};

class Int64 : public Value_<Int64, Om::Layout::Int64> {
    public: static Om::Type* GetStaticType();
};

class Int8 : public Value_<Int8, Om::Layout::Int8> {
    public: static Om::Type* GetStaticType();
};

class UInt16 : public Value_<UInt16, Om::Layout::UInt16> {
    public: static Om::Type* GetStaticType();
};

class UInt32 : public Value_<UInt32, Om::Layout::UInt32> {
    public: static Om::Type* GetStaticType();
};

class UInt64 : public Value_<UInt64, Om::Layout::UInt64> {
    public: static Om::Type* GetStaticType();
};

class UInt8 : public Value_<UInt8, Om::Layout::UInt8> {
    public: static Om::Type* GetStaticType();
};

class Void : public Value_<Void, Om::Layout::Void> {
    public: static Om::Type* GetStaticType();
};

class Vector : public Object_<Vector, Om::Layout::Vector, Object> {
  public: Vector(const VectorType& vecty, int const length) {
    length_ = length;
    type_ = &vecty;
  }
  public: int length() const { return length_; }
  public: static Om::Type* GetStaticType();
};

class String : public Object_<String, Om::Layout::String, Object> {
  public: class EnumChar {
    private: int index_;
    private: const String& string_;
    public: EnumChar(const String& r) : index_(0), string_(r) {}
    public: bool AtEnd() const { return index_ >= string_.length(); }
    public: char16 Get() const { 
      ASSERT(!AtEnd()); 
      return string_.elements()[index_]; 
    }
    public: void Next() { ASSERT(!AtEnd()); ++index_; }
    DISALLOW_COPY_AND_ASSIGN(EnumChar);
  };

  public: String(const Vector& chars) {
    ASSERT(chars.type_ == Ty_CharVector);
    chars_ = &chars;
    type_ = Ty_String;
  }

  public: const char16* elements() const {
    return reinterpret_cast<const char16*>(chars_ + 1);
  }

  public: int length() const { return chars_->length_; }
  public: static Om::Type* GetStaticType();
};

static_assert(
    sizeof(String) == sizeof(Om::Layout::String),
    "Size of Om::String must equal to Om::Layout::String");

class CharVector : public Vector {
  public: CharVector(int const length) : Vector(*Ty_CharVector, length) {}
  public: static Om::Type* GetStaticType();

  public: char16 operator[](int index) const {
    return reinterpret_cast<const char16*>(this + 1)[index];
  }

  public: char16& operator[](int index) {
    return reinterpret_cast<char16*>(this + 1)[index];
  }
};

class StringVector : public Vector {
  public: StringVector(int const length) : Vector(*Ty_StringVector, length) {}
  public: static Om::Type* GetStaticType();

  public: String* operator[](int index) const {
    return reinterpret_cast<String* const*>(this + 1)[index];
  }

  public: String*& operator[](int index) {
    return reinterpret_cast<String**>(this + 1)[index];
  }
};

} // Om

#endif // !defined(INCLUDE_Om_Object_h)
