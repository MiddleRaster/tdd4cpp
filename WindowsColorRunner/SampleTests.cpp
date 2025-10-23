#include "..\CppUnitTest.h"

/*
    If you cannot use exceptions (e.g., for kernel mode tests), define:
#undef _CPPUNWIND
#include "..\tdd.h"
    Then use the TDD_VERIFY_* macros to report errors.
*/

namespace SomeNamespace_UsingCppUnitStyleAsserts
{
    using namespace Microsoft::VisualStudio::CppUnitTestFramework;
    TEST_CLASS(SomeClass)
    {
        TEST_METHOD(SomeTest)
        {
            Assert::AreEqual(1, 2, L"nope, not equal");
        }
    };
}

namespace SomeNamespace
{
    namespace IfYouCannotUseCxx20
    {
        TEST_CLASS(SomeClass)
        {
            TEST_METHOD(AnotherTest)
            {
                TddAssert().AreEqual(1, 2, "nope, still not equal");
            }
        };
    }
}

namespace IfYouCannotUseExceptionsAtAll
{
    TEST_CLASS(SomeClass)
    {
        TEST_METHOD(AFinalTest)
        {
            TDD_VERIFY_EQUAL(1, 2);
        }
    };
}
