#ifndef TDDASSERTBASE_H
#define TDDASSERTBASE_H

#include <typeinfo>

#ifndef _CPPUNWIND
#error TddAssert* requires exceptions. If exceptions cannot be used in your test project, use the TDD_VERIFY* macros in tdd.h.
#endif

#include "tdd.h"

#ifndef _In_z_
 #ifdef TDD_NO_SAL_H
  #define TDD_DEFINED_SAL_MACROS
  #define _In_z_
 #else
  #error please #include this file after sal.h or specstrings.h or #define TDD_NO_SAL_H
 #endif
#endif

namespace TDD
{

// many specializations already provided, but the test writer must supply specializations for user-defined classes
namespace Details { template <typename string, typename T> struct AlwaysFalse { enum { value = false }; }; }
template <typename string, typename T> string ToString(const T* t) { static_assert(Details::AlwaysFalse<string, T>::value, "test writer must write a specialization for this T"); }
template <typename string, typename T> string ToString(const T& t) { static_assert(Details::AlwaysFalse<string, T>::value, "test writer must write a specialization for this T"); }

// implemented in tddAssertStl.h, tddAssertAtl.h or other string-type-specific headers.
template <typename string>               bool IsEmpty (const string& s);
template <typename string>        const char* ToAsciiz(const string& s);

template<class string> class AssertException : public TddException
{
	const string m_message, m_file;
	AssertException(unsigned long line, _In_z_ const char* file, const string& cs)
		: TDD::TddException(line, "")
		, m_message(cs)
		, m_file(file)
	{
		SetFile   (ToAsciiz(m_file));
		SetMessage(ToAsciiz(m_message));
	}
public:
	AssertException(unsigned long line, _In_z_ const char* file)
		: TDD::TddException(line, "")
		, m_file(file)
	{
		SetFile(ToAsciiz(m_file));
	}
	virtual ~AssertException() {}
	void ThrowAssertException(const string& cs) const
	{
		throw AssertException(GetLine(), GetFile(), cs); // throwing a copy
	}
private:
	AssertException& operator=(const AssertException&) = delete;
};
template<class string> struct StatelessAssertUtils : AssertException<string>
{
	StatelessAssertUtils(unsigned long line, _In_z_ const char* file) : AssertException<string>(line, file) {}
	template <typename S, typename T> void AreEqual(const S& expected, const T& actual, const string& message=string()) const
	{
		string csExpected = ToString<string>(expected);
		string csActual   = ToString<string>(actual);
		if (csExpected != csActual)
		{
			string cs  = "Expected <";
			cs += csExpected;
			cs += "> Actual <";
			cs += csActual;
			cs += ">";
			if (!IsEmpty(message))
			{
				cs += " - ";
				cs += message;
			}
			AssertException<string>::ThrowAssertException(cs);
		}
	}
	template <typename S, typename T> void AreNotEqual(const S& expected, const T& actual, const string& message=string()) const
	{
		string csActual = ToString<string>(actual);
		if (ToString<string>(expected) == csActual)
		{
			string cs = "Unexpected equality <";
			cs += csActual;
			cs += ">";
			if (!IsEmpty(message))
			{
				cs += " - ";
				cs += message;
			}
			AssertException<string>::ThrowAssertException(cs);
		}
	}

	void IsWithin(double expected, double actual, double epsilon, const string& message=string()) const
	{
		bool b = expected < actual ? actual - expected < epsilon : expected - actual < epsilon; // avoid std::fabs.
		if (b == false)
		{
			string cs = "expected <" + ToString<string>(expected) + "> to be within <" + ToString<string>(epsilon) + "> of <" + ToString<string>(actual) + ">";
			AssertException<string>::ThrowAssertException(cs);
		}
	}
};
template<typename T, class string> class StatefulAssertUtils: public StatelessAssertUtils<string>
{
	const T& m_actual;
public:
	StatefulAssertUtils(const StatelessAssertUtils<string>& sau, const T& actual)
		: StatelessAssertUtils<string>(sau)
		, m_actual(actual)
	{}
	template <typename S> void    AreEqual(const S& expected, const string& message=string()) const { StatelessAssertUtils<string>::AreEqual   (expected, m_actual, message); }
	template <typename S> void AreNotEqual(const S& expected, const string& message=string()) const { StatelessAssertUtils<string>::AreNotEqual(expected, m_actual, message); }
};

namespace Fluent
{
	template <typename T, class string> class Not
	{
		const StatefulAssertUtils<T,string> m_state; // using containment, rather than inheritance, so that IntelliSense works better
	public:
		Not(const StatefulAssertUtils<T,string>& sau) : m_state(sau) {}
		template <typename S> void EqualTo(const S& expected, const string& message=string()) const { m_state.AreNotEqual(expected, message); }
		                      void Null   (                   const string& message=string()) const { m_state.AreNotEqual(0,        message); }
		                      void True   (                   const string& message=string()) const { m_state.AreNotEqual(true,     message); }
		                      void False  (                   const string& message=string()) const { m_state.AreNotEqual(false,    message); }
		// wide (etc.) versions
		template <typename S, typename W> void EqualTo(const S& expected, const W& message) const { EqualTo(expected, ToString<string>(message)); }
		template <            typename W> void Null   (                   const W& message) const { Null   (          ToString<string>(message)); }
		template <            typename W> void True   (                   const W& message) const { True   (          ToString<string>(message)); }
		template <            typename W> void False  (                   const W& message) const { False  (          ToString<string>(message)); }
	};

	template <typename T, class string> class Is
	{
		const StatefulAssertUtils<T,string> m_state; // using containment, rather than inheritance, so that IntelliSense works better
	public:
		const Fluent::Not<T, string> Not;
		Is(const StatefulAssertUtils<T,string>& sau)
			: m_state(sau)
			, Not(m_state)
		{}
		template <typename S> void EqualTo(const S& expected, const string& message=string()) const { m_state.AreEqual(expected, message); }
		                      void Null   (                   const string& message=string()) const { m_state.AreEqual(0,        message); }
		                      void True   (                   const string& message=string()) const { m_state.AreEqual(true,     message); }
		                      void False  (                   const string& message=string()) const { m_state.AreEqual(false,    message); }
		// wide (etc.) versions
		template <typename S, typename W> void EqualTo(const S& expected, const W& message) const { EqualTo(expected, ToString<string>(message)); }
		template <            typename W> void Null   (                   const W& message) const { Null   (          ToString<string>(message)); }
		template <            typename W> void True   (                   const W& message) const { True   (          ToString<string>(message)); }
		template <            typename W> void False  (                   const W& message) const { False  (          ToString<string>(message)); }
	};

	template <typename T, class string> class That
	{
		const StatefulAssertUtils<T,string> m_state; // using containment, rather than inheritance, so that IntelliSense works better
	public:
		const Fluent::Is <T, string> Is;
		const Fluent::Not<T, string> IsNot;
		That(const T& actual, const StatelessAssertUtils<string>& sau)
			: m_state(sau, actual)
			, Is   (m_state)
			, IsNot(m_state)
		{}
	};
}

template<class string> class AssertT
{
	const StatelessAssertUtils<string> m_utils; // using containment, rather than inheritance, so that IntelliSense works better
public:
	AssertT(unsigned long line, _In_z_ const char* file) : m_utils(line, file) {}

	template <typename S, typename T> void AreEqual   (const S& expected, const T& actual, const string& message=string()) { m_utils.AreEqual   (expected, actual, message); }
	template <typename S, typename T> void AreNotEqual(const S& expected, const T& actual, const string& message=string()) { m_utils.AreNotEqual(expected, actual, message); }
	template <            typename T> void IsFalse    (                   const T& actual, const string& message=string()) { m_utils.AreEqual   (false,    actual, message); }
	template <            typename T> void IsTrue     (                   const T& actual, const string& message=string()) { m_utils.AreEqual   ( true,    actual, message); }
						     void IsWithin(double expected, double actual, double epsilon, const string& message=string()) { m_utils.IsWithin(expected, actual, epsilon, message); }
	                                  void Fail       (                                    const string& message         ) { m_utils.ThrowAssertException( string(message)); }


	template <typename E, typename L> void ExpectingException(L l, const string& message=string()) // L for lambda
	{
		string m;
		if (!IsEmpty(message))
			m = string(" - ") + message;

		auto AddMoreText = [](const string& m)
		{
			return 
#ifdef _CPPRTTI 
			  string("; was expecting exception of type '")
			+ string(typeid(E).name())
			+ string("'")
#else
			  string(" (RTTI is turned off (/GR-) otherwise the expected exception name would be displayed here)")
#endif
			 + m;
		};

		try { l(); }
		catch(const E&) { return; }
		catch(...) { m_utils.ThrowAssertException("exception of wrong type thrown" + AddMoreText(m)); }
		m_utils.ThrowAssertException("no exception thrown" + AddMoreText(m));
 	}

	template <typename T> Fluent::That<T, string> That(const T& actual) { return Fluent::That<T, string>(actual, m_utils); }

	// wide (etc.) versions
	template <typename S, typename T, typename W> void AreEqual   (const S& expected, const T& actual, const W& message) { AreEqual   (expected, actual, ToString<string>(message)); }
	template <typename S, typename T, typename W> void AreNotEqual(const S& expected, const T& actual, const W& message) { AreNotEqual(expected, actual, ToString<string>(message)); }
	template <            typename T, typename W> void IsTrue     (                   const T& actual, const W& message) { IsTrue     (          actual, ToString<string>(message)); }
	template <            typename T, typename W> void IsFalse    (                   const T& actual, const W& message) { IsFalse    (          actual, ToString<string>(message)); }
	template <                        typename W> void Fail       (                                    const W& message) { Fail       (                  ToString<string>(message)); }
	template <typename E, typename L, typename W> void ExpectingException(L l,                         const W& message) { ExpectingException<E>(l,      ToString<string>(message)); }
};

}

#ifdef TDD_DEFINED_SAL_MACROS
 #undef _In_
#endif

#endif