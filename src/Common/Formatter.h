// @(#)$Id$
//
// Evita Common - Formatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Formatter_h)
#define INCLUDE_Common_Formatter_h

#include "./String.h"
#include "./Collections/Collection_.h"

namespace Common {

#define DEBUG_FORMAT(m_format, a1, ...) \
  DEBUG_PRINTF("%ls\n", String::Format(m_format, a1, __VA_ARGS__).value());

} // Common

#endif // !defined(INCLUDE_Common_Formatter_h)
