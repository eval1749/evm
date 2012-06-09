#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - FinalizeEnumPassTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractPassTest.h"

#include "Compiler/CompileSession.h"
#include "Compiler/Passes/FinalizeEnumPass.h"
#include "Compiler/Syntax/Parser.h"
#include "Il/Ir/Name.h"

namespace CompilerTest {

using namespace Compiler;

class FinalizeEnumPassTest : public AbstractPassTest {
  protected: FinalizeEnumPassTest()
      : AbstractPassTest(FinalizeEnumPass::Create) {
  }
};

// o Enum types are direct subclass of Common.Enum.
// o Type of enum member must be base type of enum.

TEST_F(FinalizeEnumPassTest, EnumBasic) {
  RUN_PARSER("enum Color { Red, Green, Blue }");
  RUN_PASSES();

  CHECK_CLASS(class_Color, Ty_Enum, "Color");
  CHECK_FIELD(class_Color, *Ty_Int32, field_Blue, "Blue");
  CHECK_FIELD(class_Color, *Ty_Int32, field_Green, "Green");
  CHECK_FIELD(class_Color, *Ty_Int32, field_Red, "Red");

  CHECK_CONSTANT_FIELD(Int32, 2, field_Blue);
  CHECK_CONSTANT_FIELD(Int32, 1, field_Green);
  CHECK_CONSTANT_FIELD(Int32, 0, field_Red);
}

TEST_F(FinalizeEnumPassTest, EnumValue) {
  RUN_PARSER("enum Color { Red, Green = 10, Blue }");
  RUN_PASSES();

  CHECK_CLASS(class_Color, Ty_Enum, "Color");
  CHECK_FIELD(class_Color, *Ty_Int32, field_Blue, "Blue");
  CHECK_FIELD(class_Color, *Ty_Int32, field_Green, "Green");
  CHECK_FIELD(class_Color, *Ty_Int32, field_Red, "Red");

  CHECK_CONSTANT_FIELD(Int32, 11, field_Blue);
  CHECK_CONSTANT_FIELD(Int32, 10, field_Green);
  CHECK_CONSTANT_FIELD(Int32, 0, field_Red);
}

TEST_F(FinalizeEnumPassTest, EnumShareValue) {
  RUN_PARSER("enum Color : uint { Red, Green, Blue, Max = Blue }");
  RUN_PASSES();

  CHECK_CLASS(class_Color, Ty_Enum, "Color");
  CHECK_FIELD(class_Color, *Ty_UInt32, field_Blue, "Blue");
  CHECK_FIELD(class_Color, *Ty_UInt32, field_Green, "Green");
  CHECK_FIELD(class_Color, *Ty_UInt32, field_Max, "Max");
  CHECK_FIELD(class_Color, *Ty_UInt32, field_Red, "Red");

  CHECK_CONSTANT_FIELD(UInt32, 2, field_Blue);
  CHECK_CONSTANT_FIELD(UInt32, 1, field_Green);
  CHECK_CONSTANT_FIELD(UInt32, 2, field_Max);
  CHECK_CONSTANT_FIELD(UInt32, 0, field_Red);
}

TEST_F(FinalizeEnumPassTest, EnumCircular) {
  RUN_PARSER("enum Color { A = B, B }");

  CHECK_PASS_ERRORS(
    ErrorItem("Finalize_Field_StaticConst_NotConst", 0, 14, "Color.A"),
    ErrorItem("Finalize_Field_StaticConst_NotConst", 0, 21, "Color.B"));
}
} // CompilerTest
