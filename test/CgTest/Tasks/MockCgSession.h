// @(#)$Id$
//
// AbstractCgTest.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Cg_Tasks_MockCgSession_h)
#define INCLUDE_test_Cg_Tasks_MockCgSession_h

#include "./MockCgTarget.h"

namespace CgTest {

using namespace Il::Cg;

class MockCgSession : public CgSession {
  private: bool has_error_;
  private: MockCgTarget target_;

  public: MockCgSession() : has_error_(false) {}
  public: virtual ~MockCgSession() {}

  public: virtual void AddErrorInfo(const ErrorInfo&) override {
    has_error_ = true;
  }

  public: virtual bool HasError() const override {
    return has_error_;
  }

  public: virtual const CgTarget& target() const { return target_; }

  DISALLOW_COPY_AND_ASSIGN(MockCgSession);
};

} // CgTest

#endif // !defined(INCLUDE_test_Cg_Tasks_MockCgSession_h)
