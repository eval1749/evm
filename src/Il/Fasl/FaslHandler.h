// @(#)$Id$
//
// Evita Il - FastHandler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Fasl_FastHandler_h)
#define INCLUDE_Il_Fasl_FastHandler_h

#include "../Ir.h"

namespace Il {
namespace Fasl {

using namespace Il::Ir;

interface FaslHandler {
    // [E]
    public: virtual void EndInstruction(
        Instruction* const pI) = 0;

    // [H]
    public: virtual BBlock* HandleBBlock(
        int const iName) = 0;

    public: virtual Class* HandleClass(
        ClassOrNamespace* const pOuter,
        int const iModifiers,
        const Name& name,
        Class* const pBaseClass) = 0;

    public: virtual Class* HandleClassRef(
        ClassOrNamespace* const pOuter,
        const Name& name) = 0;

    public: virtual Field* HandleField(
        Class* const pClass,
        int const iModifiers,
        Type* const pType,
        const Name& name) = 0;

    public: virtual Field* HandleFieldRef(
        Class* const pClass,
        const Name& name) = 0;

    public: virtual bool HandleFunctionDef(
        Function* const pFunction,
        int const cBBlocks) = 0;

    public: virtual Function* HandleFunctionRef(
        Operand* const pOuter,
        const Name& name,
        Function::Flavor eFlavor) = 0;

    public: virtual void HandleInterface(
        Class* const pClass,
        Class* const pInterface) = 0;

    public: virtual Label* HandleLabel(
        int const iName) = 0;

    public: virtual Method* HandleMethod(
        MethodGroup* const pMethodGroup,
        int const iModifiers,
        FunctionType* const pFunctionType) = 0;

    public: virtual MethodGroup* HandleMethodGroup(
        Class* const pClass,
        const Name& name) = 0;

    public: virtual bool HandleMethodParam(
        Method* const pMethod,
        int const iIndex,
        const Name& name) = 0;

    public: virtual Method* HandleMethodRef(
        Class* const pClass,
        FunctionType* const pFunctionType,
        const Name& name) = 0;

    public: virtual Namespace* HandleNamespaceRef(
        Namespace* const pOuter,
        const Name& name) = 0;

    public: virtual bool HandleOperand(
        Instruction* const pInstruction,
        int const iIndex,
        Operand* const pOperand) = 0;

    public: virtual Output* HandleOutput(
        int const iName) = 0;

    public: virtual bool HandleOutput(
        Instruction* const pI,
        Type* const pType) = 0;

    public: virtual Variable* HandleVariable(
        const Name& name) = 0;

    // [S]
    public: virtual Instruction* StartInstruction(
        Op const eOp) = 0;
}; // FaslHandler

} // Fasl
} // Il

#endif // !defined(INCLUDE_Il_Fasl_FastHandler_h)
