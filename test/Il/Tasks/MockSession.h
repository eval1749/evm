// @(#)$Id$
//
// Evita Lexer Checker
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Il_Tasks_MockSession_h)
#define INCLUDE_test_Il_Tasks_MockSession_h

#include "Il/Tasks/Session.h"

namespace IlTest {

using namespace Il::Ir;

class MockSession : public Il::Tasks::Session {
  private: bool has_error_;
  public: MockSession() : has_error_(false) {}

  public: virtual void AddErrorInfo(const ErrorInfo&) override {
    has_error_ = true;
  }

  public: virtual bool HasError() const override {
    return has_error_;
  }
};

} // IlTest

#endif // !defined(INCLUDE_test_Il_Tasks_MockSession_h)
