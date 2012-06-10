// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instruction_h)
#define INCLUDE_Il_Ir_Instruction_h

#include "../../Functor.h"
#include "../IrObject.h"
#include "../WithSourceInfo.h"

#include "../Operands/BoolOutput.h"
#include "../Operands/PseudoOutput.h"
#include "../Operands/Register.h"
#include "../Operands/Values.h"

namespace Il {
namespace Ir {

// Note: enum Op must include all opcode used in IL, CG, TARGET.
enum__(Op, uint) {
  Op_None = 0,

  #define DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
      Op_ ## mp_name,

  #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format)

  #define DEFINE_TARGET_INSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format)

  #include "./Instructions.inc"

  Op_Limit,
};

// Internal represenation of Instruction
// Note: Each client should implement Instruction::GetDescription static
// member function to provide client specific instruation description.
class Instruction
    : public IrObject_<Instruction, IrObject>,
      protected ChildList_<OperandBox, Instruction>,
      public ChildItem_<Instruction, BBlock>,
      public LocalObject,
      public WithIndex,
      public WithWorkArea,
      public WorkListItem_<Instruction>,
      public WithSourceInfo {
  CASTABLE_CLASS(Instruction);

  public: enum Arity {
      Arity_0,    // e.g. ENTRY
      Arity_1,
      Arity_2,
      Arity_3,    // e.g. BRANCH %bx %by %bz
      Arity_Any,
      Arity_MoreThanZero,
  };

  public: typedef Instruction* (*NewInstruction)();

  public: struct Description {
    const char* mnemonic_;
    Arity arity_;
    NewInstruction ctor_;
  };

  private: typedef ChildList_<OperandBox, Instruction> OperandBoxes;

  public: class EnumOperand : public OperandBoxes::Enum {
    private: typedef OperandBoxes::Enum Base;

    public: EnumOperand(const Instruction&r )
        : Base(static_cast<const OperandBoxes*>(&r)) {}

    public: EnumOperand(const Instruction* p)
        : Base(static_cast<const OperandBoxes*>(p)) {}

    public: Operand& operator*() const { return *Get(); }
    public: Operand* Get() const { return GetBox()->GetOperand(); }

    public: Register* GetRx() const {
      return Get()->DynamicCast<Register>();
    }

    public: OperandBox* GetBox() const { return Base::Get(); }
  };

  protected: Op const opcode_;
  protected: Output* output_;
  protected: const Type* output_type_;

  // ctor
  protected: Instruction(Op, const Type*, const Output*);

  // properties
  public: BBlock& bblock() const;
  public: String mnemonic() const;
  public: Op opcode() const { return opcode_; }
  public: Output& output() const { return *output_; }
  public: const Type& output_type() const { return *output_type_; }
  public: void set_output(const Output&);
  public: void set_output_type(const Type& ty) { output_type_ = &ty; }

  public: Operand& op0() const { return *GetSx(); }
  public: Operand& op1() const { return *GetSy(); }
  public: Operand& op2() const { return *GetSz(); }

  // [A]
  public: void AppendOperand(const Operand&);
  public: void AppendOperand(OperandBox& r) { AppendOperand(&r); }

  public: void AppendOperand(const Operand* p) { AppendOperand(*p); }
  public: void AppendOperand(OperandBox*);

  // [C]
  public: int CountOperands() const;

  // [E]
  public: bool Equals(const Instruction*) const;

  // [G]
  public: static Arity GetArity(Op const eOp);
  public: Arity GetArity() const;
  public: BBlock* GetBBlock() const { return m_pParent; }
  public: BBlock* GetBB() const { return m_pParent; }
  public: static const Description& LibExport GetDescription(Op);
  public: BoolOutput* GetBd() const;
  public: BoolOutput* GetBx() const;
  public: int GetIy() const;

  // For FaslLoader
  public: OperandBox* GetLastOperandBox() const;

  public: Literal* GetLx() const;
  public: Literal* GetLy() const;
  public: Literal* GetLz() const;
  public: static const char* LibExport GetMnemonic(Op const eOp);
  public: const char* GetMnemonic() const;
  public: Op GetOp() const { return opcode_; }
  public: OperandBox* GetOperandBox(int const iIndex) const;
  public: Operand* GetOperand(int const iIndex) const;
  public: Output* GetOutput() const;
  public: PseudoOutput* GetQd() const;
  public: PseudoOutput* GetQx() const;
  public: PseudoOutput* GetQy() const;
  public: PseudoOutput* GetQz() const;
  public: Register* GetRd() const;
  public: Register* GetRx() const;
  public: Register* GetRy() const;
  public: Register* GetRz() const;
  public: SsaOutput* GetSsaOutput() const;
  public: Operand* GetSx() const { return GetOperand(0); }
  public: Operand* GetSy() const { return GetOperand(1); }
  public: Operand* GetSz() const { return GetOperand(2); }
  public: Values* GetVd() const;
  public: Values* GetVx() const;
  public: Values* GetVy() const;

  // [I]
  // For inserting "this" on method call.
  public: const Operand* InsertOperandBefore(
      const Operand* const pSx,
      OperandBox* const pRef);

  public: OperandBox* InsertOperandBefore(
      OperandBox* const pBox,
      OperandBox* const pRef);

  public: bool IsRealized() const;
  public: virtual bool IsUseless()  const;

  // [R]
  public: virtual void Realize();
  public: void RemoveOperand(OperandBox*);

  // [N]
  public: static Instruction& LibExport New(MemoryZone&, Op);

  // [T]
  public: virtual String ToString() const override;

  // [U]
  public: virtual void Unrealize();

  DISALLOW_COPY_AND_ASSIGN(Instruction);
};

template<class TSelf, Op tOpcode, class TBase = GeneralInstruction>
class Instruction_
    : public WithCastable3_<TSelf, TBase, Op, const Type*, const Output*> {

  public: typedef Instruction_<TSelf, tOpcode, TBase> Base;
  public: typedef TSelf Self;

  public: static Op GettOpcode() { return tOpcode; }

  public: static const char* Kind_() {
    return Instruction::GetDescription(tOpcode).mnemonic_;
  }

  protected: Instruction_(const Type* const pType, const Output* const pOutput)
      : WithCastable3_(tOpcode, pType, pOutput) {}

  protected: Instruction_(const Type& outy, const Output& out)
      : WithCastable3_(tOpcode, &outy, &out) {}

  // [A]
  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  } // Apply

  // [N]
  public: static Instruction* New() { return new TSelf(); }

  DISALLOW_COPY_AND_ASSIGN(Instruction_);
};

template<class TSelf, class TBase = Instruction>
class InstructionFamily_
    : public WithCastable3_<TSelf, TBase, Op, const Type*, const Output*> {

  public: typedef InstructionFamily_<TSelf, TBase> Base;

  protected: InstructionFamily_(
      Op const opcode,
      const Type* const type,
      const Output* const output)
      : WithCastable3_(opcode, type, output) {}

  protected: InstructionFamily_(
      Op const opcode,
      const Type& outy,
      const Output& out)
      : WithCastable3_(opcode, &outy, &out) {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(InstructionFamily_);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instruction_h)
