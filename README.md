# tdd4cpp

This is a drop-in replacement for Visual Studio's Native C++ Unit Test harness.
It's header-only, doesn't include any other headers, and is portable to MacOS, Linux, etc.

If you want "VS native C++"-style unit tests, #include "CppUnitTest.h". Usage is "Assert::AreEquals(1, 2);"

If you cannot use C++20 features, include "TddAssertStl.h". Usage is "TddAssert().AreEquals(1, 2);"

If you cannot use std::string, you can use your own string type in "tddAssertBase.h"

If you cannot use exceptions, include "tdd.h" and use the TDD_VERIFY* macros.  Usage is "TDD_VERIFY_EQUAL(1, 2);"


There are test runners for:
 - a Windows command-line with colors (red=failure, yellow=no tests run, green=all tests passed);
 - a portable command-line without colors;
 - a Windows GUI which loads your tests as dll (like NUnit).
