// @(#)$Id$
//
// Evita Compiler - TokenProcessor.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_TokenProcessor_h)
#define INCLUDE_evc_TokenProcessor_h

namespace Compiler {

class Token;

interface TokenProcessor {
  public: virtual void ProcessToken(const Token&) = 0;
};

} // Compiler

#endif // !defined(INCLUDE_evc_TokenProcessor_h)
