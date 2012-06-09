#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MockCgTarget.h"

#include "Om.h"
#include "Om/Frame.h"

namespace CgTest {

using namespace Il::Cg;

namespace {

static RegDesc const kFloatRegDesc[] = {
  { RegClass_Fpr, 0, 32, 0, "f0" },
  { RegClass_Fpr, 1, 32, 1, "f1" },
  { RegClass_Fpr, 2, 32, 2, "f2" },
  { RegClass_Fpr, 3, 32, 3, "f3" },
};

static const RegDesc* const kFloatAllDesc[] = {
  &kFloatRegDesc[0],
  &kFloatRegDesc[1],
  &kFloatRegDesc[2],
  &kFloatRegDesc[3],
};

static RegSet const kFprAll = {
  RegClass_Fpr, ARRAYSIZE(kFloatAllDesc), kFloatAllDesc
};

static RegSet const kFprEmptySet = {
  RegClass_Fpr, 0, nullptr,
};

static RegGroup const kFprGroup = {
    "FPR",
    RegClass_Fpr,
    8,              // width
    nullptr,           // rn
    nullptr,           // rsp
    nullptr,
    &kFprAll,     // all
    &kFprAll,     // allocable
    &kFprAll,     // free
    &kFprAll,     // args
    &kFprEmptySet,   // callee save
    &kFprEmptySet,   // caller save
}; // kFprGroup

static RegDesc const kGprRegDesc[] = {
  { RegClass_Gpr, 0, 32, 0, "g0" },
  { RegClass_Gpr, 1, 32, 1, "g1" },
  { RegClass_Gpr, 2, 32, 2, "g2" },
  { RegClass_Gpr, 3, 32, 3, "g3" },
  { RegClass_Gpr, 4, 32, 3, "g4" },
  { RegClass_Gpr, 5, 32, 3, "g5" },
  { RegClass_Gpr, 6, 32, 3, "g6" },
  { RegClass_Gpr, 7, 32, 3, "g7" },
};

static const RegDesc* const kGprAllDesc[] = {
  &kGprRegDesc[0],
  &kGprRegDesc[1],
  &kGprRegDesc[2],
  &kGprRegDesc[3],
  &kGprRegDesc[4],
  &kGprRegDesc[5],
  &kGprRegDesc[6],
  &kGprRegDesc[7],
};

static const RegDesc* const kGprAllocableDesc[] = {
  &kGprRegDesc[0],
  &kGprRegDesc[1],
  &kGprRegDesc[2],
  &kGprRegDesc[3],
  &kGprRegDesc[4],
};

static RegSet const kGprAll = {
  RegClass_Gpr, ARRAYSIZE(kGprAllDesc), kGprAllDesc
};

static RegSet const kGprEmptySet = {
  RegClass_Gpr, 0, nullptr
};

static RegSet const kGprAllocable = {
  RegClass_Gpr, ARRAYSIZE(kGprAllocableDesc), kGprAllocableDesc
};

static RegSet const kGprArgs = {
  RegClass_Gpr, ARRAYSIZE(kGprAllocableDesc), kGprAllocableDesc
};

static RegGroup const kGprGroup = {
    "GPR",
    RegClass_Gpr,
    4,                      // width
    &kGprRegDesc[7],           // $rn
    &kGprRegDesc[6],           // $rsp
    &kGprRegDesc[5],           // $tcb
    &kGprAll,               // all
    &kGprAllocable,         // allocable
    &kGprAllocable,         // free
    &kGprArgs,              // args
    &kGprEmptySet,          // callee save
    &kGprAllocable,         // caller save
}; // kGprGroup

static Physical* sPhysicals[10];

} // namespace

MockCgTarget::MockCgTarget()
    : CgTarget(&kFprGroup, &kGprGroup) {
  ::ZeroMemory(sPhysicals, sizeof(sPhysicals));
  CgTarget::Set(*this);
}

MockCgTarget::~MockCgTarget() {
  for (auto i = 0; i < ARRAYSIZE(sPhysicals); i++) {
    delete sPhysicals[i];
  }
}

Physical& MockCgTarget::physical(int name) const {
  ASSERT(uint(name) < ARRAYSIZE(sPhysicals));
  return *sPhysicals[name];
}

Physical& MockCgTarget::thread_reg() const {
  return physical(0);
}

int MockCgTarget::ComputeFrameSize(const FrameReg& frame_reg) const {
  auto& open_inst = *frame_reg.GetDefI();

  switch (frame_reg.GetFrameKind()) {
    case FrameReg::Kind_Catch:
      return sizeof(Om::CatchFrame)
          +  open_inst.CountOperands() * sizeof(void*);

    case FrameReg::Kind_Finally: {
      auto& args_inst = *open_inst.op1().StaticCast<Values>()->GetDefI();
      return sizeof(Om::FinallyFrame)
          +  args_inst.CountOperands() * sizeof(void*);
    }

    default:
      CAN_NOT_HAPPEN();
  }
}

CgOutput* MockCgTarget::GetArgReg(uint nth) {
  if (nth < 2) {
    return new Physical(kGprRegDesc[nth]);
  } else {
    return new ThreadSlot(RegClass_Gpr, nth, sizeof(void*));
  }
}

Physical* MockCgTarget::GetPhysical(const RegDesc* reg_desc) {
  auto const index = reg_desc->m_nId;
  if (auto const reg = sPhysicals[index]) {
    return reg;
  }

  auto const reg = new Physical(*reg_desc);
  sPhysicals[index] = reg;
  return reg;
}

} // CgTest
