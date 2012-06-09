// @(#)$Id$
//
// Evita Il - IR - HtmlFormatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Io_HtmlFormatter_h)
#define INCLUDE_Il_Io_HtmlFormatter_h

#include "../Functor.h"

namespace Il {
namespace Io {

using namespace Il::Ir;
class LogWriter;

class HtmlFormatter : public Functor {

    private: XhtmlWriter* const m_pWriter;

    // ctor
    public: explicit HtmlFormatter(
        XhtmlWriter* const pWriter);

    public: virtual ~HtmlFormatter() {}

    // [G]
    public: XhtmlWriter* GetWriter() const { return m_pWriter; }

    // [P][A]
    public: virtual void Process(ArrayType* const pArrayType) override;

    // [P][B]
    public: virtual void Process(BBlock* const pBBlock) override;
    public: virtual void Process(BoolType* const pType) override;
    public: virtual void Process(BoolOutput* const pBool) override;

    // [P][C]
    public: virtual void Process(Class* const pClass) override;
    public: virtual void Process(ConstructedClass* const pClass) override;

    // [P][F]
    public: virtual void Process(Field* const pField) override;
    public: virtual void Process(Function* const pFunction) override;
    public: virtual void Process(Module* const pModule) override;
    public: virtual void Process(FunctionType* const pFunctionType) override;

    // [P][G]
    public: virtual void Process(GenericClass* const pClass) override;

    // [P][I]
    public: virtual void Process(Instruction* const pInstruction) override;

    // [P][L]
    public: virtual void Process(Label* const pLabel) override;
    public: virtual void Process(Literal* const pLiteral) override;

    // [P][M]
    public: virtual void Process(Method* const pMethod) override;
    public: virtual void Process(MethodGroup* const pMethodGroup) override;

    // [P][N]
    public: virtual void Process(NameRef* const pNameRef) override;
    public: virtual void Process(Namespace* const pNamespace) override;
    //public: virtual void Process(NullOperand* const pNullOperand) override;

    // [P][O]
    public: virtual void Process(Operand* const pOperand) override;
    public: virtual void Process(OperandBox* const pOperandBox) override;
    public: virtual void Process(Output* const pOutput) override;

    // [P][P]
    public: virtual void Process(PhiOperandBox* const pOperandBox) override;
    public: virtual void Process(PointerType* const pType) override;

    // [P][S]
    public: virtual void Process(SwitchOperandBox* const pOperandBox) override;

    // [P][T]
    public: virtual void Process(TypeParam* const pTypeParam) override;

    // [P][T]
    public: virtual void Process(TypeVar* const pTypeVar) override;
    public: virtual void Process(UnresolvedType* const pUnresolvedType) override;

    // [P][V]
    public: virtual void Process(ValuesType* const pValuesType) override;
    public: virtual void Process(Variable* const pVariable) override;
    public: virtual void Process(VoidOutput* const pVoidOutput) override;

    // [W]
    protected: void Write(const Name&);
    protected: void Write(Type* const pType);
    private: void WriteClassName(Class* const pClass);
}; // HtmlFormatter

} // Io
} // Il

#endif // !defined(INCLUDE_Il_Io_HtmlFormatter_h)
