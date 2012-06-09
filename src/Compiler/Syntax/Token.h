// @(#)$Id$
//
// Evita Compiler - Token
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evita_Compiler_Syntax_Token_h)
#define INCLUDE_evita_Compiler_Syntax_Token_h

namespace Compiler {

using Common::Collections::ArrayList_;
class NameToken;

// Token is input for Parser.
// We can use Token API as:
//  if (token.Is(*Op__CloseBase) {...}
//  if (token.Is(*K_namespace)) {...}
//  if (auto const names_token = token.DynamicCast<QualifiedNameToken>()) {...}
class Token
    : public WithCastable_<Token, Object>,
      public RefCounted_<Token> {
  CASTABLE_CLASS(Token);

  private: const SourceInfo source_info_;

  // ctor
  protected: Token(const SourceInfo& source_info)
    : source_info_(source_info) {}

  public: virtual ~Token() {}

  // properties
  public: const SourceInfo& source_info() const { return source_info_; }

  // [I]
  public: virtual bool Is(const Keyword&) const { return false; }
  public: virtual bool Is(const Name&) const { return false; }
  public: virtual bool Is(const Operator&) const { return false; }

  DISALLOW_COPY_AND_ASSIGN(Token);
};

template<class TSelf, class TParent = Token>
class Token_ : public WithCastable1_<TSelf, TParent, const SourceInfo&> {
  protected: explicit Token_(const SourceInfo source_info)
    : WithCastable1_(source_info) {}
  DISALLOW_COPY_AND_ASSIGN(Token_);
};

// Base class of literal tokens.
class LiteralToken : public Token_<LiteralToken> {
  CASTABLE_CLASS(LiteralToken);
  protected: explicit LiteralToken(const SourceInfo&);
  public: virtual ~LiteralToken() {}
  public: virtual const Type& literal_type() const = 0;
  public: virtual Literal& ToLiteral(MemoryZone&) const = 0;
  DISALLOW_COPY_AND_ASSIGN(LiteralToken);
};

// Produced by Lexer.
class CharToken : public Token_<CharToken, LiteralToken> {
  CASTABLE_CLASS(CharToken);
  private: char16 code_;
  public: CharToken(const SourceInfo&, char16);
  public: virtual ~CharToken() {}
  public: char16 code() const { return code_; }
  public: virtual const Type& literal_type() const override;
  public: virtual Literal& ToLiteral(MemoryZone&) const override;
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(CharToken);
};

// Produced by Lexer.
class Float32Token : public Token_<Float32Token, LiteralToken> {
  CASTABLE_CLASS(Float32Token);
  private: float32 const f32_;
  public: Float32Token(const SourceInfo&, float32);
  public: virtual ~Float32Token() {}
  public: float32 value() const { return f32_; }
  public: virtual const Type& literal_type() const override;
  public: virtual Literal& ToLiteral(MemoryZone&) const override;
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(Float32Token);
};

// Produced by Lexer.
class Float64Token : public Token_<Float64Token, LiteralToken> {
  CASTABLE_CLASS(Float64Token);
  private: float64 const f64_;
  public: Float64Token(const SourceInfo&, float64);
  public: virtual ~Float64Token() {}
  public: float64 value() const { return f64_; }
  public: virtual const Type& literal_type() const override;
  public: virtual Literal& ToLiteral(MemoryZone&) const override;
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(Float64Token);
};

// Produced by Lexer.
class IntToken : public Token_<IntToken, LiteralToken> {
  CASTABLE_CLASS(IntToken);
  private: int64 const i64_;
  private: const Type& int_type_;
  public: IntToken(const SourceInfo&, const Type&, int64);
  public: virtual ~IntToken() {}
  public: int64 value() const { return i64_; }
  public: virtual const Type& literal_type() const override;
  public: virtual Literal& ToLiteral(MemoryZone&) const override;
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(IntToken);
};

// Produced by Lexer.
class KeywordToken : public Token_<KeywordToken> {
  CASTABLE_CLASS(KeywordToken);
  private: const Keyword& keyword_;
  public: KeywordToken(const SourceInfo&, const Keyword&);
  public: virtual ~KeywordToken() {}
  public: const Keyword& value() const { return keyword_; }
  public: virtual bool Is(const Keyword&) const override;
  public: virtual bool Is(const Name&) const override { return false; }
  public: virtual bool Is(const Operator&) const override { return false; }
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(KeywordToken);
};

// Produced by Lexer.
class NameToken : public Token_<NameToken> {
  CASTABLE_CLASS(NameToken);
  private: const Name& name_;
  public: NameToken(const SourceInfo&, const Name&);
  public: virtual ~NameToken() {}
  public: const Name& name() const { return name_; }
  public: virtual bool Is(const Keyword&) const override { return false; }
  public: virtual bool Is(const Name&) const override;
  public: virtual bool Is(const Operator&) const override { return false; }
  public: const NameRef& ToNameRef() const;
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(NameToken);
};

class NameRefToken : public Token_<NameRefToken> {
  CASTABLE_CLASS(NameRefToken);
  private: const NameRef& name_ref_;
  public: NameRefToken(const NameRef&);
  public: virtual ~NameRefToken() {}
  public: const NameRef& name_ref() const { return name_ref_; }
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(NameRefToken);
};

// Produce by Lexer.
class OperatorToken : public Token_<OperatorToken> {
  CASTABLE_CLASS(OperatorToken);
  private: const Operator& operator_;
  public: OperatorToken(const SourceInfo&, const Operator&);
  public: virtual ~OperatorToken() {}
  public: const Operator& value() const { return operator_; }
  public: virtual bool Is(const Keyword&) const override { return false; }
  public: virtual bool Is(const Name&) const override { return false; }
  public: virtual bool Is(const Operator&) const override;
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(OperatorToken);
};

// Produced by Lexer from "[" ","* "]".
class RankSpecToken : public Token_<RankSpecToken> {
  CASTABLE_CLASS(RankToken);
  private: int const rank_;
  public: RankSpecToken(const SourceInfo&, int);
  public: virtual ~RankSpecToken() {}
  public: int rank() const { return rank_; }
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(RankSpecToken);
};

// Produced by NameLexer.
// Note: We should construct NameRef w/o method.
class QualifiedNameToken : public Token_<QualifiedNameToken> {
  CASTABLE_CLASS(QualifiedNameToken);
  private: typedef Collection_<const NameToken*> NameTokens;
  public: class Enum : public NameTokens::Enum {
    private: typedef  NameTokens::Enum Base;
    public: Enum(const QualifiedNameToken& r) : Base(r.names_) {}
    DISALLOW_COPY_AND_ASSIGN(Enum);
  };
  private: NameTokens names_;
  public: explicit QualifiedNameToken(const Collection_<const NameToken*>&);
  public: virtual ~QualifiedNameToken();
  public: const NameRef& ToNameRef() const;
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(QualifiedNameToken);
};

// Produced by Lexer from "@" name.
class RawNameToken
    : public WithCastable2_<
        RawNameToken,
        NameToken,
        const SourceInfo&,
        const Name&> {
  CASTABLE_CLASS(RawNameToken);
  public: RawNameToken(const SourceInfo&, const Name&);
  public: virtual ~RawNameToken() {}
  public: virtual bool Is(const Keyword&) const override { return false; }
  public: virtual bool Is(const Name&) const override { return false; }
  public: virtual bool Is(const Operator&) const override { return false; }
  public: String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(RawNameToken);
};

// Produced by Lexer.
class StringToken : public Token_<StringToken, LiteralToken> {
  CASTABLE_CLASS(StringToken);
  private: const String string_;
  public: StringToken(const SourceInfo&, const String&);
  public: virtual ~StringToken() {}
  public: String value() const { return string_; }
  public: virtual Type& literal_type() const override { return *Ty_String; }
  public: virtual Literal& ToLiteral(MemoryZone&) const override;
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(StringToken);
};

// o NameLexer produces from NameToken RankSpecToken.
// o TypeArgsLexer produces for type argument list.
class TypeToken : public Token_<TypeToken> {
  CASTABLE_CLASS(TypeToken);
  private: const Type& type_;
  public: TypeToken(const SourceInfo&, const Type&);
  public: virtual ~TypeToken() {}
  public: const Type& type() const { return type_; }
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(TypeToken);
};

// List of types produced by TypeArgsLexer as result of
// sequence of tokens: name "<" (name ",")+ ">".
class TypeListToken : public Token_<TypeListToken> {
  CASTABLE_CLASS(TypeListToken);
  private: const Type::Collection types_;
  public: class Enum : public Type::Collection::Enum {
    private: typedef Type::Collection::Enum Base;
    public: Enum(const TypeListToken& r) : Base(r.types_) {}
    DISALLOW_COPY_AND_ASSIGN(Enum);
  };
  public: TypeListToken(const SourceInfo&, const Type::Collection&);
  public: virtual ~TypeListToken() {}
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(TypeListToken);
};

} // Compiler

#endif // !defined(INCLUDE_evita_Compiler_Syntax_Token_h)
