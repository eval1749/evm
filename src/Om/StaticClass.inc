// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Om - Static Class
//

#if !defined(DEFCLASS)
    #error "You must define DEFCLASS"
#endif

#if !defined(DEFARRAY)
  #define DEFARRAY(mp_name, mp_elemty) \
    DEFCLASS(mp_name, Type, VectorType)
#endif

#if !defined(DEFPRIMITIVE)
  #define DEFPRIMITIVE(mp_name, mp_ctype) \
    DEFCLASS(mp_name, Value, PrimitiveType)
#endif

DEFCLASS(Array, Object, ArrayType)

DEFPRIMITIVE(Boolean, bool)

DEFPRIMITIVE(Char, char16)
DEFCLASS(Class, Type, Class)
DEFCLASS(CodeObject, Object, Class)

DEFPRIMITIVE(Float32, float32)
DEFPRIMITIVE(Float64, float64)

DEFPRIMITIVE(Int16, int16)
DEFPRIMITIVE(Int32, int32)
DEFPRIMITIVE(Int64, int64)
DEFPRIMITIVE(Int8, int8)

DEFCLASS(Object, Object, Class)

DEFCLASS(PrimitiveType, Type, Class)

DEFCLASS(String, Object, Class)

DEFCLASS(Type, Object, Class)

DEFPRIMITIVE(UInt16, uint16)
DEFPRIMITIVE(UInt32, uint32)
DEFPRIMITIVE(UInt64, uint64)
DEFPRIMITIVE(UInt8, uint8)

DEFCLASS(Value, Object, Class)
DEFCLASS(Vector, Object, VectorType)

DEFCLASS(Void, Value, PrimitiveType)

// Meta types
DEFCLASS(ArrayType, Type, Class)
DEFCLASS(VectorType, Type, Class)

// Array types
DEFARRAY(CharVector, Char)
DEFARRAY(StringVector, String)

#undef DEFARRAY
#undef DEFCLASS
#undef DEFPRIMITIVE
