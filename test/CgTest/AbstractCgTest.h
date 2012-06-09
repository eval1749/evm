// @(#)$Id$
//
// AbstractCgTest.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_AbstractCgTest_h)
#define INCLUDE_test_AbstractCgTest_h

#include "./CgTest.h"
#include "./Tasks/MockCgSession.h"
#include "../Il/AbstractIltest.h"

namespace CgTest {

using namespace Il::Ir;
using IlTest::AbstractIlTest;

class AbstractCgTest : public AbstractIlTest {
  private: MockCgSession session_;
  protected: AbstractCgTest();
  public: ~AbstractCgTest();
  public: MockCgSession& session() { return session_; }
}; // AbstractTest

} // CgTest

#endif // !defined(INCLUDE_test_AbstractCgTest_h)
