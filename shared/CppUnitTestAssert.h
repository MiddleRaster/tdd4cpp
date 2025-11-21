#ifndef MS_CPP_UNITTESTFRAMEWORK_ASSERT
#define MS_CPP_UNITTESTFRAMEWORK_ASSERT

// portable drop-in replacement for VS's Assert class

#include <cstdlib>   // for std::abs
#include <algorithm> // for std::transform
#include <source_location> // C++20+ only

#include "tddAssertStl.h"

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework
{
namespace Details
{
	inline char tolower(char in)
	{
		if(in <= 'Z' && in >= 'A')
			return in - ('A'-'a');
		return in;
	}
	inline void ToLower(std::string& s) { std::transform(s.begin(), s.end(), s.begin(), tolower); }
}

struct Assert
{
	template<typename T> static void AreEqual(const T& expected, const T& actual, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreEqual(expected, actual, message);
	}
	static void AreEqual(double expected, double actual, double tolerance, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (std::abs(expected - actual) > std::abs(tolerance))
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreEqual(expected, actual, message);
	}
	static void AreEqual(float expected, float actual, float tolerance, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if(std::abs(expected - actual) > std::abs(tolerance))
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreEqual(expected, actual, message);
	}
	static void AreEqual(const char* expected, const char* actual, bool ignoreCase = false, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (ignoreCase) {
			std::string expectedLower(expected);
			std::string actualLower  (actual);
			Details::ToLower(expectedLower);
			Details::ToLower(actualLower);
			if (expectedLower.compare(actualLower) == 0)
				return;
		}
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreEqual(expected, actual, message);
	}
	static void AreEqual(const wchar_t* expected, const wchar_t* actual, bool ignoreCase = false, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (ignoreCase) {
			std::string expectedLower(TDD::Details::FromWide(expected));
			std::string   actualLower(TDD::Details::FromWide(actual));
			Details::ToLower(expectedLower);
			Details::ToLower(actualLower);
			if (expectedLower.compare(actualLower) == 0)
				return;
		}
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreEqual(expected, actual, message);
	}
	template<typename T> static void AreSame(T* expected, T* actual, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreEqual((const void*)expected, (const void*)actual, message);
	}
	template<typename T> static void AreSame(const T& expected, const T& actual, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreEqual((const void*)&expected, (const void*)&actual, message);
	}
	template<typename T> static void AreNotEqual(const T& notExpected, const T& actual, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreNotEqual(notExpected, actual, message);
	}
	static void AreNotEqual(double notExpected, double actual, double tolerance, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (std::abs(notExpected - actual) <= std::abs(tolerance))
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreNotEqual(actual, actual, std::wstring(message));
	}
	static void AreNotEqual(float notExpected, float actual, float tolerance, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (std::abs(notExpected - actual) <= std::abs(tolerance))
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreNotEqual(actual, actual, std::wstring(message));
	}
	static void AreNotEqual(const char* notExpected, const char* actual, bool ignoreCase = false, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		std::string csExpected(notExpected);
		std::string csActual  (actual);
		if (ignoreCase) {
			Details::ToLower(csExpected);
			Details::ToLower(csActual);
		}
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreNotEqual(csExpected, csActual, message);
	}
	static void AreNotEqual(const wchar_t* notExpected, const wchar_t* actual, bool ignoreCase = false, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		std::string csExpected(TDD::Details::FromWide(notExpected));
		std::string csActual  (TDD::Details::FromWide(actual));
		if (ignoreCase) {
			Details::ToLower(csExpected);
			Details::ToLower(csActual);
		}
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreNotEqual(csExpected, csActual, message);
	}
	template<typename T> static void AreNotSame(T* notExpected, T* actual, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreNotEqual((const void*)notExpected, (const void*)actual, message);
	}
	template<typename T> static void AreNotSame(const T& notExpected, const T& actual, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).AreNotEqual((const void*)&notExpected, (const void*)&actual, message);
	}
	template<typename T> static void IsNull(const T* actual, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (!!actual)
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).Fail(message);
	}
	template<typename T> static void IsNotNull(const T* actual, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (!actual)
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).Fail(message);
	}
	static void IsTrue(bool condition, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (!condition)
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).Fail(message);
	}
	static void IsFalse(bool condition, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (condition)
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).Fail(message);
	}
	static void Fail(const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		if (message == NULL)
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).Fail("");
		else
			TDD::AssertT<std::string>(loc.line(), loc.file_name()).Fail(message);
	}
	template<typename _EXPECTEDEXCEPTION, typename _FUNCTOR> static void ExpectException(_FUNCTOR functor, const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).ExpectingException<_EXPECTEDEXCEPTION, _FUNCTOR>(functor, message);
	}
	template<typename _EXPECTEDEXCEPTION, typename _RETURNTYPE> static void ExpectException(_RETURNTYPE (*func)(), const wchar_t* message = L"", const std::source_location& loc = std::source_location::current())
	{
		TDD::AssertT<std::string>(loc.line(), loc.file_name()).ExpectingException<_EXPECTEDEXCEPTION,_RETURNTYPE (*)()>(func, message);
	}
};

}}}

#endif // MS_CPP_UNITTESTFRAMEWORK_ASSERT