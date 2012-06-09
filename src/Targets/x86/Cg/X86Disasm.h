// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_X86Disasm_h)
#define INCLUDE_Il_Cg_X86Disasm_h

namespace Ee {
class Ee::CodeDesc;
} // Ee

namespace Il {
namespace X86 {

void LibExport X86Disassemble(TextWriter&, const Ee::CodeDesc&);

} // X86
} // Il

#endif // !defined(INCLUDE_Il_Cg_X86Disasm_h)
