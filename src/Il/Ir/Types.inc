// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita VM IL IR Types
//

#if !defined(DEFINE_CLASS)
  #define DEFINE_CLASS(mp_outer, mp_name, mp_mod, ...) \
    DEFTYPE(Class, mp_mod, mp_outer, mp_name, __VA_ARGS__)
#endif

#if !defined(DEFINE_GENERIC_CLASS_1)
  #define DEFINE_GENERIC_CLASS_1(mp_outer, mp_name, mp_mod, ...) \
    DEFTYPE(GenericClass, mp_mod, mp_outer, mp_name, __VA_ARGS__)
#endif

#if !defined(DEFINE_PRIMITIVE_TYPE)
  #define DEFINE_PRIMITIVE_TYPE(mp_name, mp_rc, mp_size, mp_sign) \
    DEFTYPE(PrimitiveType, Public | Final, Common, mp_name)
#endif

#if !defined(DEFTYPE)
  #error "You must define DEFTYPE."
#endif

DEFINE_PRIMITIVE_TYPE(Boolean, Gpr, 1, false)
DEFINE_PRIMITIVE_TYPE(Char, Gpr, 16, false)
DEFINE_PRIMITIVE_TYPE(Float32, Fpr, 32, false)
DEFINE_PRIMITIVE_TYPE(Float64, Fpr, 64, false)
DEFINE_PRIMITIVE_TYPE(Int16, Gpr, 16, true)
DEFINE_PRIMITIVE_TYPE(Int32, Gpr, 32, true)
DEFINE_PRIMITIVE_TYPE(Int64, Gpr, 64, true)
DEFINE_PRIMITIVE_TYPE(Int8, Gpr, 8, true)
DEFINE_PRIMITIVE_TYPE(UInt16, Gpr, 16, false)
DEFINE_PRIMITIVE_TYPE(UInt32, Gpr, 32, false)
DEFINE_PRIMITIVE_TYPE(UInt64, Gpr, 64, false)
DEFINE_PRIMITIVE_TYPE(UInt8, Gpr, 8, false)
DEFINE_PRIMITIVE_TYPE(Void, Gpr, 0, false)

DEFINE_CLASS(Common, ArgumentOutOfRangeException, Public, *Ty_Exception)
DEFINE_CLASS(Common, Class, Public, *Ty_Type)
DEFINE_CLASS(Common, Disposable, Public | Interface)
DEFINE_CLASS(Common, Enum, Public | Abstract, *Ty_Value)
DEFINE_CLASS(Common, Exception, Public)
DEFINE_CLASS(Common, Object, Public)
DEFINE_CLASS(Common, String, Public | Final, 
    *Ty_Object,
    CONSTRUCTED_CLASS_(Comparable, String),
    CONSTRUCTED_CLASS_(Enumerable, Char),
    CONSTRUCTED_CLASS_(Hashable, String))
DEFINE_CLASS(Common, Type, Public | Abstract)
DEFINE_CLASS(Common, Value, Public | Abstract)

DEFINE_GENERIC_CLASS_1(Common, Array, Public | Final,
    CONSTRUCTED_CLASS_(Enumerable, T))
DEFINE_GENERIC_CLASS_1(Common, Box, Public | Final)
DEFINE_GENERIC_CLASS_1(Common, ClosedCell, Public | Final)
DEFINE_GENERIC_CLASS_1(Common, Comparable, Public | Interface)
DEFINE_GENERIC_CLASS_1(Common, Enumerable, Public | Interface)
DEFINE_GENERIC_CLASS_1(Common, Enumerator, Public | Interface)
DEFINE_GENERIC_CLASS_1(Common, Equatable, Public | Interface)
DEFINE_GENERIC_CLASS_1(Common, Hashable, Public | Interface,
    CONSTRUCTED_CLASS_(Equatable, T))
DEFINE_GENERIC_CLASS_1(Common, Nullable, Public | Final)
DEFINE_GENERIC_CLASS_1(Common, LiteralCell, Public | Final)
DEFINE_GENERIC_CLASS_1(Common, StackCell, Public | Final)

// Note: We don't need to define Console class here. We have Console
// class here for testing.
DEFINE_CLASS(Common, Console, Public | Static)

#undef DEFINE_CLASS
#undef DEFINE_GENERIC_CLASS_1
#undef DEFINE_PRIMITIVE_TYPE
#undef DEFTYPE
