#include "precomp.h"
// @(#)$Id$
//
// Evita Ee X86 - CodeManager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86CodeDesc.h"

#include "../../../Common/Io.h"
#include "../../../Om.h"
#include "../../../Om/Frame.h"
#include "../../../Om/Object.h"
#include "../../../Om/Thread.h"
#include "../Cg/X86Arch.h"
#include "./X86CodeManager.h"

namespace Executor {
extern TextWriter* StdOut;
} // Executor

namespace Native {
using namespace Executor;


static void __fastcall ConsoleWrite_String(Om::String* string) {
  String nativeString(string->elements(), string->length());
  StdOut->Write(nativeString);
}

static void __fastcall ConsoleWriteLine() {
  StdOut->WriteLine();
}

static void __fastcall ConsoleWriteLine_String(Om::String* string) {
  String nativeString(string->elements(), string->length());
  StdOut->WriteLine(nativeString);
}

static void __fastcall InvokeInterfaceMethod(
    Om::Object* object,
    Il::Ir::Method* method) {
  ASSERT(object != nullptr);
  ASSERT(method != nullptr);
  // NYI: search method from object interface map.
}

} // Native

namespace Ee {
namespace X86 {

// Defined in X86Init.cpp
extern Function* gInvokeInterfaceMethod;

#define Q(mp_name) Name::Intern(# mp_name)

namespace {

using Common::Collections::ArrayList_;
using namespace Ee;
using namespace Il::Ir;
using namespace Il::X86;

//    7 6  5 4 3  2 1 0    7 6 5 4 3 2 1 0
//   +----+------+------+ +---+-----+------+ +----------------+
//   |mod | reg  |  r/m | | S | idx | base | |  disp8/disp32  |
//   +----+------+------+ +----------------+ +----------------+
//      r/m 4 sib (esp)
//      r/m 5 disp32 (ebp)
class Ea {
  private: Reg base_;
  private: Reg index_;
  private: int offset_;
  private: Scale scale_;

  public: Ea(Reg base)
    : base_(base), index_($ESP), offset_(0),
      scale_(Scale_None) {}

  public: Ea(Reg base, int offset)
    : base_(base), index_($ESP), offset_(offset),
      scale_(Scale_None) {}

  public: Ea(Reg base, Reg index, int offset)
    : base_(base), index_(index), offset_(offset),
      scale_(Scale_None) {}

  public: Ea(Reg base, Reg index, Scale scale, int offset)
    : base_(base), index_(index), offset_(offset),
      scale_(scale) {}

  public: int8 disp8() const { return static_cast<uint8>(offset_ & 0xFF); }
  public: int32 disp32() const { return offset_; }

  public: Mod mod() const {
    if (offset_ == 0 && base_ != $EBP) {
      return Mod_Disp0;
    }

    if (!(offset_ & ~0xFF)) {
      return Mod_Disp8;
    }

    return Mod_Disp32;
  }

  public: Reg reg() const { return base_; }

  public: Reg rm() const { 
    return NeedSib() ? static_cast<Reg>(Rm_Sib) : base_; 
  }

  public: uint8 sib() const {
    if (base_ == $ESP) {
      // Scale_1*$ESP+$ESP == base+disp. $ESP can't be index.
      return 0x24;
    }

    ASSERT(index_ != $ESP);
    return HasBase()
        ? static_cast<uint8>(scale_ | ((index_ & 7) << 3) | (base_ & 7))
        : static_cast<uint8>(scale_ | ((index_ & 7) << 3) | $EBP);
  }

  public: bool HasBase() const { return base_ != static_cast<Reg>(-1); }
  public: bool HasIndex() const { return scale_ != Scale_None; }

  public: bool NeedSib() const {
    return HasIndex() || base_ == $ESP;
  }
};

class ContextEa : public Ea {
  public: ContextEa(int offset) : Ea($EBP, offset) {}
};

class AbstractAssembler {
  private: ArrayList_<CodeDesc::Annotation> annotations_;
  private: ArrayList_<uint8> codes_;

  protected: AbstractAssembler() {}

  // [A]
  protected: void Annotate(CodeDesc::Annotation::Kind kind) {
    annotations_.Add(CodeDesc::Annotation(kind, codes_.Count()));
  }

  // [E]
  protected: void EmitInt32(int32 const i32) {
    EmitUInt8(static_cast<uint8>(i32 & 0xFF));
    EmitUInt8(static_cast<uint8>((i32 >> 8) & 0xFF));
    EmitUInt8(static_cast<uint8>((i32 >> 16) & 0xFF));
    EmitUInt8(static_cast<uint8>((i32 >> 24) & 0xFF));
  }

  protected: void EmitUInt8(uint8 u8) {
    codes_.Add(u8);
  }

  protected: void EmitEa(const Ea& ea) {
    EmitEa(ea, static_cast<Opext>(ea.rm() & 7));
  }

  protected: void EmitEa(const Ea& ea, Opext const opext) {
    if (ea.HasBase()) {
      // [base]
      // [base+disp8]
      // [base+disp32]
      auto const mod = ea.mod();
      EmitUInt8(ModRm(mod, opext, ea.rm()));

      if (ea.NeedSib()) {
        // [base+index*scale]
        // [base+index*scale+disp8]
        // [base+index*scale+disp32]
        EmitUInt8(ea.sib());
      }

      if (mod == Mod_Disp8) {
        EmitInt8(ea.disp8());

      } else if (mod == Mod_Disp32) {
        EmitInt32(ea.disp32());
      }

    } else {
      if (ea.HasIndex()) {
        // [index*scale+disp32]
        EmitUInt8(ModRm(Mod_Disp0, opext, Rm_Sib));
        EmitUInt8(ea.sib());
      } else {
        // [disp32]
        EmitUInt8(ModRm(Mod_Disp0, opext, Rm_Disp32));
      }
      EmitInt32(ea.disp32());
    }
  }

  protected: void EmitInt8(int8 const i8) {
    EmitUInt8(static_cast<uint8>(i8));
  }

  protected: void EmitOp(int const opcode) {
    if (opcode > 0xFFFF) {
      EmitUInt8(static_cast<uint8>((opcode >> 16) & 0xFF));
    }

    if (opcode > 0xFF) {
      EmitUInt8(static_cast<uint8>((opcode >> 8) & 0xFF));
    }

    EmitUInt8(static_cast<uint8>(opcode & 0xFF));
  }

  public: CodeDesc& NewCodeDesc(const Function& fun) {
    auto& code_desc = CodeManager::Get()->NewCodeDesc(
        fun,
        annotations_,
        codes_.Count());
    codes_.CopyTo(static_cast<uint8*>(code_desc.codes()));
    code_desc.FixAfterMove();
    return code_desc;
  }
};

class Assembler : public AbstractAssembler {
  // [A]
  public: Assembler& Add(Reg const rd, int i32) {
    EmitOp(op_ADD_Ev_Iz);
    EmitUInt8(ModRm(Mod_Reg, opext_ADD_Ev_Iz, rd));
    EmitInt32(i32);
    return *this;
  }

  // [C]
  public: Assembler& Call(void* const pv) {
    EmitOp(op_CALL_Jv);
    Annotate(CodeDesc::Annotation::Kind_NativeCallee);
    EmitInt32(reinterpret_cast<int>(pv));
    return *this;
  }

  // [E]
  public: Assembler& Epilogue() {
    Add($ESP, sizeof(void*)); // discard ToNativeFrame.type_
    Pop(ContextEa(offsetof(Om::Thread::Context, frame_)));
    return *this;
  }

  // [J]
  public: Assembler& Jmp(void* const pv) {
    EmitOp(op_JMP_Jv);
    Annotate(CodeDesc::Annotation::Kind_NativeCallee);
    EmitInt32(reinterpret_cast<int>(pv));
    return *this;
  }

  // [M]
  public: Assembler& Mov(const Ea& ea, Reg const rx) {
    EmitOp(op_MOV_Ev_Gv);
    EmitEa(ea, static_cast<Opext>(rx & 7));
    return *this;
  }

  public: Assembler& Mov(Reg const rd, const Ea& ea) {
    EmitOp(op_MOV_Gv_Ev);
    EmitEa(ea, static_cast<Opext>(rd & 7));
    return *this;
  }

  public: Assembler& Mov(Reg const rd, Reg const rx) {
    EmitOp(op_MOV_Gv_Ev);
    EmitUInt8(ModRm(Mod_Reg, rd, rx));
    return *this;
  }

  // [P]
  public: Assembler& Pop(const Ea& ea) {
    EmitOp(op_POP_Ev);
    EmitEa(ea, opext_POP_Ev);
    return *this;
  }

  public: Assembler& Push(const Ea& ea) {
    EmitOp(op_PUSH_Ev);
    EmitEa(ea, opext_PUSH_Ev);
    return *this;
  }

  public: Assembler& Prologue() {
    Push(ContextEa(offsetof(Om::Thread::Context, frame_)));
    Push(Om::FrameType_ToNative);
    Mov(ContextEa(offsetof(Om::Thread::Context, frame_)), $ESP);
    return *this;
  }

  public: Assembler& Push(int const i32) {
    EmitOp(op_PUSH_Iz);
    EmitInt32(i32);
    return* this;
  }

  // [R]
  public: Assembler& Ret() {
    EmitOp(op_RET);
    return *this;
  }
};

// Stack frame at wapper after calling native function.
//          +-----------------------+
//  esp-4   |    RA to wrapper      |
//          +-----------------------+
//  esp+0   |    ToNative           | <- $context.frame_
//          +-----------------------+
//  esp+4   |    $context.frame_    |
//          +-----------------------+
//  esp+8   |    RA to managed code |
//          +-----------------------+

// See CallManagedFunction in X86CodeDesc.cpp
class Trampoline {
  private: Assembler asm_;

  // ctor
  public: Trampoline(void* const nativefn, const FunctionType& funty) {
    // Push ToNative frame.
    asm_.Prologue();

    auto const num_params = funty.params_type().Count();

    // TODO(yosi) 2012-03-19 NYI Trampoline for float32/float64 parameters

    if (num_params >= 1) {
      asm_.Mov($ECX, $EAX);
    }

    if (num_params >= 3) {
      CAN_NOT_HAPPEN();
    }

    asm_.Call(nativefn);

    // Pop ToNative frame.
    asm_.Epilogue();

    // TODO(yosi) 2012-03-19 NYI Trampoline for float32/float64 returns
    asm_.Ret();
  }

  // [N]
  public: CodeDesc& NewCodeDesc(const Function& fun) {
    return asm_.NewCodeDesc(fun);
  }
};

static void InstallInvokeInterfaceMethod() {
  auto& module = *new Module();
  auto& fun = module.NewFunction(
      nullptr,
      Function::Flavor_Toplevel,
      Name::Intern("InvokeInterfaceMethod"));
  Assembler as;
  as.Prologue();
  as.Mov(ContextEa(offsetof(Om::Thread::Context, values_[0])), $r0);
  as.Mov(ContextEa(offsetof(Om::Thread::Context, values_[1])), $r1);
  as.Mov(ContextEa(offsetof(Om::Thread::Context, values_[2])), $r2);
  as.Mov(ContextEa(offsetof(Om::Thread::Context, values_[3])), $r3);
  as.Mov(ContextEa(offsetof(Om::Thread::Context, values_[4])), $r4);
  as.Mov(ContextEa(offsetof(Om::Thread::Context, values_[5])), $rn);
  as.Mov($ECX, $EAX);
  as.Mov($EDX, ContextEa(offsetof(Om::Thread::Context, param_)));
  as.Call(Native::InvokeInterfaceMethod);
  as.Epilogue();
  as.Ret();
  fun.set_code_desc(as.NewCodeDesc(fun));
  gInvokeInterfaceMethod = &fun;
}

static void InstallNativeMethod(
    void* nativefn,
    Namespace& ns,
    const char* class_name,
    const char* method_name,
    const Type& return_type,
    const ValuesType&  params_type) {
  auto& klass = *ns.Find(Name::Intern(class_name))->StaticCast<Class>();
  auto& mtg = *klass.Find(Name::Intern(method_name))->StaticCast<MethodGroup>();
  auto& mty = FunctionType::Intern(return_type, params_type);
  auto& mt = *mtg.Find(mty);

  auto& module = *new Module();
  auto& fun = module.NewFunction(
      nullptr, Function::Flavor_Toplevel, Name::Intern(method_name));

  ValuesTypeBuilder paramsty_builder;
  if (!mt.IsStatic()) {
    paramsty_builder.Append(klass);
  }

  foreach (ValuesType::Enum, en, params_type) {
    paramsty_builder.Append(en.Get());
  }

  auto& funty = FunctionType::Intern(
      return_type,
      paramsty_builder.GetValuesType());

  fun.SetFunctionType(funty);

  Trampoline trampoline(nativefn, funty);
  auto& code_desc = trampoline.NewCodeDesc(fun);
  fun.set_code_desc(code_desc);
  fun.SetMethod(mt);
  mt.SetFunction(fun);
}

} // namespace

} // X86

void RuntimeInit() {
  X86::InstallNativeMethod(
    Native::ConsoleWrite_String,
    *Namespace::Common,
    "Console",
    "Write",
    *Ty_Void,
    ValuesType::Intern(*Ty_String));

  X86::InstallNativeMethod(
    Native::ConsoleWriteLine,
    *Namespace::Common,
    "Console",
    "WriteLine",
    *Ty_Void,
    ValuesType::Intern());

  X86::InstallNativeMethod(
    Native::ConsoleWriteLine_String,
    *Namespace::Common,
    "Console",
    "WriteLine",
    *Ty_Void,
    ValuesType::Intern(*Ty_String));

  X86::InstallInvokeInterfaceMethod();
}

} // Ee
