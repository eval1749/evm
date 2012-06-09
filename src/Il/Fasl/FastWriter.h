// @(#)$Id$
//
// Evita Il - FastWriter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Fasl_FastWriter_h)
#define INCLUDE_Il_Fasl_FastWriter_h

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

class FastWriter : public Tasklet {
  private: typedef HashMap_<const Name*, int> NameMap;
  private: typedef HashMap_<const Operand*, int> OperandMap;

  // zone_ should be the first member variable.
  private: LocalMemoryZone zone_;

  private: int operand_counter_;

  private: NameMap name_map_;
  private: OperandMap operand_map_;
  private: HashSet_<SsaOutput*> phi_operand_set_;
  private: Type::Set type_set_;

  private: BitWriter bit_writer_;
  private: Namespace& global_ns_;
  private: const SourceInfo* source_info_;

  // ctor
  public: FastWriter(Session&, Stream&, Namespace&);
  public: virtual ~FastWriter();

  // [A]
  public: void Add(const Type&);

  // [F]
  public: void Finish();

  // [G]
  private: int GetRefId(const Operand&) const;

  // [I]
  private: bool IsWritten(const Operand&) const;

  // [N]
  private: SsaOutput* NeedPhiOperandPatch(const Output&) const;
  private: SsaOutput* NeedPhiOperandPatch(const PhiOperandBox&) const;

  // [R]
  private: int Remember(const Operand* p) { return Remember(*p); }
  private: int Remember(const Operand&);

  // [U]
  private: void UnexpectedOperand(const Operand&);

  // [W]
  private: void WriteArrayFaslOp(int);
  private: void WriteBBlock(const BBlock&);
  private: void WriteClassDef(const Class&);
  private: void WriteGenericClassDef(const GenericClass& pClass);
  private: void WriteClassOrNamespaceRef(const ClassOrNamespace&);
  private: void WriteFaslOp(FaslOp);
  private: void WriteFaslOpV8(FaslOp, uint);
  private: void WriteFunctionBody(const Function&);
  private: void WriteFunctionDef(const Function&);
  private: bool WriteIfRemembered(const Operand&);
  private: void WriteInstruction(const Instruction&);
  private: void WriteLabel(const Label&);
  private: void WriteLiteral(const Literal&);
  private: void WriteMethod(const Method&);
  private: void WriteModifiers(int);
  private: void WriteNamespaceRef(const Namespace&);
  private: void WriteName(const Name&);
  private: void WriteOperand(const Operand&);
  private: void WriteOutputRef(const Output&);
  private: void WriteProperty(const Property&);
  private: void WritePushInt32(int);
  private: void WriteRef(const Operand&);
  private: void WriteRefId(int);
  private: void WriteSourceInfo(const SourceInfo*);
  private: void WriteTypeRef(const Type&);

  private: void WriteUInt32V4(uint32, uint32 = 0);
  private: void WriteUInt32V5(uint32, uint32 = 0);
  private: void WriteUInt32V6(uint32, uint32 = 0);
  private: void WriteUInt32V7(uint32, uint32 = 0);
  private: void WriteUInt32V8(uint32, uint32 = 0);
  private: void WriteUInt64V8(uint64, uint32 = 0);

  DISALLOW_COPY_AND_ASSIGN(FastWriter);
}; // FastWriter

} // Fasl
} // Il

#endif // !defined(INCLUDE_Il_Fasl_FastWriter_h)
