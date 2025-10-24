# tdd4cpp

This is a drop-in replacement for Visual Studio's Native C++ Unit Testing framework.
It's header-only, doesn't include any other headers, and is portable to MacOS, Linux, etc.

If you want "VS native C++"-style unit tests, ```#include "CppUnitTest.h"```. Usage is ```Assert::AreEqual(1, 2);```

If you cannot use C++20 features, ```#include "TddAssertStl.h"```. Usage is ```TddAssert().AreEqual(1, 2);```

If you cannot use std::string, you can use your own string type in ```tddAssertBase.h```

If you cannot use exceptions, ```#include "tdd.h"``` and use the ```TDD_VERIFY*``` macros.  Usage is ```TDD_VERIFY_EQUAL(1, 2);```


There are test runners for:
 - a Windows command-line app with colors (red=failure, yellow=no tests run, green=all tests passed);
 - a portable command-line app without colors;
 - a Windows GUI which loads your tests from a dll (like NUnit does).


Finally, if you want to replace Visual Studio's dreadfully slow test runner entirely, you can compile your tests and link them into either command-line runner, and then make the runner a post-build step.
The tests will run automatically after a successful build, and the assertion failures will show up in the output window and in the error list window as **warnings**. 
They are clickable, which will take you to the source file and the line on which the assertion fired.
