// @(#)$Id$
//
// AbstractCgTest.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Cg_Tasks_MockCgTarget_h)
#define INCLUDE_test_Cg_Tasks_MockCgTarget_h

#include "./MockCgTarget.h"

namespace CgTest {

using namespace Il::Cg;

class MockCgTarget : public CgTarget {
  public: MockCgTarget();
  public: virtual ~MockCgTarget();
  public: virtual Physical& physical(int name) const override;
  public: virtual Physical& thread_reg() const override;
  public: virtual int ComputeFrameSize(const FrameReg&) const override;
  public: virtual CgOutput* GetArgReg(uint nth) override;
  public: virtual Physical* GetPhysical(const RegDesc* reg_desc) override;
  DISALLOW_COPY_AND_ASSIGN(MockCgTarget);
}; // MockCgTarget

} // CgTest

#endif // !defined(INCLUDE_test_Cg_Tasks_MockCgTarget_h)
