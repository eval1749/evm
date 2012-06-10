// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_IrDefs_h)
#define INCLUDE_Il_Ir_IrDefs_h

#include "../Common/Allocable.h"
#include "../Common/Collections.h"

namespace Il {
namespace Cg {
  class Physical;
  class StackSlot;
  class CgTarget;
} // Cg
} // Il

namespace Il {
namespace Ir {

using Il::Cg::Physical;
using Il::Cg::StackSlot;
using Il::Cg::CgTarget;

class ErrorInfo;
class SourceInfo;

class IrObject;

class BBlock;
class CfgEdge;
class ClassOrNamespace;
class CgEdge;
class Functor;

enum__(Op, int);

class Instruction;
    class ArithmeticInstruction;
    class CastInstruction;
    class GeneralInstruction;
    class LastInstruction;
    class OpenInstruction;
    class OpenExitPointInstruction;
    class RelationalInstruction;
    class TerminateInstruction;

class Module;
class Name;
class Namespace;
class MethodGroup;

class Operand;
    class Field;
    class Function;
        class ConstructedFunction;
        class GenericFunction;
    class Immediate;
        class Label;
        class Literal;
        //class NullOperand;
        //class FunctionRef;
        //class MethodRef;
    class Method;
        class ConstructedMethod;
        class GenericMethod;
    class NameRef;
    class Output;
        class SsaOutput;
            class BoolOutput;
            class FrameReg;
            class PseudoOutput;
            class Register;
                class Float;
            class Values;
        class TrueOperand;
        class UnreachableOutput;
        class VoidOutput;
    class Property;
    class Type;
        class ArrayType;
        class BoolType;
        class Class;
            class ConstructedClass;
            class GenericClass;
        class EnumType;
        class FunctionType;
            class ConstructedFunctionType;
            class GenericFunctionType;
        class Intreface;
        class PointerType;
        class PrimitiveType;
        class Struct;
        class TypeParam;
        class TypeVar;
        class UnresolvedType;
        class ValuesType;

    class Variable;

class OperandBox;
    class FunctionOperandBox;
    class PhiOperandBox;
    class SwitchOperandBox;

// TypeArgs class represents type arguments used for
//  o Constructed class
//  o Constructed method
class TypeArgs;

// Helper class for building ValuesType.
class ValuesTypeBuilder;

enum RegClass {
  RegClass_Fpr,
  RegClass_Gpr,
  RegClass_Frame,
};

template<class TBase>
class Extendable_ {
  protected: Extendable_() {}

  public: template<class T> T* Extend() { 
    return static_cast<T*>(static_cast<TBase*>(this)); 
  }

  public: template<class T> T* Extend() const {
    TBase* pThis = const_cast<TBase*>(static_cast<const TBase*>(this));
    return static_cast<T*>(pThis);
  }
  
  DISALLOW_COPY_AND_ASSIGN(Extendable_);
};

class LocalObject : public Common::Allocable {};

class WithIndex {
    private: int m_i;

    // ctor
    public: WithIndex() : m_i(0) {}

    // [G]
    public: int GetIndex() const { return m_i; }

    // [S]
    public: int SetIndex(int n) { return m_i = n; }
}; // WithIndex

class WithWorkArea {
  private: int intval_;
  private: void* ptrval_;

  // ctor
  public: WithWorkArea() : intval_(0), ptrval_(0) {}

  // [G]
  public: int GetFlag() const { return intval_; }

  public: template<typename T> T* GetWork() const {
    return reinterpret_cast<T*>(ptrval_);
  }

  // [S]
  public: int   SetFlag(int i)    { return intval_ = i; }
  public: void* SetWork(void* pv) { return ptrval_ = pv; }
}; // WithWorkArea

class None {};
class Backward {};
class Forward {};
class Layout {};
class Preorder {};
class Postorder {};
class Scc {};

#define class_Enum_(mp_Type, mp_unit, mp_List) \
  public: class Enum ## mp_unit : public mp_List::Enum { \
    private: mp_List* prepare(const mp_Type& cr) { \
      auto& r = const_cast<mp_Type&>(cr); \
      return r.PrepareTraversal(static_cast<mp_List*>(&r)); \
    } \
    public: Enum ## mp_unit(const mp_Type& r) : mp_List::Enum(prepare(r)) {} \
    public: Enum ## mp_unit(const mp_Type* p) : mp_List::Enum(prepare(*p)) {} \
  }; \
  public: class Enum ## mp_unit ## Reverse : public mp_List::EnumReverse { \
    private: mp_List* prepare(const mp_Type& cr) { \
      auto& r = const_cast<mp_Type&>(cr); \
      return r.PrepareTraversal(static_cast<mp_List*>(&r)); \
    } \
    public: Enum ## mp_unit ## Reverse(const mp_Type& r) : \
        mp_List::EnumReverse(prepare(r)) {} \
    public: Enum ## mp_unit ## Reverse(const mp_Type* p) : \
        mp_List::EnumReverse(prepare(*p)) {} \
  }; // class_Enum_

#define DEFINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    class mp_name ## I;

#include "./Ir/Instructions/Instructions.inc"

extern BoolOutput* False;
extern BoolOutput* True;
extern VoidOutput* Useless;
extern VoidOutput* Void;

#define DEFSYMBOL(name) extern const Name* Q_ ## name;
#include "./Symbols.inc"

extern const Name* QD_cctor;      // for class constructor
extern const Name* QD_ctor;       // for instance constructor
extern const Name* QD_value;      // for enum base type.

extern Type* Ty_Bool;
extern const Type* Ty_VoidPtr;

#define DEFTYPE(mp_mc, mp_mod, mp_ns, mp_name, ...) \
    extern const Name* Q_ ## mp_name; \
    extern mp_mc* Ty_ ## mp_name;

#include "./Types.inc"

inline int Ceiling(int const x, int const n) {
  return (x + n - 1) / n;
}

inline uint RotateLeft(uint const x, uint const n) {
  uint const nHigh = x >> (sizeof(uint) * 4 - n);
  uint y = x << n;
  y |= nHigh;
  return y;
} // RotateLeft

inline int RoundUp(int const x, int const n) {
  return Ceiling(x, n) * n;
}

} // Ir
} // Il

#include "./Ir/Modifier.h"
#include "./SourceInfo.h"

#endif // !defined(INCLUDE_Il_Ir_IrDefs_h)
