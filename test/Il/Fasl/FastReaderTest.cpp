#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractFaslTest.h"

#include "Il/Fasl/FaslErrorInfo.h"
#include "Il/Fasl/FastReader.h"
#include "../Tasks/MockSession.h"

namespace IlTest {

using namespace Il::Fasl;

// MockReaderCallback class serializes Fasl stream into string.
class MockReaderCallback : public FastReader::Callback {
  private: StringBuilder strbuf_;
  private: Stack_<String> operand_stack_;
  private: ArrayList_<String> operand_list_;
  private: int output_counter_;

  public: MockReaderCallback() : output_counter_(0) {
    Reset();
  }

  public: virtual ~MockReaderCallback() {}

  // [F]
  public: void Finish() {
    if (!operand_stack_.IsEmpty()) {
      strbuf_.Append("\nOperand stack should be empty.\n");
      auto count = 0;
      while (!operand_stack_.IsEmpty()) {
        String str(operand_stack_.Pop());
        ++count;
        strbuf_.AppendFormat("%d ", count);
        strbuf_.Append(str);
        strbuf_.Append("\n");
      }
    }
  }

  // [G]
  public: String GetString() const {
    return strbuf_.ToString();
  }

  // [H][A]
  public: virtual void HandleArray(int const count) override {
    Stack_<String> operands;
    for (int i = 0; i < count; i++) {
      operands.Push(operand_stack_.Pop());
    }

    StringBuilder buf;
    buf.Append('[');
    const char* comma ="";
    for (int i = 0; i < count; i++) {
      buf.Append(comma);
      comma = " ";
      buf.Append(operands.Pop());
    }
    buf.Append(']');
    operand_stack_.Push(buf.ToString());
  }

  public: virtual void HandleArrayType() override {
    PushAndRemember("ArrayType", 2);
  }

  // [H][C]
  public: virtual void HandleClass() override {
    PopAndRemember("Class", 3);
  }

  public: virtual void HandleClassContext() override {
    PopCommand("ClassContext", 1);
  }

  public: virtual void HandleClassRef() override {
    PushAndRemember("ClassRef", 2);
  }

  public: virtual void HandleConstructedClassRef() override {
    PushAndRemember("ConstructedClassRef", 2);
  }

  public: virtual void HandleConstructedMethodRef() override {
    PushAndRemember("ConstructedMethodRef", 2);
  }

  // [H][E]
  public: virtual void HandleError(const FaslErrorInfo&) override {
  }

  // [H][F]
  public: virtual void HandleField(const SourceInfo&) override {
    PopAndRemember("Field", 3);
  }

  public: virtual void HandleFileMagic() override {
    PopCommand("FileMagic", 1);
  }

  public: virtual void HandleFunction() override {
    PushAndRemember("Function", 4);
  }

  public: virtual void HandleFunctionBody() override {
    PopCommand("FunctionBody", 2);
    // 1 = False
    // 2 = True
    // 3 = Void
    output_counter_ = 3;
  }

  public: virtual void HandleFunctionType() override {
    PushAndRemember("FunctionType", 2);
  }

  // [H][G]
  public: virtual void HandleGenericClass() override {
    PopAndRemember("GenericClass", 4);
  }

  public: virtual void HandleGenericMethod() override {
    PopAndRemember("GenericMethod", 3);
  }

  // [H][I]
  public: virtual void HandleInt(int64 i64) override {
    StringBuilder buf;
    buf.AppendFormat("%d", i64);
    operand_stack_.Push(buf.ToString());
  }

  public: virtual void HandleInstruction(
      Op const opcode,
      const SourceInfo&) override {
    auto const operands = operand_stack_.Pop();
    auto const outy = operand_stack_.Pop();

    if (outy == "Void") {
      strbuf_.AppendFormat("(Instruction %s %s)\n",
          Instruction::GetMnemonic(opcode),
          operands);
    } else {
      ++output_counter_;
      strbuf_.AppendFormat("(Instruction %s %s %%r%d = %s)\n",
          Instruction::GetMnemonic(opcode),
          outy,
          output_counter_,
          operands);
    }
  }

  // [H][L]
  public: virtual void HandleLabel(int name) override {
    StringBuilder buf;
    buf.AppendFormat("(Label %d)", name);
    operand_stack_.Push(buf.ToString());
  }

  public: virtual void HandleLiteral(const Literal& lit) override {
    StringBuilder buf;
    if (auto const p = lit.DynamicCast<BooleanLiteral>()) {
      buf.AppendFormat("(Boolean %s)", p->value() ? "true" : "false");

    } else if (auto const p = lit.DynamicCast<CharLiteral>()) {
      buf.AppendFormat("(Char U+%04X)", p->value());

    } else if (auto const p = lit.DynamicCast<Int16Literal>()) {
      buf.AppendFormat("(Int16 %d)", p->value());

    } else if (auto const p = lit.DynamicCast<Int32Literal>()) {
      buf.AppendFormat("(Int32 %d)", p->value());

    } else if (auto const p = lit.DynamicCast<Int64Literal>()) {
      buf.AppendFormat("(Int64 %d)", p->value());

    } else if (auto const p = lit.DynamicCast<Int8Literal>()) {
      buf.AppendFormat("(Int8 %d)", p->value());

    } else if (auto const p = lit.DynamicCast<StringLiteral>()) {
      buf.AppendFormat("@\"%s\"", p->value());

    } else if (auto const p = lit.DynamicCast<UInt16Literal>()) {
      buf.AppendFormat("(UInt16 %u)", p->value());

    } else if (auto const p = lit.DynamicCast<UInt32Literal>()) {
      buf.AppendFormat("(UInt32 %u)", p->value());

    } else if (auto const p = lit.DynamicCast<UInt64Literal>()) {
      buf.AppendFormat("(UInt64 %lu)", p->value());

    } else if (auto const p = lit.DynamicCast<UInt8Literal>()) {
      buf.AppendFormat("UInt8 %u)", p->value());

    } else {
      CAN_NOT_HAPPEN();
    }
    operand_stack_.Push(buf.ToString());
    delete &const_cast<Literal&>(lit);
  }

  // [H][M]
  public: virtual void HandleMethod() override {
    PopAndRemember("Method", 2);

    // TODO(yosi) 2012-01-22 Remember method function if and only if method
    // isn't external.
    StringBuilder buf;
    buf.Append("(MethodFunction)");
    operand_list_.Add(buf.ToString());
  }

  public: virtual void HandleMethodGroup() override {
    PopAndRemember("MethodGroup", 1);
  }

  public: virtual void HandleMethodRef() override {
    PopAndRemember("MethodRef", 3);
  }

  // [H][N]
  public: virtual void HandleName(const char16* name) override {
    StringBuilder buf;
    buf.AppendFormat("#%d='%s'", operand_list_.Count() + 1, name);
    auto const str = buf.ToString();
    operand_list_.Add(str);
    operand_stack_.Push(str);
  }

  public: virtual void HandleNamespace() override {
    PushAndRemember("Namespace", 2);
  }

  // [H][O]
  public: virtual void HandleOutput(int const name) override {
    StringBuilder buf;
    buf.AppendFormat("%%%d", name);
    operand_stack_.Push(buf.ToString());
  }

  // [H][P]
  public: virtual void HandlePatchPhi() override {
    PushAndRemember("PatchPhi", 2);
  }

  public: virtual void HandlePointerType() override {
    PushAndRemember("PointerType", 1);
  }

  public: virtual void HandleProperty() override {
    PopCommand("Property", 3);
  }

  public: virtual void HandlePropertyMember() override {
    PopCommand("PropertyMember", 2);
  }

  // [H][R]
  public: virtual void HandleRealizeClass() override {
    PopCommand("RealizeClass", 2);
  }

  public: virtual void HandleRealizeTypeParam() override {
    PopCommand("RealizeTypeParam", 3);
  }

  public: virtual void HandleRef(int const ref_id) override {
    if (ref_id <= 0) {
      operand_stack_.Push(String("???"));
    } else if (ref_id <= 6) {
      auto const val = operand_list_.Get(ref_id - 1);
      operand_stack_.Push(val);
    } else {
      StringBuilder buf;
      buf.AppendFormat("#%d#", ref_id);
      operand_stack_.Push(buf.ToString());
    }
  }

  public: virtual void HandleReset() override {
    Reset();
  }

  // [H][T]
  public: virtual void HandleTypeParam() override {
    PushAndRemember("TypeParam", 1);
  }

  // [H][V]
  public: virtual void HandleValuesType() override {
    PushAndRemember("ValuesType", 1);
  }

  public: virtual void HandleVariable() override {
    PushAndRemember("Variable", 1);
  }

  // [H][Z]
  public: virtual void HandleZero() override {
    operand_stack_.Push(String("Zero"));
  }

  // [P]
  private: void PopCommand(const char* const name, int const count) {
    strbuf_.Append(PopOperands(name, count));
    strbuf_.Append("\n");
  }

  private: String PopOperands(const char* const name, int const count) {
    Stack_<String> operands;
    for (int i = 0; i < count; i++) {
      operands.Push(operand_stack_.Pop());
    }

    StringBuilder buf;
    buf.AppendFormat("(%s ", name);
    const char* comma = "";
    for (int i = 0; i < count; i++) {
      buf.Append(comma);
      comma = " ";
      buf.Append(operands.Pop());
    }
    buf.Append(")");

    return buf.ToString();
  }

  private: void PopAndRemember(const char* const name, int const count) {
    auto const str = PopOperands(name, count);
    operand_list_.Add(str);
    strbuf_.AppendFormat("#%d=", operand_list_.Count());
    strbuf_.Append(str);
    strbuf_.Append("\n");
  }

  private: void PushAndRemember(const char* const name, int const count) {
    PopAndRemember(name, count);
    operand_stack_.Push(String::Format("#%d#", operand_list_.Count()));
  }

  // [R]
  private: void Reset() {
    ASSERT(operand_stack_.IsEmpty());
    operand_list_.Clear();
    operand_list_.Add(String("Bool")); // 1
    operand_list_.Add(String("Int32")); // 2
    operand_list_.Add(String("Object")); // 3
    operand_list_.Add(String("String")); // 4
    operand_list_.Add(String("Void"));  // 5
    operand_list_.Add(String("%void")); // 6
  }

  DISALLOW_COPY_AND_ASSIGN(MockReaderCallback);
};

class FastReaderTest : public AbstractFaslTest {
  protected: String Load(Session& session, Stream& stream) {
    MockReaderCallback callback;
    FastReader reader(session, "mem", callback);
    for (;;) {
      uint8 buf[100];
      auto const count = stream.Read(buf, sizeof(buf));
      if (!count) break;
      reader.Feed(buf, count);
      if (reader.HasError()) break;
    }
    callback.Finish();
    return reader.HasError()
        ? "Failed to read"
        : callback.GetString();
  }
};

// class A {}
// class B : A {}
TEST_F(FastReaderTest, Class) {
  MemoryStream mem;
  MakeSampleClass(mem);

  char16 const expected[] =
    L"(FileMagic #7='EvO')\n"
    L"#9=(Class Zero 256 #8='A')\n"
    L"#11=(Class Zero 256 #10='B')\n"
    L"(RealizeClass #9# [Object])\n"
    L"(RealizeClass #11# [#9#])\n"
    L"(ClassContext #9#)\n"
    L"(ClassContext #11#)\n";

  MockSession session;
  String actual(Load(session, mem));
  EXPECT_STREQ(expected, actual.value());
}

// class Foo { Array<String> x; }
TEST_F(FastReaderTest, ConstructedClass) {
  MemoryStream mem;
  MakeSampleConstructedClass(mem);

  char16 const expected[] =
    L"(FileMagic #7='EvO')\n"
    L"#9=(Class Zero 256 #8='Foo')\n"
    L"(RealizeClass #9# [Object])\n"
    L"(ClassContext #9#)\n"
    L"#11=(Namespace Zero #10='Common')\n"
    L"#13=(ClassRef #11# #12='Array')\n"
    L"#14=(ConstructedClassRef #13# [String])\n"
    L"#16=(Field 64 #14# #15='x')\n";

  MockSession session;
  String actual(Load(session, mem));
  EXPECT_STREQ(expected, actual.value());
}

TEST_F(FastReaderTest, ConstructedMethod) {
  MemoryStream mem;
  MakeSampleConstructedMethod(mem);

  char16 const expected[] =
    L"(FileMagic #7='EvO')\n"
    L"#9=(Class Zero 256 #8='Foo')\n"
    L"(RealizeClass #9# [Object])\n"
    L"(ClassContext #9#)\n"
    L"#11=(MethodGroup #10='Bar')\n"
    L"#13=(TypeParam #12='T')\n"
    L"(RealizeTypeParam #13# [] 0)\n"
    L"#14=(ValuesType [])\n"
    L"#15=(FunctionType #14# #13#)\n"
    L"#16=(GenericMethod 64 [#13#] #15#)\n"
    L"#18=(MethodGroup #17='Baz')\n"
    L"#19=(FunctionType #14# String)\n"
    L"#20=(Method 64 #19#)\n"
    L"(FunctionBody #21# 3)\n"
    L"(Instruction Entry [])\n"
    L"(Instruction Jump [(Label 2)])\n"
    L"(Instruction Prologue #14# %r4 = [])\n"
    L"#22=(ValuesType [Int32])\n"
    L"(Instruction Values #22# %r5 = [(Int32 1)])\n"
    L"#23=(ConstructedMethodRef #16# [String])\n"
    L"(Instruction Call String %r6 = [#23# %5])\n"
    L"(Instruction Ret [%6])\n"
    L"(Instruction Exit [])\n";

  MockSession session;
  String actual(Load(session, mem));
  EXPECT_STREQ(expected, actual.value());
}

// class Foo { int x; }
TEST_F(FastReaderTest, Field) {
  MemoryStream mem;
  MakeSampleField(mem);

  char16 const expected[] =
    L"(FileMagic #7='EvO')\n"
    L"#9=(Class Zero 256 #8='Foo')\n"
    L"(RealizeClass #9# [Object])\n"
    L"(ClassContext #9#)\n"
    L"#11=(Field 64 Int32 #10='x')\n";

  MockSession session;
  String actual(Load(session, mem));
  EXPECT_STREQ(expected, actual.value());
}

// class Foo<T> { T x; }
TEST_F(FastReaderTest, GenericClass) {
  MemoryStream mem;
  MakeSampleGenericClass(mem);

  char16 const expected[] =
    L"(FileMagic #7='EvO')\n"
    L"#10=(TypeParam #9='T')\n"
    L"(RealizeTypeParam #10# [] 0)\n"
    L"#11=(GenericClass Zero 256 #8='Foo' [#10#])\n"
    L"(RealizeClass #11# [Object])\n"
    L"(ClassContext #11#)\n"
    L"#13=(Field 64 #10# #12='x')\n";

  MockSession session;
  String actual(Load(session, mem));
  EXPECT_STREQ(expected, actual.value());
}

// class Foo { extern int Bar() { ... } }
TEST_F(FastReaderTest, Method) {
  MemoryStream mem;
  MakeSampleMethod(mem);

  char16 const expected[] =
    L"(FileMagic #7='EvO')\n"
    L"#9=(Class Zero 256 #8='Foo')\n"
    L"(RealizeClass #9# [Object])\n"
    L"(ClassContext #9#)\n"
    L"#11=(MethodGroup #10='Bar')\n"
    L"#12=(ValuesType [])\n"
    L"#13=(FunctionType #12# Int32)\n"
    L"#14=(Method 68 #13#)\n";

  MockSession session;
  String actual(Load(session, mem));
  EXPECT_STREQ(expected, actual.value());
}

// class Foo { int Bar() { ... } }
TEST_F(FastReaderTest, MethodBody) {
  MemoryStream mem;
  MakeSampleMethodBody(mem);

  char16 const expected[] =
    L"(FileMagic #7='EvO')\n"
    L"#9=(Class Zero 256 #8='Foo')\n"
    L"(RealizeClass #9# [Object])\n"
    L"(ClassContext #9#)\n"
    L"#11=(MethodGroup #10='Bar')\n"
    L"#12=(ValuesType [])\n"
    L"#13=(FunctionType #12# Int32)\n"
    L"#14=(Method 64 #13#)\n"
    L"(FunctionBody #15# 3)\n"
    L"(Instruction Entry [])\n"
    L"(Instruction Jump [(Label 2)])\n"
    L"(Instruction Prologue #12# %r4 = [])\n"
    L"(Instruction Ret [(Int32 1234)])\n"
    L"(Instruction Exit [])\n";

  MockSession session;
  String actual(Load(session, mem));
  EXPECT_STREQ(expected, actual.value());
}

TEST_F(FastReaderTest, Property) {
  MemoryStream mem;
  MakeSampleProperty(mem);

  char16 const expected[] =
    L"(FileMagic #7='EvO')\n"
    L"#9=(Class Zero 256 #8='Foo')\n"
    L"(RealizeClass #9# [Object])\n"
    L"(ClassContext #9#)\n"
    L"#11=(MethodGroup #10='get_Size')\n"
    L"#12=(ValuesType [])\n"
    L"#13=(FunctionType #12# Int32)\n"
    L"#14=(Method 33024 #13#)\n"
    L"(Property 256 Int32 #16='Size')\n"
    L"(PropertyMember #17='get' #14#)\n"
    L"(FunctionBody #15# 3)\n"
    L"(Instruction Entry [])\n"
    L"(Instruction Jump [(Label 2)])\n"
    L"(Instruction Prologue #12# %r4 = [])\n"
    L"(Instruction Ret [(Int32 1234)])\n"
    L"(Instruction Exit [])\n";

  MockSession session;
  String actual(Load(session, mem));
  EXPECT_STREQ(expected, actual.value());
}

}  // IlTest
