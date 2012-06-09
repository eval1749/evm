#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "Compiler/CompilerGlobal.h"

#include "../Il/IlTest.h"

int main(int argc, char** argv) {
  Compiler::CompilerGlobal::Init();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
