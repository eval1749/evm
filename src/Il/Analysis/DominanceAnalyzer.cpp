#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Data Flow Analysis
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./DominanceAnalyzer.h"

#include "../../Common/Collections.h"
#include "./DataFlowAnalysis.h"
#include "../Ir/BBlock.h"
#include "../Ir/CfgEdge.h"
#include "../Ir/Operands/Function.h"
#include "../Tasks/FunctionEditor.h"

namespace Il {
namespace Tasks {

using namespace Il::Ir;
using Il::Io::LogWriter;
using Il::Io::XhtmlWriter;

// DomTreeBuilder class
template<class DomCFG, class DomBBlock, class DomInfo>
class DomTreeBuilder_ {

  private: class DfsTreeBuilder {
    private: int dfs_name_;
    private: DomBBlock* dfs_prev_;

    private: DfsTreeBuilder() : dfs_name_(0), dfs_prev_(nullptr) {}

    public: static void Build(DomBBlock* entry) {
      DfsTreeBuilder builder;
      builder.Process(entry);
    }

    private: void Process(DomBBlock* curr) {
      curr->SetDfsName(1);
      for (const auto& edge: curr->GetOutEdges()) {
        auto const succ = DomBBlock::GetEdgeTo(edge);
        if (!succ->GetDfsName())
          Process(succ);
      }

      dfs_name_ += 1;
      curr->SetDfsName(dfs_name_);
      curr->SetDfsNext(dfs_prev_);
      dfs_prev_ = curr;
      //pBB->WriteLog("visit %p dfs=%d", pBB, dfs_name_);
    }
  };

  private: DomCFG& cfg_;
  private: DomBBlock* const entry_;

  public: DomTreeBuilder_(DomCFG& cfg) 
      : cfg_(cfg), entry_(cfg.GetEntry()) {}

  public: void Build() {
    Init();
    DfsTreeBuilder::Build(entry_);
    RemoveUnrechable();
    ComputeParent();
    entry_->SetDomParent(nullptr);
    ComputeChildren();
    ComputeFrontiers();
    Uninit();
  }

  private: void AddFrontier(DomBBlock* node, DomBBlock* frontier) {
    if (node->GetDomFrontiers().Contains(frontier))
      return;
    node->GetDomFrontiers().Add(frontier);
  }

  //  Make children list in reverse postorder.
  private: void ComputeChildren() {
    for (auto bblock = entry_->GetDfsNext(); bblock;
         bblock = bblock->GetDfsNext()) {
      auto const parent = bblock->GetDomParent();
      parent->GetDomChildren().Append(bblock->GetDomInfo());
    }
  }

  //  Loop over bblock which has more than one predecessors.
  private: void ComputeFrontiers() {
    for (auto& bblock: cfg_.function().bblocks()) {
      auto const node = bblock.Extend<DomBBlock>();
      if (!HasMoreThanOnePred(node))
        continue;

      for (const auto& edge: node->GetInEdges()) {
        auto const idom = node->GetDomParent();
        for (auto runner = DomBBlock::GetEdgeFrom(edge);
             runner != idom;
             runner = runner->GetDomParent())
          AddFrontier(runner, node);
      }
    }
  }

  //  Computes parent (immediate dominator) for each bblock.
  private: void ComputeParent() {
    entry_->SetDomParent(entry_);
    entry_->SetDepth(1);
    auto changed = true;
    while (changed) {
      changed = false;
      for (auto pBB = entry_->GetDfsNext(); pBB; pBB = pBB->GetDfsNext()) {
        if (ComputeParentAux(pBB)) {
          changed = true;
        }
      }
    }
  }

  //  Computes new parent by processed predecessor.
  private: bool ComputeParentAux(DomBBlock* curr) {
    ASSERT(!!curr);
    for (const auto& edge: curr->GetInEdges()) {
      auto parent = DomBBlock::GetEdgeFrom(edge);
      if (!parent->GetDomParent())
        continue;

      for (const auto& edge: curr->GetInEdges()) {
        auto const pred = DomBBlock::GetEdgeFrom(edge);
        if (parent != pred && pred->GetDomParent())
          parent = Intersect(pred, parent);
      }

      if (curr->GetDomParent() != parent) {
        curr->SetDomParent(parent);
        curr->SetDepth(parent->GetDepth() + 1);
        return true;
      }
    }
    return false;
  }

  private: static bool HasMoreThanOnePred(DomBBlock* node) {
    const auto end = node->GetInEdges().end();
    auto it = node->GetInEdges().begin();
    if (it == end)
      return false;
    ++it;
    ASSERT(it != end || node->GetInEdges().GetFirst() == node->GetInEdges().GetLast());
    return it != end;
  }

  //  Allocates dominfo for all bblocks
  private: void Init() {
    for (auto& bblock: cfg_.function().bblocks()) {
      auto& dom_bblock = *bblock.Extend<DomBBlock>();
      dom_bblock.SetDfsName(0);
      dom_bblock.SetDfsNext(nullptr);

      if (auto const pDomInfo = dom_bblock.GetDomInfo()) {
        pDomInfo->Reset();
      } else {
        dom_bblock.SetDomInfo(new DomInfo(dom_bblock));
      }
    }
  }

  private: DomBBlock* Intersect(DomBBlock* finger1, DomBBlock* finger2) {
    while (finger1 != finger2) {
      while (finger1->GetDfsName() < finger2->GetDfsName())
        finger1 = finger1->GetDomParent();

      while (finger2->GetDfsName() < finger1->GetDfsName())
        finger2 = finger2->GetDomParent();
   }
    return finger1;
  }

  private: void RemoveUnrechable() {
    Common::Collections::Stack_<BBlock*> unreachables;
    for (auto& bblock: cfg_.function().bblocks()) {
      if (!bblock.Extend<DomBBlock>()->GetDfsName())
        unreachables.Push(&bblock);
    }

    while (!unreachables.IsEmpty()) {
      auto& bblock = *unreachables.Pop();
      bblock.GetFunction()->RemoveBBlock(bblock);
    }
  }

  private: void Uninit() {
    for (auto& bblock: cfg_.function().bblocks())
      bblock.Reset();
  }

  DISALLOW_COPY_AND_ASSIGN(DomTreeBuilder_);
};

#if 0
template<class DomCFG, class DomBBlock, class DomInfo = BBlock::DomInfo>
class DomTreeDumper {
  public: static void Run(const DomCFG& fun, const char* psz) {
    auto const pLogWriter = fun.GetLogWriter();

    if (pLogWriter == nullptr) {
      return;
    }

    Function::LogSection oSection(
        &fun.function(),
        "%sDominance Tree of %p",
        psz,
        &fun.function());

    pLogWriter->StartElement("table", "border", "1");

    pLogWriter->StartElement("thead");
        pLogWriter->StartElement("tr");
            pLogWriter->WriteElement("th", "BBlock");
            pLogWriter->WriteElement("th", "Depth");
            pLogWriter->WriteElement("th", "Parent");
            pLogWriter->WriteElement("th", "Children");
            pLogWriter->WriteElement("th", "Frontiers");
        pLogWriter->EndElement("tr");
    pLogWriter->EndElement("thead");

    for (auto& bblock: fun.function().bblocks()) {
        auto const pBBlock = bblock.Extend<DomBBlock>();

        pLogWriter->StartElement("tr");

        pLogWriter->StartElement("td");
        pLogWriter->Write(pBBlock);
        pLogWriter->EndElement("td");

        auto const pDomInfo = pBBlock->GetDomInfo();

        pLogWriter->WriteElement("td", "%d", pDomInfo->GetDepth());

        if (!pDomInfo->GetDomParent()) {
            pLogWriter->StartElement("td");
            pLogWriter->Write("-");
            pLogWriter->EndElement("td");

        } else {
            pLogWriter->StartElement("td");
            pLogWriter->Write(pDomInfo->GetDomParent());
            pLogWriter->EndElement("td");
        }

        // Children
        {
            pLogWriter->StartElement("td");
            pLogWriter->WriteString("{");

            const char* psz = "";
            foreach (BBlock::DomInfo::EnumChild, oEnum, *pDomInfo) {
                auto const pChild = oEnum.Get();
                pLogWriter->Write("%s%p", psz, pChild);
                psz = " ";
            }

            pLogWriter->WriteString("}");
            pLogWriter->EndElement("td");
        }

        // Frontiers
        {
            pLogWriter->StartElement("td");
            pLogWriter->WriteString("{");

            const char* psz = "";
            foreach (BBlock::DomInfo::EnumFrontier, oEnum, *pDomInfo) {
                auto const pFrontier = oEnum.Get();
                pLogWriter->Write("%s%p", psz, pFrontier);
                psz = " ";
            }

            pLogWriter->WriteString("}");
            pLogWriter->EndElement("td");
        }

        pLogWriter->EndElement("tr");
    }

    pLogWriter->EndElement("table");
}
};
#endif

template<class Base_>
class DomBBlock_ : public Base_ {
  private: typedef BBlock::DomInfo DomInfo;
  private: typedef DomBBlock_<Base_> BBlock_;

  public: uint GetDfsName() const { return static_cast<uint>(GetIndex()); }
  public: BBlock_* GetDfsNext() const { return GetWork<BBlock_>(); }
  public: void SetDfsName(uint nName) { SetIndex(nName); }
  public: void SetDfsNext(BBlock_* pNext) { SetWork(pNext); }

  public: uint GetDepth() const { return GetDomInfo()->GetDepth(); }
  public: uint SetDepth(uint n) { return GetDomInfo()->SetDepth(n); }

  public: DomInfo::ChildList& GetDomChildren() const {
    return GetDomInfo()->GetChildren();
  }

  public: DomInfo::FrontierList& GetDomFrontiers() const {
    return GetDomInfo()->GetFrontiers();
  }

  public: BBlock_* GetDomParent() const {
    return GetDomInfo()->GetParent()->Extend<BBlock_>();
  }

  public: void SetDomParent(BBlock_* pBB) { GetDomInfo()->SetParent(pBB); }

  public: static BBlock_* GetEdgeFrom(const CfgEdge& edge) {
    return GetEdgeFromInternal(edge)->Extend<BBlock_>();
  }

  public: static BBlock_* GetEdgeTo(const CfgEdge& edge) {
    return GetEdgeToInternal(edge)->Extend<BBlock_>();
  }

  DISALLOW_COPY_AND_ASSIGN(DomBBlock_);
};

class DomBBlockBase : public BBlock {
  public: typedef InEdges InEdges;
  public: typedef OutEdges OutEdges;
  //private: DomBBlockBase() {}
  public: DomInfo* GetDomInfo() const { return m_pDomInfo; }
  public: const InEdges& GetInEdges() const { return in_edges(); }
  public: const OutEdges& GetOutEdges() const { return out_edges(); }
  public: void SetDomInfo(DomInfo* pDomInfo) { m_pDomInfo = pDomInfo; }

  protected: static BBlock* GetEdgeFromInternal(const CfgEdge& edge) {
    return edge.GetFrom();
  }
  protected: static BBlock* GetEdgeToInternal(const CfgEdge& edge) {
    return edge.GetTo();
  }
  DISALLOW_COPY_AND_ASSIGN(DomBBlockBase);
};

class PostDomBBlockBase : public BBlock {
  public: typedef BBlock::InEdges OutEdges;
  public: typedef BBlock::OutEdges InEdges;
  //private: PostDomBBlockBase() {}
  public: DomInfo* GetDomInfo() const { return m_pPostDomInfo; }
  public: const InEdges& GetInEdges() const { return out_edges(); }
  public: const OutEdges& GetOutEdges() const { return in_edges(); }
  public: void SetDomInfo(DomInfo* pDomInfo) { m_pPostDomInfo = pDomInfo; }
  protected: static BBlock* GetEdgeFromInternal(const CfgEdge& edge) {
    return edge.GetTo();
  }
  protected: static BBlock* GetEdgeToInternal(const CfgEdge& edge) {
    return edge.GetFrom();
  }
  DISALLOW_COPY_AND_ASSIGN(PostDomBBlockBase);
};

typedef DomBBlock_<DomBBlockBase> DomBBlock;
typedef DomBBlock_<PostDomBBlockBase> PostDomBBlock;

class DomCFG {
  private: const Function& function_;

  public: DomCFG(const Function& function) : function_(function) {}
  public: const Function& function() const { return function_; }

  public: DomBBlock* GetEntry() const {
    return function_.GetEntryBB()->Extend<DomBBlock>();
  }

#if 0
  public: LogWriter* GetLogWriter() const {
    return function_.GetSession()->GetLogWriter();
  }
#endif

  DISALLOW_COPY_AND_ASSIGN(DomCFG);
};

class PostDomCFG : public DomCFG {
  public: PostDomCFG(const Function& function) : DomCFG(function) {}

  public: PostDomBBlock* GetEntry() const {
    return function().GetExitBB()->Extend<PostDomBBlock>();
  }

  DISALLOW_COPY_AND_ASSIGN(PostDomCFG);
};

DominanceAnalyzer::DominanceAnalyzer(Session& session)
    : FunctionTasklet("DominanceAnalyzer", session) {}

/// <summary>
///   Computes dominance.
/// </summary>
/// <returns>
///   Returns false if dominance tree has been computed.
/// </returns>
bool DominanceAnalyzer::ComputeDominance(const Function& fun) {
  DEBUG_FORMAT("%s", fun);

  if (fun.Has(Function::Cache_Dom)) {
    DEBUG_FORMAT("Use cached dominance: %s", fun);
    return false;
  }

  DomCFG cfg(fun);
  DomTreeBuilder_<DomCFG, DomBBlock, BBlock::DomInfo> builder(cfg);
  builder.Build();
  fun.MarkCache(Function::Cache_Dom);

  #if 0
    DomTreeDumper<DomCFG, DomBBlock, BBlock::DomInfo>
        ::Run(cfg, "");
  #endif

  return true;
}

/// <summary>
///   Computes post-dominance.
/// </summary>
/// <returns>
///   Returns false if dominance tree has been computed.
/// </returns>
bool DominanceAnalyzer::ComputePostDominance(const Function& fun) {
  DEBUG_FORMAT("%s", fun);

  if (fun.Has(Function::Cache_PostDom)) {
    DEBUG_FORMAT("Use cached dominance: %s", fun);
    return false;
  }

  {
    FunctionEditor editor(session());
    editor.EliminateInfiniteLoop(const_cast<Function&>(fun));
  }

  PostDomCFG cfg(fun);
  DomTreeBuilder_<PostDomCFG, PostDomBBlock, BBlock::DomInfo> builder(cfg);
  builder.Build();
  fun.MarkCache(Function::Cache_PostDom);

  #if 0
    DomTreeDumper<PostDomCFG, PostDomBBlock, BBlock::DomInfo>
        ::Run(cfg, "Post-");

  #endif
  return true;
}

}  // Tasks
}  // Il
