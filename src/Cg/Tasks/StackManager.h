// @(#)$Id$
//
// Evita Il - Cg - StackLayouttask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Il_Cg_StackManager_h)
#define INCLUDE_Evita_Il_Cg_StackManager_h

namespace Il {
namespace Cg {

class StackManager {
  private: class AllocationMap {
    private: static int const kAllocUnit = sizeof(void*) * 16;
    private: int capacity_;
    private: int* elements_;
    private: int length_;
    public: AllocationMap();
    public: ~AllocationMap();
    public: int Count() const { return length_; }
    public: void ExtendTo(int);
    public: void Fill(int, int, int);
    public: int Get(int) const;
    public: void Reset();
    public: void Set(int, int);
  };

  private: AllocationMap map_;
  private: CgSession& session_;

  public: StackManager(CgSession&);
  public: ~StackManager();
  public: int Allocate(int, int);
  public: bool Assign(int, int);
  public: void Release(int);
  public: void Reset();

  DISALLOW_COPY_AND_ASSIGN(StackManager);
};

} // Cg
} // Il

#endif // !defined(INCLUDE_Evita_Il_Cg_StackManager_h)
