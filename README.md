# tdd4cpp

This is a drop-in replacement for Visual Studio's Native C++ Unit Testing framework.
It's header-only, doesn't include any other headers, and is portable to MacOS, Linux, etc.

Here are the three ways to write asserts:
```cpp
#include "..\shared\CppUnitTest.h"

namespace YourNamespace
{
    namespace UsingCppUnitStyleAsserts
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
}
```

### Summary

| if you | then | usage is |
|--------|------|----------|
| want "VS native C++"-style unit tests | ```#include "CppUnitTest.h"``` | ```Assert::AreEqual(1, 2);``` |
| cannot use C++20 features | ```#include "TddAssertStl.h"``` | ```TddAssert().AreEqual(1,2);``` |
| cannot use ```std::string``` | use your own string type in ```tddAssertBase.h``` | ```TddAssert().AreEqual(1,2);``` |
| cannot use exceptions | ```#include "tdd.h"```; <br> use the ```TDD_VERIFY*``` macros | ```TDD_VERIFY_EQUAL(1,2);``` |

### Test Runners

There are test runners for:
 - a completely portable command-line app;
 - a Windows command-line app with colors (red=failure, yellow=no tests run, green=all tests passed);
 - (future) a Windows GUI which loads your tests from a dll (like NUnit does).

### Visual Studio integration

Finally, if you want to replace Visual Studio's dreadfully slow test runner entirely, you can compile your tests and link them into either command-line runner, and then make the runner a post-build step.
The tests will run automatically after a successful build, and the assertion failures will show up in the output window and in the error list window as **warnings**. 
They are <u>clickable</u>, which will take you to the source file and the line on which the assertion fired.
