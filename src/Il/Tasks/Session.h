// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_Session_h)
#define INCLUDE_Il_Tasks_Session_h

#include "../Ir/ErrorInfo.h"

namespace Il {
namespace Tasks {

using Il::Ir::ErrorInfo;

interface Session {
  public: virtual void AddErrorInfo(const ErrorInfo&) = 0;
  public: virtual bool HasError() const = 0;
};

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_Session_h)
