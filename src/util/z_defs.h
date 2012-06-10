//////////////////////////////////////////////////////////////////////////////
//
// Common Definitions
// z_defs.h
//
// Copyright (C) 2007 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// @(#)$Id: //proj/evedit2/mainline/tinycl/z_defs.h#1 $
//
#if !defined(INCLUDE_z_defs_h)
#define INCLUDE_z_defs_h

// precomp.h must include <stddef.h> for offsetof

// C4373: previous versions of the compiler did not override when parameters
// only differed by const/volatile qualifiers
#pragma warning(disable: 4373)

// C4481: nonstandard extension used: override specifier 'override'
#pragma warning(disable: 4481)

// C4505: 'Common::Collections::ArrayList_<T>::ToString' : unreferenced local function has been removed
#pragma warning(disable: 4505)

// C4350: C++ exception handler used, but unwind semantics are not enabled.
// Specify
// /EHsc in <xlocale>
#pragma warning(disable: 4530)

// warning C4710: 'int swprintf(wchar_t *,size_t,const wchar_t *,...)' : function not inlined
#pragma warning(disable: 4710)

// warning C4711: function 'void Foo(void)' selected for automatic inline expansion
#pragma warning(disable: 4711)

// C4355: 'this': used in base member initializer list:
// http://msdn.microsoft.com/en-us/library/3c594ae3.aspx
#define ALLOW_THIS_IN_INITIALIZER_LIST(expr) \
    __pragma(warning(push)) \
    __pragma(warning(disable:4355)) \
    expr \
    __pragma(warning(pop))

typedef wchar_t char16;

typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef float float32;
typedef double float64;

static_assert(4 == sizeof(float32), "float32");
static_assert(8 == sizeof(float64), "float64");

typedef unsigned int uint;

#define foreach(mp_enum, mp_var, mp_arg) \
    for (mp_enum mp_var(mp_arg); ! (mp_var).AtEnd(); (mp_var).Next())

#define abstract virtual
#if !defined(interface)
#define interface struct
#endif
#define lengthof(a) ( sizeof(a) / sizeof(*(a)) )
#define unless(mp_expr) if (! (mp_expr) )
#define when(mp_expr) if ((mp_expr))

#define LibCallback __fastcall
#define LibExport __fastcall
#define NoReturn __declspec(noreturn)

#define DISALLOW_COPY_AND_ASSIGN(mp_type) \
  private: mp_type(const mp_type&); \
  private: void operator=(const mp_type&)

template<typename T>
T CEILING(T const x, T const y) {
  return (x + y - 1) / y;
}

template<typename T>
T ROUNDUP(T const x, T const y) {
  return CEILING(x, y) * y;
}

template<typename T>
void swap(T& rx, T& ry) {
  auto const temp = rx;
  rx = ry;
  ry = temp;
} // swap

#if _MSC_VER >= 1700
// C++11 requires a forward declaration of an unscoped enumeration must have 
// an underlying type.
#define HAS_FORWARD_DECLARED_ENUMS 0
#else
#define HAS_FORWARD_DECLARED_ENUMS 0
#endif

#if HAS_FORWARD_DECLARED_ENUMS
  #define enum__(name, type) enum name : type
#else
  #define enum__(name, type) enum name
#endif

#endif //!defined(INCLUDE_z_defs_h)
