// precomp.h
#if !defined(Evita_test_Il_precomp_h)
#define Evita_test_Il_precomp_h


// warning C4625: 'testing::internal::TestFactoryImpl<TestClass>' : copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable: 4625)

// warning C4626: 'testing::internal::TestFactoryImpl<TestClass>' : assignment operator could not be generated because a base class assignment operator is inaccessible
#pragma warning(disable: 4626)

// warning C4826: Conversion from 'const void *' to 'testing::internal::UInt64' is sign-extended. This may cause unexpected runtime behavior.
#pragma warning(disable: 4826)

// warning C4986: 'operator new[]': exception specification does not match previous declaration
#pragma warning(disable: 4986)

#include "Il/precomp.h"

#endif // !defined(Evita_test_Il_precomp_h)
