// @(#)$Id$
//
// Evita Il - FastDumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Fasl_FastDumper_h)
#define INCLUDE_Il_Fasl_FastDumper_h

#include "../Functor.h"

#include "./BitWriter.h"
#include "./FaslOp.h"

#include "../Tasks/Tasklet.h"
#include "../../Common/Collections.h"
#include "../../Common/Io.h"
#include "../../Common/LocalMemoryZone.h"

#include "../Ir/Name.h"
#include "../Ir/Operands/Type.h"

namespace Il {
namespace Fasl {

using namespace Common::Collections;
using namespace Common::Io;
using namespace Il::Ir;
using Il::Tasks::Session;
using Il::Tasks::Tasklet;

class BitWriter;

class FastDumper : public Functor, public Tasklet {

  private: typedef HashMap_<const Name*, int> NameMap;
  private: typedef HashMap_<const Operand*, int> OperandMap;

  // zone_ should be the first member variable.
  private: LocalMemoryZone zone_;

  private: int m_iNameId;
  private: int m_iOperandId;

  private: NameMap m_oNameMap;
  private: OperandMap m_oOperandMap;

  private: BitWriter bit_writer_;
  private: Operand* m_pContextOperand;
  private: const SourceInfo* m_pSourceInfo;

  // ctor
  public: FastDumper(Session&, Stream&);
  public: ~FastDumper();

  // [D]
  // This is entry point of dumper.
  public: void DumpClass(FaslOp, const Class&);
  public: void DumpGenericClass(const GenericClass& pClass);
  public: void DumpTypes(const Type::Set&);

  // Note: These methods are marked "public" for calling from Functor
  // rather than clients of FastDumper.
  // [R]
  public: int Remember(const Operand*);

  // [W]
  public: void Write(FaslOp const eOp);
  public: void Write(Instruction* const pInstruction);
  public: void Write(const Name& name);
  public: void Write(const Name& r) { Write(&r); }
  private: void WriteOp(Op const eOp);

  public: void WriteDef(FaslOp const eOp, Operand* const pOperand);
  public: void WriteFieldDef(Field* const pField);
  public: void WriteFunctionDef(Function* const pFunction);
  public: void WriteMethodDef(Method* const pMethod);
  public: void WriteMethodGroupDef(MethodGroup* const pMethodGroup);

  public: void WriteModifiers(const int iModifiers);
  public: void WriteOutput(SsaOutput* const pOutput);

  public: void WriteRef(BBlock* const pBBlock);
  public: void WriteRef(int const iId);
  public: void WriteRef(const Operand* const pOperand);
  public: void WriteRef(const Operand& r) { WriteRef(&r); }

  private: void WriteSetContext(Operand* const pOperand);
  private: void WriteSourceInfo(const SourceInfo* pSourceInfo);

  public: void WriteV4(int const iValue, int const iMsb = 0);
  public: void WriteV5(int const iValue, int const iMsb = 0);
  public: void WriteV6(int const iValue, int const iMsb = 0);
  public: void WriteV7(int const iValue, int const iMsb = 0);
  public: void WriteV8(int const iValue, int const iMsb = 0);
  public: void WriteV8(int64 const iValue, int const iMsb = 0);
  public: void WriteVariableDef(Variable* const pVar);

  DISALLOW_COPY_AND_ASSIGN(FastDumper);
}; // FastDumper

} // Fasl
} // Il

#endif // !defined(INCLUDE_Il_Fasl_FastDumper_h)
