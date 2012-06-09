// @(#)$Id$
//
// Evita Il - IR - Data Flow Analysis
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_DataFlowAnalysis_h)
#define INCLUDE_Il_Ir_DataFlowAnalysis_h

#include "../Ir/BBlock.h"
#include "./BitVec.h"

namespace Il {
namespace Ir {

typedef DoubleLinkedList_<Register> RegList;
typedef WorkList_<Register> RegWorkList;

class BitVec;

class DataFlowData : public LocalObject {
  public: BitVec in_vec_;
  public: BitVec kill_vec_;
  public: BitVec out_vec_;

  public: DataFlowData() {}

  DISALLOW_COPY_AND_ASSIGN(DataFlowData);
}; // DataFlowData

class DataFlowBB : public BBlock {
  // [C]
  public: void ClearIn(int k)   const { GetIn().SetZero(k); }
  public: void ClearKill(int k) const { GetKill().SetZero(k); }
  public: void ClearOut(int k)  const { GetOut().SetZero(k); }

  // [G]
  public: BitVec& GetIn()   const {
    return const_cast<BitVec&>(m_pDfData->in_vec_);
  }

  public: BitVec& GetKill() const {
    return const_cast<BitVec&>(m_pDfData->kill_vec_);
  }

  public: BitVec& GetOut() const {
    return const_cast<BitVec&>(m_pDfData->out_vec_);
  }

  // [I]
  public: void InitDataFlow(uint const);
  public: bool IsIn(int k)   const { return GetIn().IsOne(k); }
  public: bool IsKill(int k) const { return GetKill().IsOne(k); }
  public: bool IsOut(int k)  const { return GetOut().IsOne(k); }

  // [S]
  public: void SetIn(int k) { GetIn().SetOne(k); }
  public: void SetKill(int k) { GetKill().SetOne(k); }
  public: void SetOut(int k) { GetOut().SetOne(k); }
  DISALLOW_COPY_AND_ASSIGN(DataFlowBB);
}; // DataFlowBB

class Dfa {
  public: static void SolveBackward(const Function&);
  DISALLOW_COPY_AND_ASSIGN(Dfa);
}; // Dfa

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_DataFlowAnalysis_h)
