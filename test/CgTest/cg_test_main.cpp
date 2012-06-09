#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CgTest.h"

#include "Common.h"
#include "Il.h"

int main(int argc, char** argv) {
  Common::Init();
  Il::Init();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
