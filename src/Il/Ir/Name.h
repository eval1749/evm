// @(#)$Id$
//
// Evita Il Ir Name
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Name_h)
#define INCLUDE_Il_Ir_Name_h

namespace Il {
namespace Ir {

class Name : public WithCastable_<Name, Object> {
  CASTABLE_CLASS(Name);

  private: uint const hash_code_;
  private: String name_;

  private: Name(const String, uint);
  public: virtual ~Name() {}
  public: operator const String&() const { return name_; }

  // [C]
  public: int ComputeHashCode() const { return hash_code_; }

  // [I]
  public: static void LibExport Init();
  public: static const Name& LibExport Intern(const String&);

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(Name);
}; // Name

inline int ComputeHashCode(const Name* const p) {
  ASSERT(p != nullptr);
  return p->ComputeHashCode();
}

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Name_h)
