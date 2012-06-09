// @(#)$Id$
//
// Evita Il - FastLoader
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Fasl_FastLoader_h)
#define INCLUDE_Il_Fasl_FastLoader_h

#include "./FaslError.h"
#include "./FaslOp.h"
#include "./FastReader.h"

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
class FaslErrorInfo;

// Fasl file loader.
class FastLoader : public Tasklet, protected FastReader::Callback {
  // ExecutorMain uses Callback::HandleClass for finding Main method in
  // loaded classes.
  public: interface Callback {
    protected: Callback() {}

    public: virtual void HandleClass(Class&) = 0;

    DISALLOW_COPY_AND_ASSIGN(Callback);
  };

  private: struct Context {
    BBlock* bblock_;
    ArrayList_<BBlock*> bblocks_;
    Class* class_;
    FrameReg* frame_reg_;
    Function* function_;
    MethodGroup* method_group_;
    Property* property_;

    Context()
        : bblock_(nullptr),
          class_(nullptr),
          frame_reg_(nullptr),
          function_(nullptr),
          method_group_(nullptr),
          property_(nullptr) {}

    void Reset() {
      bblock_ = nullptr;
      class_ = nullptr;
      frame_reg_ = nullptr;
      function_ = nullptr;
      method_group_ = nullptr;
      property_ = nullptr;
    }
  };

  private: typedef Array_<const Object*> ObjectArray;
  private: typedef ArrayList_<const Object*> ObjectList;
  private: typedef Stack_<const Object*> ObjectStack;
  private: typedef ArrayList_<const Output*> OutputList;

  private: Callback* callback_;
  private: Context context_;
  private: Namespace& global_ns_;
  private: ObjectList operand_list_;
  private: ObjectStack operand_stack_;
  private: OutputList output_list_;
  private: FastReader reader_;

  // We allocate objects in operand_list, e.g. ObjectArray, Int32, Int64, in
  // this zone.
  private: LocalMemoryZone zone_;

  // ctor
  public: FastLoader(Session&, Namespace&, const String&,
                     Callback* = nullptr);
  public: virtual ~FastLoader();

  // [E]
  private: void Error(FaslError);
  private: void Error(FaslError, Box);
  private: void Error(FaslError, Box, Box);
  private: ObjectArray& ExpectArray(const Object*);
  private: const Class& ExpectClass(const Object*);
  private: const ClassOrNamespace& ExpectClassOrNamespace(const Object*);
  private: const FunctionType& ExpectFunctionType(const Object*);
  private: const GenericClass& ExpectGenericClass(const Object*);
  private: const GenericMethod& ExpectGenericMethod(const Object*);
  private: int ExpectInt32(const Object*);
  private: const Method& ExpectMethod(const Object*);
  private: const MethodGroup& ExpectMethodGroup(const Object*);
  private: const Name& ExpectName(const Object*);
  private: const Namespace& ExpectNamespace(const Object*);
  private: const Type& ExpectType(const Object*);
  private: const TypeParam& ExpectTypeParam(const Object*);
  private: const ValuesType& ExpectValuesType(const Object*);

  // [F]
  public: void Feed(const void*, uint);
  public: void Finish();

  // [H]
  public: bool HasError() const;

  // [H][A]
  protected: virtual void HandleArray(int) override;
  protected: virtual void HandleArrayType() override;

  // [H][C]
  protected: virtual void HandleClass() override;
  protected: virtual void HandleClassContext() override;
  protected: virtual void HandleClassRef() override;
  protected: virtual void HandleConstructedClassRef() override;
  protected: virtual void HandleConstructedMethodRef() override;

  // [H][E]
  protected: virtual void HandleError(const FaslErrorInfo&) override;

  // [H][F]
  protected: virtual void HandleField(const SourceInfo&) override;
  protected: virtual void HandleFileMagic() override;
  protected: virtual void HandleFunction() override;
  protected: virtual void HandleFunctionBody() override;
  protected: virtual void HandleFunctionType() override;

  // [H][G]
  protected: virtual void HandleGenericClass() override;
  protected: virtual void HandleGenericMethod() override;

  // [H][I]
  protected: virtual void HandleInt(int64) override;
  protected: virtual void HandleInstruction(Op, const SourceInfo&) override;

  // [H][L]
  protected: virtual void HandleLabel(int) override;
  protected: virtual void HandleLiteral(const Literal&) override;

  // [H][M]
  protected: virtual void HandleMethod() override;
  protected: virtual void HandleMethodGroup() override;
  protected: virtual void HandleMethodRef() override;

  // [H][N]
  protected: virtual void HandleName(const char16*) override;
  protected: virtual void HandleNamespace() override;

  // [H][O]
  protected: virtual void HandleOutput(int) override;

  // [H][P]
  protected: virtual void HandlePatchPhi() override;
  protected: virtual void HandlePointerType() override;
  protected: virtual void HandleProperty() override;
  protected: virtual void HandlePropertyMember() override;

  // [H][R]
  protected: virtual void HandleRealizeClass() override;
  protected: virtual void HandleRef(int) override;
  protected: virtual void HandleReset() override;

  // [H][T]
  protected: virtual void HandleTypeParam() override;

  // [H][V]
  protected: virtual void HandleValuesType() override;
  protected: virtual void HandleVariable() override;

  // [H][Z]
  protected: virtual void HandleZero() override;

  // [P]
  private: bool Pop(Object**, int);
  private: void Push(const Object&);
  private: void PushAndRemember(const Operand&);
  private: void PushRef(int);

  // [R]
  private: void Remember(const Operand&);
  private: void ResetAll();
  private: void ResetOutput();

  DISALLOW_COPY_AND_ASSIGN(FastLoader);
}; // FastLoader

} // Fasl
} // Il

#endif // !defined(INCLUDE_Il_Fasl_FastLoader_h)
