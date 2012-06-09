#include "precomp.h"
// @(#)$Id$
//
// Evita IL - DominanceTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractIlTest.h"

#include "Il/Analysis/DominanceAnalyzer.h"

namespace IlTest {

using namespace Il::Tasks;

class DominanceTest : public AbstractIlTest {
  protected: static bool EqualBBlockSets(
      const BBlockSet& set1,
      const BBlockSet& set2) {
    if (set1.Count() != set2.Count()) {
      return false;
    }

    foreach (BBlockSet::Enum, bblocks, set1) {
      if (!set2.Contains(*bblocks)) {
        return false;
      }
    }

    return true;
  }

  protected: BBlockSet& ListBBlock() {
    return *new(module().zone()) BBlockSet();
  }

  protected: BBlockSet& ListBBlock(BBlock& b1) {
    BBlockSet& set = *new(module().zone()) BBlockSet();
    set.Add(&b1);
    return set;
  }

  protected: BBlockSet& ListBBlock(BBlock& b1, BBlock& b2) {
    BBlockSet& set = *new(module().zone()) BBlockSet();
    set.Add(&b1);
    set.Add(&b2);
    return set;
  }

  protected: BBlockSet& ListChildren(const BBlock& bb) {
    BBlockSet& set = *new(module().zone()) BBlockSet();
    foreach (BBlock::EnumChild, children, &bb) {
      set.Add(children.Get());
    }
    return set;
  }

  protected: BBlockSet& ListFrontiers(const BBlock& bb) {
    BBlockSet& set = *new(module().zone()) BBlockSet();
    foreach (BBlock::EnumFrontier, frontiers, &bb) {
      set.Add(frontiers.Get());
    }
    return set;
  }

  protected: BBlockSet& ListPostChildren(const BBlock& bb) {
    BBlockSet& set = *new(module().zone()) BBlockSet();
    foreach (BBlock::EnumPostChild, children, &bb) {
      set.Add(children.Get());
    }
    return set;
  }
};

//
//      B0---------+    B0 -> B1, B5
//      |          |
//      B1<------+ |    B1 -> B2, B4
//      |        | |
//   +->B2-->B6  | |    B2 -> B3, B6
//   |  |    |   | |
//   +--B3<--+   | |    B3 -> B4, B2
//      |        | |
//      B4<------+ |    B4 -> B1, B5
//      |          |    B6 -> B3
//      B5<--------+
TEST_F(DominanceTest, Basic) {
  auto& fun = NewFunction("Foo");

  fun.SetUp();
  fun.GetEntryBB()->SetName(-1);
  fun.GetStartBB()->SetName(-2);
  fun.GetExitBB()->SetName(-2);

  auto& exit_bb = *fun.GetExitBB();
  auto& entry_bb = *fun.GetEntryBB();

  auto& zone = module().zone();
  auto& bb0 = *fun.GetStartBB();
  auto& bb1 = NewBBlock(1);
  auto& bb2 = NewBBlock(2);
  auto& bb3 = NewBBlock(3);
  auto& bb4 = NewBBlock(4);
  auto& bb5 = NewBBlock(5);
  auto& bb6 = NewBBlock(6);
  fun.InsertBefore(bb1, exit_bb);
  fun.InsertBefore(bb2, exit_bb);
  fun.InsertBefore(bb3, exit_bb);
  fun.InsertBefore(bb4, exit_bb);
  fun.InsertBefore(bb5, exit_bb);
  fun.InsertBefore(bb6, exit_bb);

  bb0.AppendI(*new(zone) BranchI(module().NewBoolOutput(), bb1, bb5));
  bb1.AppendI(*new(zone) BranchI(module().NewBoolOutput(), bb2, bb4));
  bb2.AppendI(*new(zone) BranchI(module().NewBoolOutput(), bb3, bb6));
  bb3.AppendI(*new(zone) BranchI(module().NewBoolOutput(), bb2, bb4));
  bb4.AppendI(*new(zone) BranchI(module().NewBoolOutput(), bb1, bb5));
  bb5.AppendI(*new(zone) RetI(*Void));
  bb6.AppendI(*new(zone) JumpI(bb3));

  {
    DominanceAnalyzer analyzer(session());
    ASSERT_TRUE(analyzer.ComputeDominance(fun));
  }

  // Dominance Tree
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb1, bb5), ListChildren(bb0));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb2, bb4), ListChildren(bb1));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb3, bb6), ListChildren(bb2));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(), ListChildren(bb3));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(), ListChildren(bb4));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(), ListChildren(bb5));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(), ListChildren(bb6));
  EXPECT_EQ(&entry_bb, bb0.GetImmeidateDominator());
  EXPECT_EQ(&bb0, bb1.GetImmeidateDominator());
  EXPECT_EQ(&bb1, bb2.GetImmeidateDominator());
  EXPECT_EQ(&bb2, bb3.GetImmeidateDominator());
  EXPECT_EQ(&bb1, bb4.GetImmeidateDominator());
  EXPECT_EQ(&bb0, bb5.GetImmeidateDominator());
  EXPECT_EQ(&bb2, bb6.GetImmeidateDominator());

  // Dominance Frontiers
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(exit_bb), ListFrontiers(bb0));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb1, bb5), ListFrontiers(bb1));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb2, bb4), ListFrontiers(bb2));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb2, bb4), ListFrontiers(bb3));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb1, bb5), ListFrontiers(bb4));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(exit_bb), ListFrontiers(bb5));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb3), ListFrontiers(bb6));

  {
    DominanceAnalyzer analyzer(session());
    ASSERT_TRUE(analyzer.ComputePostDominance(fun));
  }

  EXPECT_PRED2(EqualBBlockSets, ListBBlock(), ListPostChildren(bb0));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(), ListPostChildren(bb1));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(), ListPostChildren(bb2));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb2, bb6), ListPostChildren(bb3));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb1, bb3), ListPostChildren(bb4));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(bb0, bb4), ListPostChildren(bb5));
  EXPECT_PRED2(EqualBBlockSets, ListBBlock(), ListPostChildren(bb6));
  EXPECT_EQ(&bb5, bb0.GetPostImmeidateDominator());
  EXPECT_EQ(&bb4, bb1.GetPostImmeidateDominator());
  EXPECT_EQ(&bb3, bb2.GetPostImmeidateDominator());
  EXPECT_EQ(&bb4, bb3.GetPostImmeidateDominator());
  EXPECT_EQ(&bb5, bb4.GetPostImmeidateDominator());
  EXPECT_EQ(&exit_bb, bb5.GetPostImmeidateDominator());
  EXPECT_EQ(&bb3, bb6.GetPostImmeidateDominator());
}

}  // IlTest
