// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Compiler_Syntax_DomBuilder_h)
#define INCLUDE_test_Compiler_Syntax_DomBuilder_h

namespace Dom {

class Document;
class Element;

class Builder {
  private: typedef const String& S;

  private: Document* const document_;
  private: Element* element_;

  // ctor
  public: Builder();
  public: ~Builder();

  // [E]
  public: Element& EmptyElement(S, S, S);
  public: Element& EmptyElement(S, S, S, S, S);

  public: void EndElement(S);

  // [F]
  public: Document& Finish();

  // [I]
  public: String InternName(S);

  // [S]
  public: Element& SetAttribute(S, int);
  public: Element& SetAttribute(S, S);

  public: Element& StartElement(S);
  public: Element& StartElement(S, S, S);
  public: Element& StartElement(S, S, S, S, S);

  DISALLOW_COPY_AND_ASSIGN(Builder);
}; // Builder

} // Dom

#endif // !defined(INCLUDE_test_Compiler_Syntax_DomBuilder_h)
