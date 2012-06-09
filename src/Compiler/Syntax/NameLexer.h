// @(#)$Id$
//
// Evita Compiler - NameLexer.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_NameLexer_h)
#define INCLUDE_evc_NameLexer_h

#include "./Token.h"
#include "./TokenHandler.h"

namespace Compiler {

// NameLexer handles
//  o name => Name
//  o name ('.' name)+ => NameRef
// TODO(yosi) 2012-05-20 NYI: CastOperator, e.g. static_cast<type>
class NameLexer : public TokenProcessor {
  private: enum Mode {
    Mode_Expression,
    Mode_Type,
  };

  private: enum State {
    State_Start,

    State_Gt,
    State_Gt_Gt,
    State_Gt_Rank,

    State_Name,
    State_Name_Dot,
    State_Name_Dot_Name,
    State_Name_Dot_Name_Dot,
    State_Name_Lt,
    State_Name_Lt_Name,
    State_Name_Lt_Name_Dot,
    State_Name_Lt_Name_Rank,
    State_Name_Rank,
  };

  private: class Builder {
    private: ArrayList_<Builder*> builders_;
    private: const Token* close_token_;
    private: ArrayList_<const NameRef::Item*> items_;
    private: const Token* lookahead_token_;
    private: ArrayList_<int> ranks_;
    public: Builder(const NameToken&);
    public: ~Builder();
    public: void Add(Builder&);
    public: void Add(int);
    public: void Add(const NameToken&);
    private: const NameRef& MakeNameRef();
    private: const Token& MakeToken();
    public: void Process(TokenProcessor&);
    private: void ReleaseTokens();
    public: void SetComma(const Token&);
    public: void SetComposite();
    public: void SetDot(const Token&);
    public: void SetGt(const Token&);
    public: void SetGtGt(const Token&);
    public: void StartComposite(const Token&);
  };

  private: Builder* builder_;
  private: Stack_<Builder*> builder_stack_;
  private: Mode mode_;
  private: Stack_<Mode> mode_stack_;
  private: TokenProcessor& processor_;
  private: State state_;

  // ctor
  public: explicit NameLexer(TokenProcessor&);
  public: virtual ~NameLexer();

  // [E]
  private: void EndArgument();
  private: void EndTwoComposites();

  // [F]
  private: void FinishBuild();
  private: void FinishBuild(const Token&);

  // [H]
  private: void HandleGt(const Token&);
  private: void HandleGtGt(const Token&);

  // [P]
  public: void PopMode();
  public: virtual void ProcessToken(const Token&) override;

  // [S]
  public: void SetExpressionMode();
  private: void StartComposite(const Token&);

  DISALLOW_COPY_AND_ASSIGN(NameLexer);
};

} // Compiler

#endif // !defined(INCLUDE_evc_NameLexer_h)
