#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - NameRefResolverTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractTaskTest.h"

#include "Compiler/CompileSession.h"
#include "Compiler/Ir.h"
#include "Compiler/Passes/ResolveMethodPass.h"
#include "Compiler/Syntax/Parser.h"
#include "Compiler/Tasks/NameRefResolver.h"

namespace CompilerTest {

class NameRefResolverTest : public AbstractTaskTest {
  protected: NameRefResolverTest()
      : AbstractTaskTest(ResolveMethodPass::Create) {}

  protected: CallI* FindCallI(const Operand& operand) {
    auto const method_def = GetSampleMethodDef();
    foreach (Function::EnumI, insts, method_def->function()) {
      auto& inst = *insts.Get();
      if (auto const call_inst = inst.DynamicCast<CallI>()) {
        auto const args_inst = inst.op1().StaticCast<Values>()
            ->GetDefI()->DynamicCast<ValuesI>();
        if (args_inst && args_inst->CountOperands() >= 1
            && args_inst->op0() == operand) {
          return call_inst;
        }
      }
    }
    return nullptr;
  }

  protected: LoadI* GetLoadI(const NameRefI& name_ref_inst) {
    Register::EnumUser users(name_ref_inst.output().StaticCast<Register>());
    return users.AtEnd() ? nullptr : users.Get()->GetI()->StaticCast<LoadI>();
  }

  protected: NameRefI* GetNameRefI(const String& name1) {
    return GetNameRefI(NewNameRef(name1));
  }

  protected: NameRefI* GetNameRefI(const String& name1, const String& name2) {
    return GetNameRefI(NewNameRef(name1, name2));
  }

  protected: NameRefI* GetNameRefI(const NameRef& name_ref) {
    auto const method_def = GetSampleMethodDef();
    if (!method_def) {
      return nullptr;
    }
    return FindNameRef(method_def->method(), name_ref);
  }

  protected: MethodDef* GetSampleMethodDef() {
    auto const class_def_Sample = FindClassDef("Sample");
    if (!class_def_Sample) {
      return nullptr;
    }

    return FindMethodDef(*class_def_Sample, "Method");
  }

  protected: StoreI* GetStoreI(const NameRefI& name_ref_inst) {
    Register::EnumUser users(name_ref_inst.output().StaticCast<Register>());
    return users.AtEnd() ? nullptr : users.Get()->GetI()->StaticCast<StoreI>();
  }
};

TEST_F(NameRefResolverTest, Cascade) {
  RUN_PARSER(
    "class B { void Run() {} }",
    "class C { B B { get; set; } }",
    "class Sample {",
    "  void Method() {",
    "   C c = new C();",
    "   c.B.Run();",
    "  }",
    "}");

  RUN_PASSES();

  auto const class_B = FindClass("B");
  ASSERT_TRUE((!!class_B));

  auto const mtg_B_Run= FindMethodGroup(*class_B, "Run");
  ASSERT_TRUE((!!mtg_B_Run));

  auto const name_ref_inst = GetNameRefI("B", "Run");
  ASSERT_TRUE((!!name_ref_inst));

  auto const class_C = FindClass("C");
  ASSERT_TRUE((!!class_C));

  auto const method_get_B = FindMethod(class_C, "get_B", *class_B);
  ASSERT_TRUE(!!method_get_B);

  auto const call_inst = FindCall(*name_ref_inst);
  ASSERT_TRUE((!!call_inst));

  NameRefResolver resolver(session(), *GetSampleMethodDef());
  resolver.Resolve(*name_ref_inst);

  ASSERT_EQ(*mtg_B_Run, call_inst->op0());

  EXPECT_EQ(
    *method_get_B,
    call_inst->op1().StaticCast<Values>()
        ->GetDefI()->op0().StaticCast<Register>()
            ->GetDefI()->StaticCast<CallI>()->op0());
}

// name_ref => field_ptr
TEST_F(NameRefResolverTest, FieldGetter) {
  RUN_PARSER(
    "class Sample {",
    "  int x;",
    "  int Method() {",
    "    return x;",
    "  }",
    "}");

  RUN_PASSES();

  auto const field_x = FindClass("Sample")->Find(Name::Intern("x"))
      ->DynamicCast<Field>();
  ASSERT_TRUE(!!field_x);

  auto const name_ref_inst = GetNameRefI("x");
  ASSERT_TRUE((!!name_ref_inst));

  auto const load_inst = GetLoadI(*name_ref_inst);
  ASSERT_TRUE(!!load_inst);

  NameRefResolver resolver(session(), *GetSampleMethodDef());
  resolver.Resolve(*name_ref_inst);

  auto const field_ptr_inst = load_inst->op0().StaticCast<Register>()
      ->GetDefI();
  ASSERT_EQ(Il::Ir::Op_FieldPtr, field_ptr_inst->opcode());
  EXPECT_EQ(*field_x, field_ptr_inst->op1());
}

// name_ref => field_ptr
TEST_F(NameRefResolverTest, FieldSetter) {
  RUN_PARSER(
    "class Sample {",
    "  int x;",
    "  void Method() {",
    "    x = 3;",
    "  }",
    "}");

  RUN_PASSES();

  auto const field_x = FindClass("Sample")->Find(Name::Intern("x"))
      ->DynamicCast<Field>();
  ASSERT_TRUE(!!field_x);

  auto const name_ref_inst = GetNameRefI("x");
  ASSERT_TRUE((!!name_ref_inst));

  auto const store_inst = GetStoreI(*name_ref_inst);
  ASSERT_TRUE(!!store_inst);

  NameRefResolver resolver(session(), *GetSampleMethodDef());
  resolver.Resolve(*name_ref_inst);

  auto const field_ptr_inst = store_inst->op0().StaticCast<Register>()
      ->GetDefI();
  ASSERT_EQ(Il::Ir::Op_FieldPtr, field_ptr_inst->opcode());
  EXPECT_EQ(*field_x, field_ptr_inst->op1());
}

// name_ref => method group
TEST_F(NameRefResolverTest, InstanceMethod) {
  RUN_PARSER(
    "class B { void Run(){} }",
    "class Sample {",
    "  void Method() {",
    "   B b = new B();",
    "   b.Run();",
    "  }",
    "}");

  RUN_PASSES();

  auto const class_B = FindClass("B");
  ASSERT_TRUE((!!class_B));

  auto const mtg_B_Run= FindMethodGroup(*class_B, "Run");
  ASSERT_TRUE((!!mtg_B_Run));

  auto const name_ref_inst = GetNameRefI("Run");
  ASSERT_TRUE((!!name_ref_inst));

  auto const call_inst = FindCall(*name_ref_inst);
  ASSERT_TRUE((!!call_inst));

  NameRefResolver resolver(session(), *GetSampleMethodDef());
  resolver.Resolve(*name_ref_inst);

  EXPECT_EQ(*mtg_B_Run, call_inst->op0());
}

// name_ref => method
TEST_F(NameRefResolverTest, PropertyGetter) {
  RUN_PARSER(
    "class Sample {",
    "  int Method() {",
    "   return \"Bar\".Length;",
    "  }",
    "}");

  RUN_PASSES();

  auto const class_String = FindClass("Common", "String");
  ASSERT_TRUE(!!class_String);

  auto const mtg_String_get_Length = FindMethodGroup(
    *class_String, "get_Length");
  ASSERT_TRUE(!!mtg_String_get_Length);

  auto const mt_String_get_Length = mtg_String_get_Length->Find(
      FunctionType::Intern(*Ty_Int32, ValuesType::Intern()));

  auto const name_ref_inst = GetNameRefI("Length");
  ASSERT_TRUE((!!name_ref_inst));

  auto const method_def = GetSampleMethodDef();

  NameRefResolver resolver(session(), *method_def);
  resolver.Resolve(*name_ref_inst);

  auto const call_inst = FindCallI(name_ref_inst->op0());
  ASSERT_TRUE(!!call_inst);

  EXPECT_EQ(*mt_String_get_Length, call_inst->op0());
}

// name_ref => method
TEST_F(NameRefResolverTest, PropertySetter) {
  RUN_PARSER(
    "class B { int Length { get; set; } }",
    "class Sample {",
    "  void Method() {",
    "   B b = new B();",
    "   b.Length = 3;",
    "  }",
    "}");

  RUN_PASSES();

  auto const class_B = FindClass("B");
  ASSERT_TRUE((!!class_B));

  auto const mtg_B_set_Length = FindMethodGroup(
    *class_B, "set_Length");
  ASSERT_TRUE(!!mtg_B_set_Length);

  auto const mt_B_set_Length = mtg_B_set_Length->Find(
      FunctionType::Intern(*Ty_Void, ValuesType::Intern(*Ty_Int32)));

  auto const name_ref_inst = GetNameRefI("Length");
  ASSERT_TRUE((!!name_ref_inst));

  NameRefResolver resolver(session(), *GetSampleMethodDef());
  resolver.Resolve(*name_ref_inst);

  auto const call_inst = FindCallI(name_ref_inst->op0());
  ASSERT_TRUE(!!call_inst);

  EXPECT_EQ(*mt_B_set_Length, call_inst->op0());
}

// name_ref => method group
TEST_F(NameRefResolverTest, StaticMethod) {
  RUN_PARSER(
    "class B { static void Run() {} }",
    "class Sample {",
    "  void Method() {",
    "   B.Run();",
    "  }",
    "}");

  RUN_PASSES();

  auto const class_B = FindClass("B");
  ASSERT_TRUE((!!class_B));

  auto const mtg_B_Run= FindMethodGroup(*class_B, "Run");
  ASSERT_TRUE((!!mtg_B_Run));

  auto const name_ref_inst = GetNameRefI("B", "Run");
  ASSERT_TRUE((!!name_ref_inst));

  auto const call_inst = FindCall(*name_ref_inst);
  ASSERT_TRUE((!!call_inst));

  NameRefResolver resolver(session(), *GetSampleMethodDef());
  resolver.Resolve(*name_ref_inst);

  EXPECT_EQ(*mtg_B_Run, call_inst->op0());
}

} // CompilerTest
