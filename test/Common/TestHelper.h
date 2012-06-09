// @(#)$Id$
//
// Evita Lexer Checker
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Common_TestHelper_h)
#define INCLUDE_test_Common_TestHelper_h

#include <ostream>

namespace Common {
::std::ostream& operator<<(::std::ostream& os, const Object&);
::std::ostream& operator<<(::std::ostream& os, const String&);
}

#endif // !defined(INCLUDE_test_Common_TestHelper_h)
