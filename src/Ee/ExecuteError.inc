// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Executor
//

#if !defined(DEFERROR)
    #error "You must define DEFERROR macro."
#endif // !defined(DEFERROR)

DEFERROR(Command, NotFound)

DEFERROR(Dump, NotSupported)

DEFERROR(Invoke, NoCodeDesc)
DEFERROR(Invoke, NoFunction)
DEFERROR(Invoke, NoModule)

DEFERROR(Pass, Failed)

#undef DEFERROR
