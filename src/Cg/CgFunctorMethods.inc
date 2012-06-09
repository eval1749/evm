// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita IL Applyable IR Objects.
//

#if !defined(CG_FUNCTOR_METHOD_2_)
    #error "You must define CG_FUNCTOR_METHOD_2_"
#endif // !defined(CG_FUNCTOR_METHOD_2_)

CG_FUNCTOR_METHOD_2_(CopyInstruction, CgInstruction)

CG_FUNCTOR_METHOD_2_(CgOutput, Output)
CG_FUNCTOR_METHOD_2_(ClosedMarker, CgPseudoOutput)
CG_FUNCTOR_METHOD_2_(FunLit, CgImmediate)
CG_FUNCTOR_METHOD_2_(MemSlot, CgOutput)
CG_FUNCTOR_METHOD_2_(Physical, CgOutput)
CG_FUNCTOR_METHOD_2_(StackSlot, MemSlot)
CG_FUNCTOR_METHOD_2_(ThreadSlot, MemSlot)
CG_FUNCTOR_METHOD_2_(VarHome, MemSlot)

#undef CG_FUNCTOR_METHOD_2_
