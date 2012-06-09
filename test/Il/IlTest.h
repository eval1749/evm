// @(#)$Id$
//
// Evita Lexer Checker
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_IlTest_h)
#define INCLUDE_test_IlTest_h

// warning C4625: 'Foo' : copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable: 4625)

// warning C4626: 'Foo' : assignment operator could not be generated because a base class assignment operator is inaccessible
#pragma warning(disable: 4626)

// warning C4710: function 'int foo(void)' : function not inlined
#pragma warning(disable: 4710)

// warning C4826: Conversion from 'const void *' to 'testing::internal::UInt64' is sign-extended. This may cause unexpected runtime behavior.
#pragma warning(disable: 4826)

// warning C4986: 'operator new[]': exception specification does not match previous declaration
#pragma warning(disable: 4986)

#include "gtest/gtest.h"

#include "Il.h"

typedef Common::Collections::HashSet_<Il::Ir::BBlock*> BBlockSet;
::std::ostream& operator<<(::std::ostream& os, const BBlockSet& set);

#define DEFINE_PRINTER2(ns1, ty) \
  namespace ns1 { \
    ::std::ostream& operator<<(::std::ostream&, const ty&); }

#define DEFINE_PRINTER3(ns1, ns2, ty) \
  namespace ns1 { namespace ns2 { \
    ::std::ostream& operator<<(::std::ostream&, const ty&); } }

#include "./Printers.inc"

#endif // !defined(INCLUDE_test_IlTest_h)
