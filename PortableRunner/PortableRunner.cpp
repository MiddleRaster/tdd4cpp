#include <iostream>
#include "..\shared\tdd.h"

class PortableReporter : public TDD::Reporter
{
	unsigned int m_testsRun, m_failedTests;
	std::ostream& m_out;
public:
	PortableReporter(std::ostream& out = std::cout) : m_testsRun(0), m_failedTests(0), m_out(out) {}
	virtual ~PortableReporter()
	{
		if (m_testsRun == 0)
			m_out << "No tests were run!!!\n";
		else {
			m_out << m_failedTests << " failure";
			m_out << (m_failedTests == 1 ? "" : "s");
			m_out << " out of " << m_testsRun;
			m_out << (m_testsRun == 1 ? " test run\n" : " tests run\n");
		}
	}
	unsigned int TestsRun   () const { return m_testsRun; }
	unsigned int TestsFailed() const { return m_failedTests; }

public: // TDD::Reporter
	virtual void ForEachTest   (const TDD::UnitTestInfo&) { ++m_testsRun; }
	virtual void ForEachFailure(const TDD::TestFailure& tr)
	{
		++m_failedTests;

		m_out << "Failure in " << tr.group << "." << tr.testname << " -\n";
		m_out << tr.file_name << "(" << tr.line_number << ") : warning : Assertion failure : \"" << tr.error_string << "\"\n";
	}
};

int main()
{
	PortableReporter reporter;
	TDD::Discriminator discriminator;
	TDD::ClassRegistrarBase::RunTests(discriminator, reporter);
	return 0; // for VS integration, return value must be 0, or else it thinks the post-build step failed.
}
