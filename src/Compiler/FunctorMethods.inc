// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler Applyable IR Objects.
//
// Note: When you add class here, you should also put the class to
// VisitFunctor.cpp.

#if !defined(FUNCTOR_METHOD_)
    #error "You must define FUNCTOR_METHOD_"
#endif // !defined(FUNCTOR_METHOD_)

FUNCTOR_METHOD_(AliasDef)
FUNCTOR_METHOD_(CastOperator)
FUNCTOR_METHOD_(ClassDef)
FUNCTOR_METHOD_(CompilationUnit)
FUNCTOR_METHOD_(CompileSession)
FUNCTOR_METHOD_(FieldDef)
FUNCTOR_METHOD_(Function)
FUNCTOR_METHOD_(LocalVarDef)
FUNCTOR_METHOD_(Keyword) // dummy
FUNCTOR_METHOD_(MethodDef)
FUNCTOR_METHOD_(MethodGroupDef)
FUNCTOR_METHOD_(MethodParamDef)
FUNCTOR_METHOD_(NameDef) // dummy
FUNCTOR_METHOD_(ClassOrNamespaceDef) // dummy
FUNCTOR_METHOD_(NamespaceBody)
FUNCTOR_METHOD_(NamespaceDef)
FUNCTOR_METHOD_(NamespaceMember) // dummy
FUNCTOR_METHOD_(Operator) // dummy
FUNCTOR_METHOD_(RankSpec) // dummy
FUNCTOR_METHOD_(Type) // dummy
FUNCTOR_METHOD_(TypeArgList) // dummy
FUNCTOR_METHOD_(UsingNamespace)
FUNCTOR_METHOD_(VarDef)

#undef FUNCTOR_METHOD_
