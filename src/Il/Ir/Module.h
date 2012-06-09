// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Module_h)
#define INCLUDE_Il_Ir_Module_h

#include "../Functor.h"
#include "./Graph.h"
#include "./IrObject.h"
#include "./Operands/Function.h"
#include "./Operands/Literal.h"

namespace Il {
namespace Ir {

/// <summary>
///   Compilation module contains functions. This is a unit of compilation.
/// </summary>
class Module
    : public Graph_<Function, Module>,
      public IrObject_<Module> {
  CASTABLE_CLASS(Module)

  private: typedef HashSet_<HashKey_<const Literal>> LiteralSet;

  private: mutable LiteralSet literal_set_;
  private: mutable int num_bblocks_;
  private: mutable int num_functions_;
  private: mutable int num_outputs_;
  private: mutable LocalMemoryZone zone_;

  // ctor
  public: Module();
  public: virtual ~Module();

  // properties
  public: MemoryZone& zone() const { return zone_; }

  // [C]
  public: Module& Clone(const TypeArgs&) const;
  public: Module& Clone() const;

  // [E]
  class_Enum_(Module, Function,          LayoutList);
  class_Enum_(Module, FunctionPreorder,  PostorderList)
  class_Enum_(Module, FunctionPostorder, PostorderList)

  // [N]
  public: BBlock& NewBBlock() const;
  public: const BoolOutput& NewBoolOutput() const;
  public: Function& NewFunction(Function*, Function::Flavor, const Name&);
  public: const Literal& NewLiteral(bool) const;
  public: const Literal& NewLiteral(const Type&, int64) const;
  public: const Output& NewOutput(const Type&) const;
  public: const Register& NewRegister() const;
  public: const Values& NewValues() const;

  // [R]
  public: void RemoveFunction(Function&);

  DISALLOW_COPY_AND_ASSIGN(Module);
};


#define COMPILER_INTERNAL_ERROR() \
  DEBUG_PRINTF("Compiler internal error\n");

#define C_INTERNAL_ERROR(msg) \
  DEBUG_PRINTF("Compiler internal error: %s\n", msg);

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Module_h)
