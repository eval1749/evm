// @(#)$Id$
//
// Evita Compiler - SourceInfo
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_SourceInfo_h)
#define INCLUDE_Il_Ir_SourceInfo_h

namespace Il {
namespace Ir {

// To support "#line" directive, each source info has source file name
// information.
class SourceInfo {
  private: int column_;
  private: int line_;
  private: String name_;

  // ctor
  public: SourceInfo();
  public: SourceInfo(const String&, int, int);

  // operators
  public: bool operator==(const SourceInfo&) const;
  public: bool operator!=(const SourceInfo&) const;
  public: bool operator<(const SourceInfo&) const;
  public: bool operator<=(const SourceInfo&) const;
  public: bool operator>(const SourceInfo&) const;
  public: bool operator>=(const SourceInfo&) const;

  // properties
  public: int column() const { return column_; }
  public: String file_name() const { return name_; }
  public: int line() const { return line_; }

  // [C]
  public: int ComputeHashCode() const;

  // [E]
  public: bool Equals(const SourceInfo&) const;

  // [G]
  public: int GetColumn() const { return column_; }
  public: int GetLine() const { return line_; }
  public: String GetFileName() const { return name_; }

  // [H]
  public: bool HasInfo() const { return !name_.IsEmpty(); }
};

static_assert(
    sizeof(SourceInfo) == (sizeof(void*) == 4 ? 16 : 32),
    "SourceInfo is too large");
} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_SourceInfo_h)
