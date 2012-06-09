// @(#)$Id$
//
// Evita Lexer Checker
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_CommonTest_h)
#define INCLUDE_test_CommonTest_h

#include <ostream>

#include "Common.h"

namespace Common {
::std::ostream& operator<<(::std::ostream&, const Object&);
::std::ostream& operator<<(::std::ostream&, const String&);
} // Common

#include "gtest/gtest.h"

#endif // !defined(INCLUDE_test_CommonTest_h)
