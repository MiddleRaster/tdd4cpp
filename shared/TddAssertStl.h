#ifndef TDDASSERTSTL_H
#define TDDASSERTSTL_H

#include <string>
#include <sstream>
#include <iomanip>

#include "tddAssertBase.h"

namespace TDD
{
	template <> inline bool         IsEmpty(const std::string& s) { return s.empty(); }
	template <> inline const char* ToAsciiz(const std::string& s) { return s.c_str(); }

	namespace Details
	{
		static std::string FromSignedInt(int i)
		{
			std::stringstream s;
			s << i;
			return s.str();
		}
		static std::string FromUnsignedInt(unsigned int i)
		{
			std::stringstream s;
			s << i;
			return s.str();
		}
		static std::string FromSignedInt64(long long i)
		{
			std::stringstream s;
			s << i;
			return s.str();
		}
        static std::string FromUnsignedInt64(unsigned long long i)
        {
            std::stringstream s;
            s << i;
            return s.str();
        }
		static std::string FromDouble(double d)
		{
			std::stringstream s;
			s << std::setprecision(10) << d;
			return s.str();
		}
		static std::string FromWide(const wchar_t* w)
		{
			std::wstring t(w);
			std::string s(t.length(), 0);
			std::locale loc; // use default, as "en_US.UTF-8" doesn't work on Windows and "english" doesn't work on iOS.
			std::use_facet<std::ctype<wchar_t>>(loc).narrow(w, w + t.length(), '\0', &s[0]);
			return s;
		}
		static std::string FromWide(const std::wstring& w) { return FromWide(w.c_str()); }
		static std::string FromPointer(const void* p)
		{
			std::stringstream s;
			s << p;
			return s.str();
		}
	}

	// Implement specializations similar to these for your user-defined types

	template <> inline std::string ToString<std::string, bool>              (const bool& t)              { return Details::FromSignedInt(t); }
	template <> inline std::string ToString<std::string, int>               (const int& t)               { return Details::FromSignedInt(t); }
	template <> inline std::string ToString<std::string, long>              (const long& t)              { return Details::FromSignedInt(t); }
	template <> inline std::string ToString<std::string, long long>         (const long long& t)         { return Details::FromSignedInt64(t); }
	template <> inline std::string ToString<std::string, short>             (const short& t)             { return Details::FromSignedInt(t); }
	template <> inline std::string ToString<std::string, char>              (const char& t)              { return Details::FromSignedInt(t); }
	template <> inline std::string ToString<std::string, signed char>       (const signed char& t)       { return Details::FromSignedInt(t); }
	template <> inline std::string ToString<std::string, unsigned int>      (const unsigned int& t)      { return Details::FromUnsignedInt(t); }
	template <> inline std::string ToString<std::string, unsigned long>     (const unsigned long& t)     { return Details::FromUnsignedInt(t); }
	template <> inline std::string ToString<std::string, unsigned long long>(const unsigned long long& t){ return Details::FromUnsignedInt64(t); }
	template <> inline std::string ToString<std::string, float>             (const float & t)            { return Details::FromDouble(t); }
	template <> inline std::string ToString<std::string, double>            (const double & t)           { return Details::FromDouble(t); }
	template <> inline std::string ToString<std::string, unsigned short>    (const unsigned short & t)   { return Details::FromUnsignedInt(t); }
	template <> inline std::string ToString<std::string, unsigned char>     (const unsigned char & t)    { return Details::FromUnsignedInt(t); }
	template <> inline std::string ToString<std::string, std::string>       (const std::string & t)      { return t; }
	template <> inline std::string ToString<std::string, std::wstring>      (const std::wstring& t)      { return Details::FromWide(t); }
	template <> inline std::string ToString<std::string, void>              (const void   * t)           { return Details::FromPointer(t); }

	template <> inline std::string ToString<std::string,  char  > (const  char  * t) { return std::string(t); }
	template <> inline std::string ToString<std::string, wchar_t> (const wchar_t* t) { return Details::FromWide(t); }
}

#define TddAssert(...) TDD::AssertT<std::string>(__LINE__, __FILE__)

#endif
