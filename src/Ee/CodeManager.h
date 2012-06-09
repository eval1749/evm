// @(#)$Id$
//
// Evita Ee - CodeManager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Ee_CodeManager_h)
#define INCLUDE_Ee_CodeManager_h

#include "./CodeDesc.h"
#include "../Common/Lockable.h"
#include "../Common/Collections.h"

namespace Ee {

using namespace Common::Collections;
using Il::Ir::Function;

class CodeManager {
  private: class CodeHeap {
    private: class Block {
      friend class CodeHeap;
      private: static int const kAllocUnit = 16;
      private: int32 curr_;
      private: int32 end_;
      private: Block* next_;
      private: Block* prev_;
      public: void* operator new(size_t, void* p) { return p; }
      public: Block(Block*, size_t);
      public: void* Alloc(size_t);
    };

    private: Block* head_;
    private: Block* tail_;
  
    public: CodeHeap();
    public: ~CodeHeap();
    public: void* Alloc(size_t);
  };

  private: class CodeDescBox {
    private: CodeDesc* code_desc_;
    public: CodeDescBox(CodeDesc* p = nullptr) : code_desc_(p) {}

    public: CodeDescBox(const CodeDesc& code_desc)
        : code_desc_(&const_cast<CodeDesc&>(code_desc)) {}

    // operators
    public: bool operator ==(const Address&) const;
    public: bool operator ==(const CodeDescBox&) const;
    public: bool operator <(const Address&) const;
    public: bool operator <(const CodeDescBox&) const;
    public: operator CodeDesc*() const { return code_desc_; }

    // properties
    private: uintptr_t code_max() const;
    private: uintptr_t code_min() const;
  }; // CodeDescBox

  private: CodeHeap code_heap_;
  private: Common::Lockable code_desc_map_lock_;
  private: BinaryTree_<Address, CodeDescBox> code_desc_map_;

  // ctor
  protected: CodeManager();
  public: ~CodeManager();

  public: CodeDesc* Find(const Address) const;

  // [G]
  public: static CodeManager* Get();

  // [I]
  public: static void Init();

  // [A]
  public: CodeDesc& NewCodeDesc(
      const Function&,
      const Collection_<CodeDesc::Annotation>&,
      uint32);

  DISALLOW_COPY_AND_ASSIGN(CodeManager);
};

} // Ee

#endif // !defined(INCLUDE_Ee_CodeManager_h)
