#include <iostream>

#include "..\tdd.h"

#include "ConsoleColor.h"

class ResultReporter : public TDD::Reporter
{
    unsigned int m_testsRun, m_failedTests;
public:
    ResultReporter() : m_testsRun(0), m_failedTests(0) {}
   ~ResultReporter() override
    {
        using namespace TddConsole;
        ConsoleColor cc;
        cc.SetFontAndBackgroundColors(  (WORD)(m_failedTests == 0 ? ConsoleColor::LightGreen : ConsoleColor::LightRed),
                                        (WORD)ConsoleColor::Black);
        if (m_testsRun == 0)
        {
            cc.SetFontAndBackgroundColors(ConsoleColor::Yellow, ConsoleColor::Black);
            std::cout << "No tests were run!!!\n";
        }
        else
        {
            std::cout <<  m_failedTests << " failure";
            std::cout << (m_failedTests == 1 ? "" : "s");
            std::cout << " out of " << m_testsRun << " test";
            std::cout << (m_testsRun == 1 ? "" : "s");
        }
    }
    unsigned int TestsRun   () const { return m_testsRun; }
    unsigned int TestsFailed() const { return m_failedTests; }

public: // TDD::Reporter
    virtual void ForEachTest   (const TDD::UnitTestInfo&) { ++m_testsRun; }
    virtual void ForEachFailure(const TDD::TestFailure& tr)
    {
        ++m_failedTests;

        using namespace TddConsole;
        ConsoleColor cc;
        cc.SetFontAndBackgroundColors((WORD)ConsoleColor::LightRed, (WORD)ConsoleColor::Black);

        std::cout << "Failure in " << tr.group << "." << tr.testname << " - \n";
        std::cout << tr.file_name << "(" << tr.line_number << ") : ERROR : " << tr.error_string << "\n";
    }
private:
    ResultReporter& operator=(const ResultReporter&) = delete;
};



int main()
{
    ResultReporter rr;
    TDD::Discriminator discriminator;
    TDD::ClassRegistrarBase::RunTests(discriminator, rr);
    return (rr.TestsRun() == 0) || (rr.TestsFailed() != 0) ? -1 : 0;
}
