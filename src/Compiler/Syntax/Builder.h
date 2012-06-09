// @(#)$Id$
//
// Evita Builder
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_Builder_h)
#define INCLUDE_evc_Builder_h

namespace Compiler {

using Il::Ir::Function;
using Il::Ir::BBlock;

class Builder {
  private: BBlock* curr_block_;
  private: const Function& function_;
  private: const Module& module_;
  private: int num_insts_;
  private: Builder* const outer_;
  private: SourceInfo source_info_;
  private: BBlock* succ_block_;

  private: Builder(Builder*, const Function&, const SourceInfo&);
  public: ~Builder();

  // properties
  public: const Module& module() const { return module_; }
  public: const SourceInfo& source_info() const { return source_info_; }
  public: MemoryZone& zone();

  public: void set_source_info(const SourceInfo& source_info) {
    source_info_ = source_info;
  }

  // [E]
  public: const BoolOutput& EmitBool(Op, const Operand&, bool);
  public: const BoolOutput& EmitBool(Op, const Operand&, const Operand&);
  public: void EmitBranchI(const BoolOutput&, const BBlock&, const BBlock&);
  public: void EmitCallI(const Operand&, const Operand&);
  public: void EmitCallI(const Operand&, const Values&);
  public: const Register& EmitCallI(const Type&, const Operand&);
  public: const Register& EmitCallI(const Type&, const Operand&,
                                    const Operand&);
  public: const Register& EmitCallI(const Type&, const Operand&,
                                    const Operand&, const Operand&);
  public: const Register& EmitCallI(const Type&, const Operand&,
                                    const Values&);
  public: void EmitCloses(Instruction&, FrameReg*, FrameReg*);
  public: void EmitI(Instruction&);
  public: const Register& EmitI(Op, const Type&, const Operand&);
  public: const Register& EmitI(Op, const Type&, const Operand&,
                                const Operand&);
  public: void EmitJumpI(BBlock&);
  public: const Register& EmitLoadI(const Operand&);
  public: const Register& EmitNameRefI(const NameRef&);
  public: const Register& EmitNameRefI(const Type&, const Operand&,
                                       const NameRef&);
  public: const Register& EmitNullI(const Type&);
  public: PhiI& EmitPhiI(const Type&);
  public: void EmitRetI(const Operand&);
  public: void EmitStoreI(const Operand&, const Operand&);
  public: const Values& EmitValuesI();
  public: const Values& EmitValuesI(const Operand&);
  public: const Values& EmitValuesI(const Operand&, const Operand&);
  public: Builder* EndFunction();

  // [G]
  public: BBlock* GetCurr() const { return curr_block_; }
  public: BBlock& GetSucc() const;

  // [N]
  public: BBlock& NewBBlock();
  public: const BoolOutput& NewBoolOutput();
  public: const Literal& NewLiteral(bool);
  public: const Literal& NewLiteral(const Type&, int64);
  public: const Output& NewOutput(const Type&);
  public: const Register& NewRegister();
  public: BBlock& NewSucc();
  public: const Values& NewValues();

  // [R]
  public: void ResetCurr();

  // [S]
  public: void SetCurr(BBlock&);
  public: void SetCurrSucc(BBlock&, BBlock&);
  public: void SetCurrSucc(BBlock&);
  public: void SetSucc(BBlock&);
  private: void SetUpMethodBody(Function&, MethodDef&);
  public: static Builder& StartFunction(Builder*, const Function&);

  public: static Builder& StartMethodFunction(
      Builder*,
      MethodDef&,
      const SourceInfo&);

  DISALLOW_COPY_AND_ASSIGN(Builder);
};

} // Compiler

#endif // !defined(INCLUDE_evc_Builder_h)
