// @(#)$Id$
//
// Evita Om - Memory Manager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Om_Layout_h)
#define INCLUDE_Om_Layout_h

namespace Om {

class FriendPointer : public DoubleLinkedItem_<FriendPointer> {
    public: typedef DoubleLinkedList_<FriendPointer> List;

    private: void* const m_pv;
    private: int m_cRefs;

    // ctor
    public: FriendPointer(void* const pv) :
        m_cRefs(1),
        m_pv(pv) {}

    // [A]
    public: void AddRef() { m_cRefs++; }

    // [G]
    public: template<typename T> T* Get() const {
        return reinterpret_cast<T*>(m_pv);
    } // Get

    // [R]
    public: void Release() { ASSERT(m_cRefs > 0); m_cRefs--; }
    public: void Reset() { m_cRefs = 0; }

    DISALLOW_COPY_AND_ASSIGN(FriendPointer);
};

namespace Layout {

struct Object {
  const Om::Type* type_;
};
static_assert(
    sizeof(Object) == sizeof(void*),
    "sizeof(Om::Layout::Object) must be sizeof(void*)");

struct Value : Object {};

struct Array : Value {};

struct Boolean : Value {
    bool m_value;
};

struct Char : Value {
    intptr_t m_data;
};

static_assert(
    offsetof(Char, type_) == 0,
    "Offset of type must be zero");

static_assert(
    sizeof(Char) == Arch::Align_Object,
    "sizeof(Char)");

struct CodeObject : Value {
    intptr_t m_length;
    void* m_pvAnnotations;
    void* m_pvGcMap;
};

static_assert(
    sizeof(CodeObject) == sizeof(void*) * 4,
    "sizeof(CodeObject)");

struct Float32 : Value {
    union {
        float32 f32;
        int32   i32;
    } m_u;
};

struct Float64 : Value {
    union {
        float64 f64;
        int64   i64;
    } m_u;
};

struct Int16 : Value { int16 m_value; };
struct Int32 : Value { int32 m_value; };
struct Int64 : Value { int64 m_value; };
struct Int8 : Value { int8 m_value; };

struct UInt16 : Value { uint16 m_value; };
struct UInt32 : Value { uint32 m_value; };
struct UInt64 : Value { uint64 m_value; };
struct UInt8 : Value { uint8 m_value; };

struct Vector : Value {
  intptr_t length_;
};

struct String : Object {
  const Om::Vector* chars_;
};

static_assert(
    sizeof(String) == sizeof(void*) * 2,
    "Om::Layout::String must be just two pointers.");

struct Void : Value {
};

// Meta types
struct Type : Object {
  FriendPointer* m_pEeType;
  Om::Type* base_type_;
  Om::Type* element_type_;
  int align_;
  int element_size_;
  int fixed_size_;
  int rank_;
};

struct Class : Type {};
struct EnumType : Type {};
struct PrimitiveType : Type {};
struct ArrayType : Type {};
struct ValueType : Type {};
struct VectorType : Type {};

// Array types
struct CharVector : VectorType {};
struct StringVector : VectorType {};

} // Layout
} // Om

#endif // !defined(INCLUDE_Om_Layout_h)
