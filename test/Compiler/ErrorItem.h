// @(#)$Id$
//
// Evita Compiler - ErrorItem
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Compiler_ErrorItem)
#define INCLUDE_test_Compiler_ErrorItem

#include <ostream>

namespace CompilerTest {

using Il::Ir::ErrorInfo;

class ErrorList;

class ErrorItem : public Object_<ErrorItem> {
  CASTABLE_CLASS(ErrorItems);
  private: String code_;
  private: int line_;
  private: int column_;
  private: Collection_<Box> params_;

  public: ErrorItem(const String&, int line, int column);
  public: ErrorItem(const String&, int, int column, Box);
  public: ErrorItem(const String&, int, int column, Box, Box);
  public: ErrorItem(const ErrorItem&);
  public: ErrorItem(const ErrorInfo&);
  public: ErrorItem();
  public: ErrorItem& operator =(const ErrorItem&);
  public: bool operator ==(const ErrorItem&) const;
  public: bool operator ==(const ErrorList&) const;
  public: bool operator !=(const ErrorItem&) const;
  public: bool operator !=(const ErrorList&) const;
  public: virtual String ToString() const override;
};

::std::ostream& operator <<(::std::ostream&, const ErrorItem&);

class ErrorList : public Object_<ErrorList> {
  CASTABLE_CLASS(ErrorList);
  private: ArrayList_<ErrorItem> items_;
  public: ErrorList() {}
  public: ErrorList(const ErrorList&);
  public: bool operator =(const ErrorList&) const;
  public: void Add(const ErrorItem&);
  public: bool operator ==(const ErrorList&) const;
  public: bool operator ==(const ErrorItem&) const;
  public: virtual String ToString() const override;
};

::std::ostream& operator <<(::std::ostream&, const ErrorList&);

} // CompilerTest

#endif // !defined(INCLUDE_test_Compiler_ErrorItem)
