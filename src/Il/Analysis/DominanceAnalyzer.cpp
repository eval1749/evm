#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Data Flow Analysis
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./DominanceAnalyzer.h"

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

    public: static void Build(DomBBlock& entry) {
      DfsTreeBuilder builder;
      builder.Process(entry);
    }

    private: void Process(DomBBlock& curr) {
      curr.SetDfsName(1);
      foreach (DomBBlock::EnumOutEdge, edges, &curr) {
        auto& succ = *edges.GetNode();
        if (succ.GetDfsName() == 0) {
          Process(succ);
        }
      }

      dfs_name_ += 1;
      curr.SetDfsName(dfs_name_);
      curr.SetDfsNext(dfs_prev_);
      dfs_prev_ = &curr;
      //pBB->WriteLog("visit %p dfs=%d", pBB, dfs_name_);
    }
  };

  private: DomCFG& cfg_;
  private: DomBBlock& entry_;

  public: DomTreeBuilder_(DomCFG& cfg) 
      : cfg_(cfg), entry_(*cfg.GetEntry()) {}

  public: void Build() {
    Init();
    DfsTreeBuilder::Build(entry_);
    RemoveUnrechable();
    ComputeParent();
    entry_.SetParent(nullptr);
    ComputeChildren();
    ComputeFrontiers();
    Uninit();
  }

  private: void AddFrontier(DomBBlock& node, DomBBlock& frontier) {
    if (!node.GetFrontiers().Contains(&frontier)) {
      node.GetFrontiers().Add(&frontier);
    }
  }

  //  Make children list in reverse postorder.
  private: void ComputeChildren() {
    for (
        auto pBB = entry_.GetDfsNext();
        nullptr != pBB;
        pBB = pBB->GetDfsNext()) {
      auto const parent = pBB->GetParent();
      parent->GetChildren().Append(pBB->GetDomInfo());
    }
  }

  //  Loop over bblock which has more than one predecessors.
  private: void ComputeFrontiers() {
    foreach (Function::EnumBBlock, oEnum, cfg_.function()) {
      auto const node = oEnum.Get()->Extend<DomBBlock>();

      if (!HasMoreThanOnePred(node)) {
        continue;
      }

      foreach (DomBBlock::EnumInEdge, oEnum, node) {
        auto const idom = node->GetParent();
        for (auto runner = oEnum.GetNode(); 
            runner != idom;
            runner = runner->GetParent()) {
          AddFrontier(*runner, *node);
        }
      }
    }
  }

  //  Computes parent (immediate dominator) for each bblock.
  private: void ComputeParent() {
    entry_.SetParent(&entry_);
    entry_.SetDepth(1);
    auto changed = true;
    while (changed) {
      changed = false;
      for (auto pBB = entry_.GetDfsNext(); pBB; pBB = pBB->GetDfsNext()) {
        if (ComputeParentAux(pBB)) {
          changed = true;
        }
      }
    }
  }

  //  Computes new parent by processed predecessor.
  private: bool ComputeParentAux(DomBBlock* curr) {
    ASSERT(curr != nullptr);

    foreach (DomBBlock::EnumInEdge, oEnum, curr) {
      auto parent = oEnum.GetNode();

      if (parent->GetParent() == nullptr) {
        continue;
      }

      foreach (DomBBlock::EnumInEdge, oEnum, curr) {
        auto const pred = oEnum.GetNode();
        if (parent != pred && pred->GetParent() != nullptr) {
          parent = &Intersect(*pred, *parent);
        }
      }

      if (curr->GetParent() != parent) {
        curr->SetParent(parent);
        curr->SetDepth(parent->GetDepth() + 1);
        return true;
      }
    }

    return false;
  }

  private: static bool HasMoreThanOnePred(DomBBlock* node) {
    DomBBlock::EnumInEdge oEnum(node);
    if (oEnum.AtEnd()) {
        return false;
    }
    oEnum.Next();
    return !oEnum.AtEnd();
  }

  //  Allocates dominfo for all bblocks
  private: void Init() {
    foreach (Function::EnumBBlock, bblocks, cfg_.function()) {
      auto& bblock = *bblocks.Get()->Extend<DomBBlock>();
      bblock.SetDfsName(0);
      bblock.SetDfsNext(nullptr);

      if (auto const pDomInfo = bblock.GetDomInfo()) {
        pDomInfo->Reset();
      } else {
        bblock.SetDomInfo(new DomInfo(bblock));
      }
    }
  }

  private: DomBBlock& Intersect(DomBBlock& finger1in, DomBBlock& finger2in) {
    auto finger1 = &finger1in;
    auto finger2 = &finger2in;

    while (finger1 != finger2) {
      while (finger1->GetDfsName() < finger2->GetDfsName()) {
        finger1 = finger1->GetParent();
      }

      while (finger2->GetDfsName() < finger1->GetDfsName()) {
        finger2 = finger2->GetParent();
      }
   }

    return *finger1;
  }

  private: void RemoveUnrechable() {
    Function::EnumBBlock oEnum(cfg_.function());
    while (!oEnum.AtEnd()) {
        BBlock* pBB = oEnum.Get();
        oEnum.Next();

        if (pBB->Extend<DomBBlock>()->GetDfsName() == 0) {
          pBB->GetFunction()->RemoveBBlock(*pBB);
        }
    }
  }

  private: void Uninit() {
    foreach (Function::EnumBBlock, oEnum, cfg_.function()) {
      auto const pBB = oEnum.Get();
      pBB->Reset();
    }
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

    foreach (Function::EnumBBlock, oEnum, fun.function()) {
        auto const pBBlock = oEnum.Get()->Extend<DomBBlock>();

        pLogWriter->StartElement("tr");

        pLogWriter->StartElement("td");
        pLogWriter->Write(pBBlock);
        pLogWriter->EndElement("td");

        auto const pDomInfo = pBBlock->GetDomInfo();

        pLogWriter->WriteElement("td", "%d", pDomInfo->GetDepth());

        if (pDomInfo->GetParent() == nullptr) {
            pLogWriter->StartElement("td");
            pLogWriter->Write("-");
            pLogWriter->EndElement("td");

        } else {
            pLogWriter->StartElement("td");
            pLogWriter->Write(pDomInfo->GetParent());
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

template<class Base_, class EnumInEdge_, class EnumOutEdge_>
class DomBBlock_ : public Base_ {
  private: typedef BBlock::DomInfo DomInfo;
  private: typedef DomBBlock_<Base_, EnumInEdge_, EnumOutEdge_> BBlock_;

  public: uint GetDfsName() const { return static_cast<uint>(GetIndex()); }
  public: BBlock_* GetDfsNext() const { return GetWork<BBlock_>(); }
  public: void SetDfsName(uint nName) { SetIndex(nName); }
  public: void SetDfsNext(BBlock_* pNext) { SetWork(pNext); }

  public: class EnumInEdge : public EnumInEdge_ {
    public: EnumInEdge(BBlock* pBB) : EnumInEdge_(pBB) {}

    public: BBlock_* GetNode() const { 
      return EnumInEdge_::GetNode()->Extend<BBlock_>(); 
    }

    DISALLOW_COPY_AND_ASSIGN(EnumInEdge);
  };

  public: class EnumOutEdge : public EnumOutEdge_ {
    public: EnumOutEdge(BBlock* pBB) :
        EnumOutEdge_(pBB) {}

    public: BBlock_* GetNode() const { 
      return EnumOutEdge_::GetNode()->Extend<BBlock_>(); 
    }

    DISALLOW_COPY_AND_ASSIGN(EnumOutEdge);
  };

  public: DomInfo::ChildList& GetChildren() const {
    return GetDomInfo()->GetChildren();
  }

  public: uint GetDepth() const { return GetDomInfo()->GetDepth(); }
  public: uint SetDepth(uint n) { return GetDomInfo()->SetDepth(n); }

  public: DomInfo::FrontierList& GetFrontiers() const {
    return GetDomInfo()->GetFrontiers();
  }

  public: BBlock_* GetParent() const {
    return GetDomInfo()->GetParent()->Extend<BBlock_>();
  }

  public: void SetParent(BBlock_* pBB) { GetDomInfo()->SetParent(pBB); }
  DISALLOW_COPY_AND_ASSIGN(DomBBlock_);
};

class DomBBlockBase : public BBlock {
  private: DomBBlockBase() {}
  public: DomInfo* GetDomInfo() const { return m_pDomInfo; }
  public: void SetDomInfo(DomInfo* pDomInfo) { m_pDomInfo = pDomInfo; }
  DISALLOW_COPY_AND_ASSIGN(DomBBlockBase);
};

class PostDomBBlockBase : public BBlock {
  private: PostDomBBlockBase() {}
  public: DomInfo* GetDomInfo() const { return m_pPostDomInfo; }
  public: void SetDomInfo(DomInfo* pDomInfo) { m_pPostDomInfo = pDomInfo; }
  DISALLOW_COPY_AND_ASSIGN(PostDomBBlockBase);
};

typedef DomBBlock_<
    DomBBlockBase,
    BBlock::EnumInEdge,
    BBlock::EnumOutEdge >
DomBBlock;

typedef DomBBlock_<
    PostDomBBlockBase,
    BBlock::EnumOutEdge,
    BBlock::EnumInEdge >
PostDomBBlock;

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
