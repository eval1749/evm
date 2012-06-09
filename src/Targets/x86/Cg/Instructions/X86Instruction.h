// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_CgX86_X86Instructions_X86Instruction_h)
#define INCLUDE_evm_CgX86_X86Instructions_X86Instruction_h

namespace Il {
namespace Cg {

class X86Instruction
    : public InstructionFamily_<X86Instruction, CgInstruction> {

 CASTABLE_CLASS(X86Instruction)

 protected: X86Instruction(
    Op const opcode,
    const Type* outy,
    const Output* out)
    : Base(opcode, outy, out) {}

 protected: X86Instruction(
    Op const opcode,
    const Type& outy,
    const Output& out)
    : Base(opcode, outy, out) {}

  private: virtual void Apply(Functor*) override { CAN_NOT_HAPPEN(); }

  private: virtual void Apply(CgFunctor* functor) override {
    functor->Process(this);
  }

  public: virtual void Apply(X86Functor* functor) {
    functor->Process(this);
  }

  DISALLOW_COPY_AND_ASSIGN(X86Instruction);
};

template<class TSelf, Op tOpcode, class TBase = X86Instruction>
class X86Instruction_ : public Instruction_<TSelf, tOpcode, TBase> {
  public: typedef X86Instruction_<TSelf, tOpcode, TBase> Base;

  protected: X86Instruction_(
      const Type& outy,
      const Output& out)
      : Instruction_(&outy, &out) {}

  // [A]
  public: virtual void Apply(X86Functor* const functor) {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(X86Instruction_);
};

/// <summary>
///     X86 BOOL instruction used for loading floating-point number.
///   <example>
///     x86BOOL bool %bd &lt;= %ix int
///   </example>
/// </summary>
class x86BoolI : public X86Instruction_<x86BoolI, Op_x86Bool> {
  // ctor
  public: x86BoolI() : Base(*Ty_Void, *Void) {}

  public: x86BoolI(
      const BoolOutput& out,
      const Operand& src,
      X86::Tttn const tttn)
      : Base(*Ty_Bool, out) {
    AppendOperand(src);
    AppendOperand(new TttnLit(tttn));
  }

  // [G]
  public: X86::Tttn GetTttn() const {
    return GetSy()->StaticCast<TttnLit>()->value();
  }

  // Note: Since x86BoolI may be shared with instructions, we should not
  // modify tttn.

  DISALLOW_COPY_AND_ASSIGN(x86BoolI);
};

// X86CLC
class x86ClcI : public X86Instruction_<x86ClcI, Op_x86Clc> {
  public: x86ClcI() : Base(*Ty_Void, *Void) {}
  DISALLOW_COPY_AND_ASSIGN(x86ClcI);
};

/// <summary>
///     X86 CVTFLOAT instruction.
///   <example>
///     x86CVTFLOAT float %rd &lt;= %rx
///   </example>
/// </summary>
class x86CvtFloatI : public X86Instruction_<x86CvtFloatI, Op_x86CvtFloat> {
  // ctor
  public: x86CvtFloatI() : Base(*Ty_Void, *Void) {}

  public: x86CvtFloatI(
      const Type& outy,
      const Register& out,
      const Operand& src)
      : Base(outy, out) {
    AppendOperand(src);
  }

  DISALLOW_COPY_AND_ASSIGN(x86CvtFloatI);
};

/// <summary>
///     X86 CVTINT instruction.
///   <example>
///     x86CVTINT int %rd &lt;= %rx
///   </example>
/// </summary>
class x86CvtIntI : public X86Instruction_<x86CvtIntI, Op_x86CvtInt> {
  // ctor
  public: x86CvtIntI() : Base(*Ty_Void, *Void) {}

  public: x86CvtIntI(
      const Type& outy,
      const Register& out,
      const Operand& src)
      : Base(outy, out) {
    AppendOperand(src);
  }

  DISALLOW_COPY_AND_ASSIGN(x86CvtIntI);
};

/// <summary>
///     X86 ENCODE instruction used for loading floating-point number.
///   <example>
///     x86ENCODE int %rd &lt;= %rx
///   </example>
/// </summary>
class x86EncodeI : public X86Instruction_<x86EncodeI, Op_x86Encode> {
  // ctor
  public: x86EncodeI() : Base(*Ty_Void, *Void) {}

  public: x86EncodeI(
      const Type& outy,
      const Output& out,
      const Output& src)
      : Base(outy, out) {
    AppendOperand(src);
  }
  DISALLOW_COPY_AND_ASSIGN(x86EncodeI);
};

/// <summary>
///     X86 integer CMP instruction.
///   <example>
///     X86CMP int %id &lt;= %sx %sy
///   </example>
/// </summary>
class x86CmpI : public X86Instruction_<x86CmpI, Op_x86Cmp> {
  // ctor
  public: x86CmpI() : Base(*Ty_Void, *Void) {}

  public: x86CmpI(
      const PseudoOutput& out,
      const Operand& src0,
      const Operand& src1)
      : Base(*Ty_Int32, out) {
    AppendOperand(src0);
    AppendOperand(src1);
  }

  DISALLOW_COPY_AND_ASSIGN(x86CmpI);
};

/// <summary>
///     X86 integer CmpF32 instruction.
///   <example>
///     X86CmpF32 int %id &lt;= %sx %sy
///   </example>
/// </summary>
class x86CmpF32I : public X86Instruction_<x86CmpF32I, Op_x86CmpF32> {
  // ctor
  public: x86CmpF32I() : Base(*Ty_Void, *Void) {}

  public: x86CmpF32I(
      const PseudoOutput& out,
      const Operand& src0,
      const Operand& src1)
      : Base(*Ty_Int32, out) {
    AppendOperand(src0);
    AppendOperand(src1);
  }

  DISALLOW_COPY_AND_ASSIGN(x86CmpF32I);
};

/// <summary>
///     X86 integer CmpF64 instruction.
///   <example>
///     X86CmpF64 int %id &lt;= %sx %sy
///   </example>
/// </summary>
class x86CmpF64I : public X86Instruction_<x86CmpF64I, Op_x86CmpF64> {
  // ctor
  public: x86CmpF64I() : Base(*Ty_Void, *Void) {}

  public: x86CmpF64I(
      const PseudoOutput& out,
      const Operand& src0,
      const Operand& src1)
      : Base(*Ty_Int32, out) {
    AppendOperand(src0);
    AppendOperand(src1);
  }

  DISALLOW_COPY_AND_ASSIGN(x86CmpF64I);
};

// x86LEA int %rd <= %rx iy
class x86LeaI : public X86Instruction_<x86LeaI, Op_x86Lea> {
  public: static Instruction* New() { return new Self(); }

  // ctor
  public: x86LeaI() : Base(*Ty_Void, *Void) {}

  public: x86LeaI(
      const Type& outy,
      const Output& out,
      const Output& src,
      int32 const ofs)
      : Base(outy, out) {
    AppendOperand(src);
    AppendOperand(new Int32Literal(ofs));
  }

  DISALLOW_COPY_AND_ASSIGN(x86LeaI);
};

// X86STC
class x86StcI : public X86Instruction_<x86StcI, Op_x86Stc> {
  public: x86StcI() : Base(*Ty_Void, *Void) {}
  DISALLOW_COPY_AND_ASSIGN(x86StcI);
};

// X86TEST int %id <= %sx %sy
class x86TestI : public X86Instruction_<x86TestI, Op_x86Test> {
  // ctor
  public: x86TestI() : Base(*Ty_Void, *Void) {}

  public: x86TestI(
      const PseudoOutput& out,
      const Operand& src0,
      const Operand& src1)
      : Base(*Ty_Int32, out) {
    AppendOperand(src0);
    AppendOperand(src1);
  }

  DISALLOW_COPY_AND_ASSIGN(x86TestI);
};

/// <summary>
///     X86 ZERO instruction used for loading floating-point +0.0.
///   <example>
///     x86ZERO int %rd
///   </example>
/// </summary>
class x86ZeroI : public X86Instruction_<x86ZeroI, Op_x86Zero> {
  public: x86ZeroI() : Base(*Ty_Void, *Void) {}
  public: x86ZeroI(const Type& outy, const Register& out)
      : Base(outy, out) {}
  DISALLOW_COPY_AND_ASSIGN(x86ZeroI);
};

}  // Cg
}  // Il

#endif // !defined(INCLUDE_evm_CgX86_X86Instructions_X86Instruction_h)
