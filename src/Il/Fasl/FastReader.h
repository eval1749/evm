// @(#)$Id$
//
// Evita Il - FastReader
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Fasl_FastReader_h)
#define INCLUDE_Il_Fasl_FastReader_h

#include "../Functor.h"

#include "./BitReader.h"
#include "./FaslError.h"
#include "./FaslOp.h"

#include "../Tasks/Tasklet.h"
#include "../../Common/Collections.h"
#include "../../Common/LocalMemoryZone.h"

#include "../Ir/Name.h"
#include "../Ir/Operands/Type.h"

namespace Il {
namespace Fasl {

using namespace Common::Collections;
using namespace Il::Ir;
using Il::Tasks::Session;
using Il::Tasks::Tasklet;

class BitReader;
class FaslErrorInfo;

// Fasl reader.
class FastReader : public Tasklet {
  public: interface Callback {
    protected: Callback() {}

    // [H][A]
    public: virtual void HandleArray(int) = 0;
    public: virtual void HandleArrayType() = 0;

    // [H][C]
    public: virtual void HandleClass() = 0;
    public: virtual void HandleClassContext() = 0;
    public: virtual void HandleClassRef() = 0;
    public: virtual void HandleConstructedClassRef() = 0;
    public: virtual void HandleConstructedMethodRef() = 0;

    // [H][E]
    // TODO(yosi) 2012-02-06 FASL reader error should be ErrorInfo rather
    // than error code only.
    public: virtual void HandleError(const FaslErrorInfo&) = 0;

    // [H][F]
    public: virtual void HandleField(const SourceInfo&) = 0;
    public: virtual void HandleFileMagic() = 0;
    public: virtual void HandleFunction() = 0;
    public: virtual void HandleFunctionBody() = 0;
    public: virtual void HandleFunctionType() = 0;

    // [H][G]
    public: virtual void HandleGenericClass() = 0;
    public: virtual void HandleGenericMethod() = 0;

    // [H][I]
    public: virtual void HandleInt(int64) = 0;
    public: virtual void HandleInstruction(Op, const SourceInfo&) = 0;

    // [H][L]
    public: virtual void HandleLabel(int) = 0;
    public: virtual void HandleLiteral(const Literal&) = 0;

    // [H][M]
    public: virtual void HandleMethod() = 0;
    public: virtual void HandleMethodGroup() = 0;
    public: virtual void HandleMethodRef() = 0;

    // [H][N]
    public: virtual void HandleName(const char16*) = 0;
    public: virtual void HandleNamespace() = 0;

    // [H][O]
    public: virtual void HandleOutput(int) = 0;

    // [H][P]
    public: virtual void HandlePatchPhi() = 0;
    public: virtual void HandlePointerType() = 0;
    public: virtual void HandleProperty() = 0;
    public: virtual void HandlePropertyMember() = 0;

    // [H][R]
    public: virtual void HandleRealizeClass() = 0;
    public: virtual void HandleRef(int) = 0;
    public: virtual void HandleReset() = 0;

    // [H]
    public: virtual void HandleTypeParam() = 0;

    // [H][V]
    public: virtual void HandleValuesType() = 0;
    public: virtual void HandleVariable() = 0;

    // [H][Z]
    public: virtual void HandleZero() = 0;

    DISALLOW_COPY_AND_ASSIGN(Callback);
  };

  private: enum State {
    State_Char,
    State_Error,
    State_FaslOp,
    State_I4,
    State_I4_I4,
    State_I4_I4_I4,
    State_NameChar,
    State_I5,
    State_V4,
    State_V5,
    State_V6,
    State_V7,
    State_V8,
  };

  private: BitReader bit_reader_;
  private: Callback& callback_;
  private: int counter_;
  private: StringBuilder char_sink_;
  private: FaslOp fasl_op_;
  private: String file_name_;
  private: int64 intacc_;
  private: State state_;
  private: SourceInfo source_info_;

  // ctor
  public: FastReader(Session&, const String&, Callback&);
  public: virtual ~FastReader();

  // [C]
  private: void CallRef();

  // [E]
  public: void Error(const FaslErrorInfo&);
  private: int ExpectInt32();

  // [F]
  public: void Feed(const void*, uint);

  // [G]
  public: SourceInfo GetSourceInfo() const;

  // [H]
  public: bool HasError() const;

  // [I]
  private: void InvalidState();

  // [P]
  private: void Process();
  private: void ProcessName();
  private: void ProcessVn(int);

  // [S]
  private: void StartFaslOp();

  DISALLOW_COPY_AND_ASSIGN(FastReader);
}; // FastReader

} // Fasl
} // Il

#endif // !defined(INCLUDE_Il_Fasl_FastReader_h)
