// @(#)$Id$
//
// Evita Il - IR - Function
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_IrIrObject_h)
#define INCLUDE_Il_IrIrObject_h

#include "../BBlock.h"
#include "./FrameReg.h"
#include "./Operand.h"
#include "./Variable.h"
#include "./VoidOutput.h"

#include "../CgEdge.h"
#include "../Graph.h"
#include "../GraphNode.h"

#include "../Instructions/Instruction.h"

namespace Ee {
class CodeDesc;
} // Ee

namespace Il {
namespace Ir {

class LoopNode;

/// <summary>
///     Internal representation of funciton object.
/// </summary>
class Function :
    public Operand_<Function>,
    public Extendable_<Function>,
    public Graph_<BBlock, Function>,
    public GraphNode_<Module, Function, CgEdge>,
    public WithIndex,
    public WithWorkArea,
    public WorkListItem_<Function> {

  CASTABLE_CLASS(Function);

  // For Module::NewFunction
  friend class Module;

  // For retrieving owner of variable. See Variable::Realize
  friend class Variable;

  class_Enum_(Function, BBlock,          LayoutList);
  class_Enum_(Function, BBlockPreorder,  PreorderList)
  class_Enum_(Function, BBlockPostorder, PostorderList)

  public: class EnumCall : public Users::Enum {
    public: EnumCall(const Function& r) : Users::Enum(&r.m_oCalls) {}
    public: EnumCall(const Function* p) : Users::Enum(&p->m_oCalls) {}

    public: Instruction* GetI() const {
      ASSERT(!AtEnd()); return Users::Enum::Get()->GetI();
    }
  };

  public: class EnumFrameReg : public FrameReg::List::Enum {
    public: EnumFrameReg(const Function& r)
        : FrameReg::List::Enum(&r.m_oFrameRegs) {}

    public: EnumFrameReg(const Function* p)
        : FrameReg::List::Enum(&p->m_oFrameRegs) {}
  };

  public: class EnumI {
    private: BBlock* bblock_;
    private: Instruction* instruction_;

    public: EnumI(const Function& r)
      : bblock_(r.GetEntryBB()),
        instruction_(r.GetEntryBB()->GetFirstI()) {}

    public: EnumI(const Function* p)
      : bblock_(p->GetEntryBB()),
        instruction_(p->GetEntryBB()->GetFirstI()) {}

    public: bool AtEnd() const { return instruction_ == nullptr; }

    public: Instruction* Get() const {
      ASSERT(!AtEnd());
      return instruction_;
    }

    public: void Next() {
      instruction_ = instruction_->GetNext();

      for (;;) {
        if (instruction_ != nullptr) return;
        bblock_ = bblock_->GetNext();
        if (bblock_ == nullptr) return;
        instruction_ = bblock_->GetFirstI();
      }
    }
  };

  public: class EnumOuterAndSelf {
    private: const Function* m_pRunner;

    public: EnumOuterAndSelf(const Function& r) : m_pRunner(&r) {}
    public: bool AtEnd() const { return m_pRunner == nullptr; }

    public: Function* Get() const {
      ASSERT(!this->AtEnd());
      return const_cast<Function*>(m_pRunner);
    }

    public: void Next() { ASSERT(!this->AtEnd()); m_pRunner++; }
  };

  public: class EnumOutput : protected EnumI {
    public: EnumOutput(const Function& r) : EnumI(r) { NextInternal(); }
    public: EnumOutput(const Function* p) : EnumI(p) { NextInternal(); }

    public: bool AtEnd() const { return EnumI::AtEnd(); }
    public: Output* Get() const { return EnumI::Get()->GetOutput(); }

    public: void Next() {
      ASSERT(!AtEnd());
      EnumI::Next();
      NextInternal();
    }

    private: void NextInternal() {
      while (!EnumI::AtEnd()) {
        if (!EnumI::Get()->GetOutput()->Is<VoidOutput>()) {
          break;
        }
        EnumI::Next();
      }
    }
  };

  public: class EnumReg : protected EnumOutput {
    public: EnumReg(const Function& r) : EnumOutput(r) { NextInternal(); }
    public: EnumReg(const Function* p) : EnumOutput(p) { NextInternal(); }

    public: Register* Get() const {
      ASSERT(!AtEnd());
      return EnumOutput::Get()->StaticCast<Register>();
    }

    public: bool AtEnd() const { return EnumOutput::AtEnd(); }

    public: void Next() {
      ASSERT(!AtEnd());
      EnumOutput::Next();
      NextInternal();
    }

    private: void NextInternal() {
      while (!AtEnd()) {
        if (EnumOutput::Get()->Is<Register>()) break;
          EnumOutput::Next();
      }
    }
  };

  // TODO(yosi) 2012-01-29 Do we still use Flag_DynamicExtent?
  public: enum Flag { 
    Flag_DynamicExtent  = 1 << 0,
  };

  public: enum Flavor {
    Flavor_Anonymous,
    Flavor_Finally,
    Flavor_Named,
    Flavor_Template,
    Flavor_Toplevel,
  };

  // InstructionRange is a wrapper class of OperandBox list range.
  public: class InstructionRange {
    public: class Iterator {
      private: Users::Iterator iterator_;
      public: Iterator(const Users::Iterator& iterator)
          : iterator_(iterator) {}
      public: Instruction& operator*() const {
        return iterator_->instruction();
      }
      public: bool operator==(const Iterator& another) const {
        return iterator_ == another.iterator_;
      }
      public: bool operator!=(const Iterator& another) const {
        return !operator==(another);
      }
      public: Iterator operator++() {
        ++iterator_;
        return* this;
      }
    };
    private: const Users* users_;
    public: InstructionRange(const Users& users) : users_(&users) {}
    public: Iterator begin() const { return Iterator(users_->begin()); }
    public: Iterator end() const { return Iterator(users_->end()); }
  };

  // VariableRange is a wrapper class of OperandBox list range.
  public: class VariableRange {
    public: class Iterator {
      private: Users::Iterator iterator_;
      public: Iterator(const Users::Iterator& iterator)
          : iterator_(iterator) {}
      public: Variable& operator*() const {
        return iterator_->instruction().variable();
      }
      public: bool operator==(const Iterator& another) const {
        return iterator_ == another.iterator_;
      }
      public: bool operator!=(const Iterator& another) const {
        return !operator==(another);
      }
      public: Iterator operator++() {
        ++iterator_;
        return* this;
      }
    };
    private: const Users* users_;
    public: VariableRange(const Users& users) : users_(&users) {}
    public: Iterator begin() const { return Iterator(users_->begin()); }
    public: Iterator end() const { return Iterator(users_->end()); }
  };

  private: Flavor flavor_;
  private: const Name& name_;

  // Asm and Stack passes use m_cbFrame.
  public: uint m_cbFrame;

  // Function type is fixed after object construction. Since compiler
  // creates Function object for storing instructions before type
  // resolution or during parsing source code.
  private: const FunctionType* m_pFunctionType;

  // A method if this function is implementation of method body.
  private: Method* method_;

  private: Function* const m_pOuter;

  private: Users m_oCalls;
  private: FrameReg::List m_oFrameRegs;
  private: Users users_;
  private: Users m_oUpVarDefs;
  private: Users m_oVarDefs;

  private: uint m_rgfFlag;

  // Output from Cg
  private: Ee::CodeDesc* code_desc_;

  // ctor
  private: Function(Function*, Flavor, const Name&);
  public: virtual ~Function() {}

  // properties
  public: const LayoutList& bblocks() const {
    return static_cast<const LayoutList&>(*this);
  }
  public: Ee::CodeDesc* code_desc() const { return code_desc_; }
  public: Module& module() const { return *GetGraphParent(); }
  public: Flavor flavor() const { return flavor_; }

  public: const FunctionType& function_type() const {
    return *m_pFunctionType;
  }

  public: const Name& name() const { return name_; }
  public: void set_code_desc(Ee::CodeDesc&);

  public: const InstructionRange upvardefs() const {
    return InstructionRange(m_oUpVarDefs);
  }

  public: const InstructionRange vardefs() const {
    return InstructionRange(m_oVarDefs);
  }

  public: VariableRange variables() const {
    return VariableRange(m_oVarDefs);
  }

  // [A]
  public: FrameReg* AddFrameReg(FrameReg*);

  // For FastLoader
  public: void AppendBBlock(BBlock* const pBBlock);

  // [C]
  public: bool ComputeLiveness();
  public: LoopNode* ComputeLoop();
  public: bool ComputeVarLiveness();

  // [F]
  public: PseudoOutput* FindUpVar(const Variable* const pVar) const;

  // [G]
  public: BBlock* GetEntryBB() const;
  public: BBlock* GetExitBB()  const;
  public: Flavor  GetFlavor()  const { return flavor_; }
  public: Method* GetMethod() const { return method_; }
  public: Function* GetOuter() const { return m_pOuter; }
  public: PrologueI* GetPrologueI() const;
  public: Function* GetSingleCaller() const;
  public: BBlock* GetStartBB() const;

  // [H]
  public: bool HasCallSite() const { return !m_oCalls.IsEmpty(); }
  public: bool HasNonLocalExitPoint() const;
  public: bool HasUpVar() const;
  public: bool HasUseSite() const;
  public: bool HasVariable() const { return !m_oVarDefs.IsEmpty(); }

  // [I]
  public: void InsertAfter(const BBlock&, const BBlock&);
  public: void InsertBefore(const BBlock&, const BBlock&);
  public: PseudoOutput* InternUpVar(Variable* const);
  public: bool IsClosure() const;

  public: bool IsDynamicExtent() const {
    return (m_rgfFlag & Flag_DynamicExtent) != 0;
  }

  public: bool IsNotClosure() const;

  public: bool IsStackRestify() const;

  // [M]
  public: void MakeVoid();
  public: uint MarkFlag(uint const n) { return m_rgfFlag |= n; }
  public: BBlock* MoveBBlock(BBlock* const, BBlock* const);

  // [R]
  public: virtual void Realize(OperandBox* const) override;
  public: void RemoveBBlock(BBlock&);
  public: void RemoveFrameReg(FrameReg* p) { m_oFrameRegs.Delete(p); }

  // [S]
  public: Flavor SetFlavor(Flavor const e)  { return flavor_ = e; }
  public: void SetFunctionType(const FunctionType&);
  public: void SetMethod(Method&);
  public: void SetOuter(Function* const pOuter);
  public: void SetUp();

  // [U]
  public: uint UnmarkFlag(uint const n) { return m_rgfFlag &= ~n; }
  public: virtual void Unrealize(OperandBox*) override;

  DISALLOW_COPY_AND_ASSIGN(Function);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_IrIrObject_h)
