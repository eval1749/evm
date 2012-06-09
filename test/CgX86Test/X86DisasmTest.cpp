#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CgX86Test.h"

#include "Common/Io/StringWriter.h"
#include "Ee/CodeDesc.h"
#include "Targets/x86/Cg/X86DisAsm.h"

namespace CgX86Test {

using Common::Io::StringWriter;
using namespace Il::Cg;

class MyCodeDesc : public Ee::CodeDesc {
  public: MyCodeDesc(const Function& fun, const uint8* codes, int len)
    : Ee::CodeDesc(
        fun, 
        Collection_<Ee::CodeDesc::Annotation>(), 
        const_cast<uint8*>(codes), 
        len) {}
};

class X86DisasmTest : public ::testing::Test {
  private: Module module_;
  private: Function& fun_;

  protected: X86DisasmTest()
    : fun_(
          module_.NewFunction(
              nullptr, Function::Flavor_Toplevel, Name::Intern("foo"))) {}

  protected: String Disasm(const uint8* codes, int codelen) {
    MyCodeDesc code_desc(fun_, codes, codelen);
    StringWriter writer;
    X86Disassemble(writer, code_desc);
    return writer.ToString();
  }
};

TEST_F(X86DisasmTest, Lea_Gv_Ev) {
  uint8 const codes[] = { 0x8D, 0x64, 0x24, 0x08 };
  auto actual = Disasm(codes, ARRAYSIZE(codes));
  EXPECT_EQ(String(" 0000 8D 64 24 08       LEA ESP, [ESP+8]\n"), actual);
}

TEST_F(X86DisasmTest, Mov_Ev_Gv) {
  uint8 const codes[] = { 0x89, 0x5A, 0x08 };
  auto actual = Disasm(codes, ARRAYSIZE(codes));
  EXPECT_EQ(String(" 0000 89 5A 08          MOV [EDX+8], EBX\n"), actual);
}

TEST_F(X86DisasmTest, Mov_Gv_Ev) {
  uint8 const codes[] = { 0x8B, 0x5C, 0x24, 0x04 };
  auto actual = Disasm(codes, ARRAYSIZE(codes));
  EXPECT_EQ(String(" 0000 8B 5C 24 04       MOV EBX, [ESP+4]\n"), actual);
}

TEST_F(X86DisasmTest, Sub_Ev_Ib) {
  uint8 const codes[] = { 0x83, 0xEC, 0x08 };
  auto actual = Disasm(codes, ARRAYSIZE(codes));
  EXPECT_EQ(String(" 0000 83 EC 08          SUB ESP, 8\n"), actual);
}

} // CgX86Test
