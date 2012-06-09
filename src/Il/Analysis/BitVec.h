// @(#)$Id$
//
// Evita Il - IR - Data Flow Analysis
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Analysis_BitVec_h)
#define INCLUDE_Il_Analysis_BitVec_h

namespace Il {
namespace Ir {

class BitVec {
  private: enum Constants {
      BitsInWord = sizeof(uint32) * 8,
  }; // Constants

  private: int capacity_;
  private: int count_;
  private: uint32* bits_;

  // ctor
  public: explicit BitVec(int = 0);
  public: ~BitVec();

  // [A]
  public: BitVec& Adjust(int);
  public: BitVec& AndC2(const BitVec&);

  // [C]
  private: template<class Operator> BitVec& Compute(const BitVec& that);
  public: BitVec& Copy(const BitVec&);
  public: int Count() const { return count_; }

  private: int CountWord(int const bits) const {
    ASSERT(bits >= 0);
    return Ceiling(bits, BitsInWord);
  }

  // [E]
  public: bool Equals(const BitVec&) const;

  // [F]
  public: BitVec& FillOne();
  public: BitVec& FillZero();
  public: int FindOne() const;
  public: int FindLastOne() const;

  // [H]
  public: int Hash() const;

  // [I]
  public: BitVec& Ior(const BitVec&);
  public: bool IsOne(int) const;
  public: bool IsZero(int) const;

  // [S]
  public: void SetOne(int);
  public: void SetZero(int);

  DISALLOW_COPY_AND_ASSIGN(BitVec);
}; // BitVec

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Analysis_BitVec_h)
