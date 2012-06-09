// @(#)$Id$
//
// Evita Il - Ir - CodeEmitter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_CodeEmitter_h)
#define INCLUDE_Il_Tasks_CodeEmitter_h

#include "../Ir.h"
#include "./Session.h"

namespace Il {
namespace Tasks {

using namespace Il::Ir;

class CodeEmitter {
  private: BBlock& bblock_;
  private: const Instruction& ref_inst_;
  private: const Module& module_;
  private: Session& session_;

  // ctor
  public: CodeEmitter(Session&, const Instruction&);

  // properties
  public: BBlock& bblock() const { return bblock_; }
  public: const Module& module() const { return module_; }
  public: const Instruction& ref_inst() const { return ref_inst_; }
  public: Session& session() const { return session_; }
  public: MemoryZone& zone() const;

  // [A]
  public: const Register& Add(const Type&, const Operand&, const Operand&);

  // [C]
  public: const Register& Call(const Type&, const Operand&, const Operand&);
  public: const Register& Call(const Type&, const Operand&, const Operand&,
                               const Operand&);

  // [E]
  public: const Register& EltRef(const Register&, int);
  public: void Emit(const Instruction&);

  // [F]
  public: const Register& FieldPtr(const Operand&, const Field&);

  // [L]
  public: const Output& Load(const Operand&);

  // [M]
  public: const Register& Mul(const Type&, const Operand&, const Operand&);

  // [N]
  public: const Register& NewArray(const Type&, int);
  public: const BoolOutput& NewBoolOutput();
  public: const Literal& NewLiteral(bool);
  public: const Literal& NewLiteral(const Type&, int64);
  public: const Output& NewOutput(const Type&);
  public: const PseudoOutput& NewPseudoRegister();
  public: const Register& NewRegister();
  public: const Values& NewValues();

  // [S]
  public: const Register& Shl(const Type&, const Operand&, int);
  public: void Store(const SsaOutput&, const Operand&);
  public: const Register& Sub(const Type&, const Operand&, const Operand&);

  DISALLOW_COPY_AND_ASSIGN(CodeEmitter);
};

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_CodeEmitter_h)
