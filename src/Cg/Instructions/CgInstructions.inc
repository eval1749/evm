// -*- Mode: C++; -*-
// @(#)$Id$
//

#if !defined(DEFCGINSTRUCTION)
  #error "You must define DEFCGINSTRUCTION."
#endif

DEFCGINSTRUCTION(Assign, 1, Copy, "ASSIGN ty %rd <= %sx")
DEFCGINSTRUCTION(Copy, 1, Copy, "COPY ty %rd <= %sx")
DEFCGINSTRUCTION(PhiCopy, 1, Copy, "PHICOPY %rd <= %sx")
DEFCGINSTRUCTION(ReinterpretCast, 1, Cg, "REINTERPRETCAST ty %rd <= %rx")
DEFCGINSTRUCTION(Reload, 1, Copy, "RELOAD ty %pd <= %mx")
DEFCGINSTRUCTION(Spill, 1, Copy, "SPILL ty %md <= %px")
DEFCGINSTRUCTION(Swap, 1, Cg, "SWAP ty %pd <= %px")
DEFCGINSTRUCTION(UpVarRef, 2, Cg, "UPVARREF ty %rd <= %rx %qy")
DEFCGINSTRUCTION(VarAnnex, 1, Cg, "VARANNEX ty %md <= %cx")
DEFCGINSTRUCTION(VarHome, 2, Cg, "VARHOME ty %md <= %qx %ry")
DEFCGINSTRUCTION(VarRef, 1, Cg, "VARREF ty %rd <= %qx")

#undef DEFCGINSTRUCTION
