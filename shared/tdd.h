#ifndef TDD_H
#define TDD_H

/*
what we want:
TEST_CLASS(SomeClass) // or TESTCLASS(SomeClass)
{
    SomeClass()
    {
        // This class will be (re-)constructed for _each_ test method.
    }
    ~SomeClass()
    {
        // The dtor will be called after _each_ test method.
        // It's (usually) bad practice to throw from a dtor, 
        // so if you want to validate something during cleanup, you can do so in the TEST_METHOD_CLEANUP method instead
    }

    TEST_CLASS_INITIALIZE(ClassSetup) // static, called once per class, before all tests (but _before_ ctor)
    {
        // Initialize any data-member here that takes a long time to initialize.
        // This method will be called only once, before any tests are run.

        // N.B.:  make your slow data static and initialize those statics here.
        // This matches MSTest/xUnit.net/NUnit's behavior.
    }
    TEST_CLASS_CLEANUP(ClassTeardown) // static, called once per class, after all tests, after last dtor
    {
        // clean up static data here
    }

    TEST_METHOD_INITIALIZE(TestSetup) // before each test
    {
        // initialize datamembers here (or in ctor)
        // may also validate something, e.g.:
        TDD_VERIFY(<expression>);
        // or, preferably using the Assert class
        TddAssert().AreEqual(expected, actual);
    }
    TEST_METHOD_CLEANUP(TestTeardown) // after each test
    {
        // cleanup datamembers here (or in dtor)
        // may also validate something, e.g.:
        TDD_VERIFY(<expression>);
        // or, preferably using the Assert class
        TddAssert().AreEqual(expected, actual);
    }

    TESTMETHOD(SomeTest)
    {
        TDD_VERIFY(<expression>);
        // or, preferably using the Assert class
        TddAssert().AreEqual(expected, actual);
    }
    // or
    TEST_METHOD(AnotherTest)
    {
        TDD_VERIFY(<expression>);
        // or, preferably using the Assert class
        TddAssert().AreEqual(expected, actual);
    }
};
*/

// since sal.h is a Microsoft-specific header, specify that we don't want SAL
#define TDD_NO_SAL_H
#ifndef _Inout_count_
 #ifdef TDD_NO_SAL_H
  #define TDD_DEFINED_SAL_MACROS
  #define _Inout_count_(x)
  #define _In_z_
  #define _In_
 #else
  #error please #include this file after sal.h or specstrings.h or #define TDD_NO_SAL_H
 #endif
#endif

namespace TDD
{

struct UnitTestInfo // a container to hold some info about each test
{
    UnitTestInfo(_In_z_ const char* g, _In_z_ const char* t) : group(g), testname(t) {}
    UnitTestInfo(_In_ const UnitTestInfo* uti) : group(uti->group), testname(uti->testname) {}
    const char *group, *testname;
};

struct TestFailure : public UnitTestInfo // a container to hold some info about a test failure
{
    TestFailure(_In_ const UnitTestInfo* uti, unsigned long line, _In_z_ const char* name, _In_z_ const char* error) 
        : UnitTestInfo(uti)
        , file_name(name)
        , error_string(error) 
        , line_number(line)
    {}
    const char *file_name, *error_string;
    unsigned long line_number;
};

struct Reporter
{
    virtual void ForEachTest   (const UnitTestInfo&) {}  // called once for each test
    virtual void ForEachFailure(const TestFailure&) = 0;  // called once for each failure
    virtual ~Reporter(){}
};
struct Discriminator
{
    virtual bool WantTest(const UnitTestInfo&) { return true; } // return true if you want to run this test
    virtual ~Discriminator(){}
};

#ifdef _CPPUNWIND

class TddException
{
    unsigned long _line;
    const char * _file;
    const char * _message;

public:
    TddException(unsigned long line, _In_z_ const char * file) 
        : _line(line)
        , _file(file)
        , _message("internal test exception") 
    {}

    TddException(_In_z_ const char* message, unsigned long line, _In_z_ const char * file) 
        : _line(line)
        , _file(file)
        , _message(message) 
    {}
    virtual ~TddException() {}

    unsigned long GetLine() const { return _line; }
    const char *  GetFile() const { return _file; }
    virtual const char * GetExceptionText() const { return _message; }

protected:
    void SetMessage(_In_z_ const char* message) { _message = message; }
    void SetFile   (_In_z_ const char* file)    { _file = file; }
};

#define THROW_TDD_EXCEPTION                  throw TDD::TddException(         __LINE__, __FILE__)
#define THROW_TDD_EXCEPTION_MESSAGE(message) throw TDD::TddException(message, __LINE__, __FILE__)

#endif // #ifdef _CPPUNWIND

struct TestClassBase
{
    virtual ~TestClassBase     () {}
    virtual void TestInitialize() {} // before each test
    virtual void TestCleanup   () {} // after each test
};

class Verifier
{
    static Reporter*& GetReporter()
    {
        static Reporter * s_reporter = 0;
        return s_reporter;
    }
    static UnitTestInfo*& GetUnitTestInfo()
    {
        static UnitTestInfo * s_uti = 0;
        return s_uti;
    }
public:
    static void SetVerifierInfo(Reporter& reporter, UnitTestInfo& uti)
    {
        GetReporter() = &reporter;
        GetUnitTestInfo() = &uti;
    }
    static void Verify(unsigned long line, _In_z_ const char * filename, bool b, _In_z_ const char * errorString)
    {
        if (false == b)
        {
        // if exception handling semantics are defined and _TDD_NO_RETURN_ON_ASSERT_FAILURE isn't defined
        // then we throw exceptions to avoid continuing processing in the current execution context
        // if either exception handling semantics are undefined or _TDD_NO_RETURN_ON_ASSERT_FAILURE is defined
        // then we just call the reporter with the failure and do not throw any exceptions
        #if defined(_CPPUNWIND) && !defined(_TDD_NO_RETURN_ON_ASSERT_FAILURE)
            throw TDD::TddException(errorString, line, filename);
        #else
            GetReporter()->ForEachFailure (TestFailure (GetUnitTestInfo(), line, filename, errorString));
        #endif
        }        
    }
};

typedef void (*pfnModuleInitializeAndCleanup)();
class ClassRegistrarBase
{
    static void NullInitializer() {}
    ClassRegistrarBase* m_pNext;    // pointer to next test class entry
public:
    ClassRegistrarBase() : m_pNext(0)
    {
        AddClass(this); 
    }
    virtual ~ClassRegistrarBase(){}

    static pfnModuleInitializeAndCleanup& GetModuleInitialize()
    {
        static pfnModuleInitializeAndCleanup s_initialize = NullInitializer;
        return s_initialize;
    }
    static pfnModuleInitializeAndCleanup& GetModuleCleanup()
    {
        static pfnModuleInitializeAndCleanup s_cleanup = NullInitializer;
        return s_cleanup;
    }

protected:
    static bool& GetModuleInitializeFunctionWasCalled()
    {
        static bool s_bModuleInitializeFunctionWasCalled = false;
        return s_bModuleInitializeFunctionWasCalled;
    }
    static bool& GetModuleInitializationFailed()
    {
        static bool s_bModuleInitializationFailed = false;
        return s_bModuleInitializationFailed;
    }
public:
    static void RunTests(_In_ Discriminator& d, _In_ Reporter& r)
    {
        ClassRegistrarBase* p = GetTestTable();
        while (p) {
            p->RunClassTests(d, r);
            p = p->m_pNext;
        }

        if (true == GetModuleInitializeFunctionWasCalled())
            TryCatchAndReport(r, "<Global>", [](){ GetModuleCleanup()(); }, "TestModuleCleanup", "unknown exception from TestModuleCleanup");
    }

protected:
    template <typename L> static bool TryCatchAndReport(Reporter& r, const char* className, L l, _In_z_ const char * testname, _In_z_ const char * message) // L for lambda
    {
        (void)message; // not used if _CPPUNWIND is not defined.
        UnitTestInfo uti(className, testname);
        Verifier::SetVerifierInfo(r, uti);
    #ifdef _CPPUNWIND
        try {
    #endif
            l();
    #ifdef _CPPUNWIND
        } catch (TddException& e) {
            r.ForEachFailure (TestFailure (&uti, e.GetLine(), e.GetFile(), e.GetExceptionText()));
            return true;
        } catch (...) {
            r.ForEachFailure (TestFailure (&uti, __LINE__, __FILE__, message));
            return true;
        }
    #endif
        return false;
    }

private:
    virtual void RunClassTests(_In_ Discriminator& d, _In_ Reporter& r) = 0;

    static ClassRegistrarBase*& GetTestTable()
    {
        static ClassRegistrarBase * s_table = 0;
        return s_table;
    }
    static void AddClass(_In_ ClassRegistrarBase* t)
    {
        ClassRegistrarBase * p = GetTestTable();
        if (!p)   // empty: add to beginning
            GetTestTable() = t;
        else {
            while (p->m_pNext) p = p->m_pNext;
            p->m_pNext = t;   // add to end
        }
    }
};
typedef ClassRegistrarBase UnitTestBase; // for backwards-compatibility

// some template meta-programming to determine if static TestClassInitialize/TestClassCleanup methods exist
struct HasMethodMachinery
{
    // won't compile (SFINAE) if second template parameter (which will be the function or method) doesn't match the first template parameter
    template <typename T, T> struct PrototypeChecker;
    typedef char Yes;
    typedef long No;
};
template<bool b, class Type = void> struct enable_if             {}; // "type" undefined => will not compile => SFINAE
template<        class Type       > struct enable_if<true, Type> { typedef Type type; };

#define TDD_MAKE_OPTIONAL_METHOD(MyType,method) \
    template <class Type> class TypeHas##method : TDD::HasMethodMachinery \
    { \
        template <typename TT> static Yes HasMethod(PrototypeChecker<void(*)(), &TT::method >*); \
        template <typename TT> static No  HasMethod(...); \
    public: \
        enum { value = (sizeof(HasMethod<Type>(0)) == sizeof(Yes)) }; \
    }; \
    template <typename TT> static typename TDD::enable_if< (TypeHas##method<TT>::value), void>::type Do##method() { TT::method(); } \
    template <typename TT> static typename TDD::enable_if<!(TypeHas##method<TT>::value), void>::type Do##method() { } \
    static void Call##method() { Do##method<T>(); }

#define   TDD_MAKE_10_OPTIONAL_METHODS(MyType,method)      TDD_MAKE_OPTIONAL_METHOD(MyType,method##0);      TDD_MAKE_OPTIONAL_METHOD(MyType,method##1);      TDD_MAKE_OPTIONAL_METHOD(MyType,method##2);      TDD_MAKE_OPTIONAL_METHOD(MyType,method##3);      TDD_MAKE_OPTIONAL_METHOD(MyType,method##4);      TDD_MAKE_OPTIONAL_METHOD(MyType,method##5);      TDD_MAKE_OPTIONAL_METHOD(MyType,method##6);      TDD_MAKE_OPTIONAL_METHOD(MyType,method##7);      TDD_MAKE_OPTIONAL_METHOD(MyType,method##8);      TDD_MAKE_OPTIONAL_METHOD(MyType,method##9);
#define  TDD_MAKE_100_OPTIONAL_METHODS(MyType,method)  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##0);  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##1);  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##2);  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##3);  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##4);  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##5);  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##6);  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##7);  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##8);  TDD_MAKE_10_OPTIONAL_METHODS(MyType,method##9);
#define TDD_MAKE_1000_OPTIONAL_METHODS(MyType,method) TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##0); TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##1); TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##2); TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##3); TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##4); TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##5); TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##6); TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##7); TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##8); TDD_MAKE_100_OPTIONAL_METHODS(MyType,method##9);

#define TDD_CALL_OPTIONAL_METHOD(MyType,method,number) Call##method##number()
#define   TDD_CALL_10_OPTIONAL_METHODS(MyType,method)      TDD_CALL_OPTIONAL_METHOD(MyType,method, 0);      TDD_CALL_OPTIONAL_METHOD(MyType,method, 1);      TDD_CALL_OPTIONAL_METHOD(MyType,method, 2);      TDD_CALL_OPTIONAL_METHOD(MyType,method, 3);      TDD_CALL_OPTIONAL_METHOD(MyType,method, 4);      TDD_CALL_OPTIONAL_METHOD(MyType,method, 5);      TDD_CALL_OPTIONAL_METHOD(MyType,method ,6);      TDD_CALL_OPTIONAL_METHOD(MyType,method, 7);      TDD_CALL_OPTIONAL_METHOD(MyType,method, 8);      TDD_CALL_OPTIONAL_METHOD(MyType,method, 9);
#define  TDD_CALL_100_OPTIONAL_METHODS(MyType,method)  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##0);  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##1);  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##2);  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##3);  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##4);  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##5);  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##6);  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##7);  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##8);  TDD_CALL_10_OPTIONAL_METHODS(MyType,method##9);
#define TDD_CALL_1000_OPTIONAL_METHODS(MyType,method) TDD_CALL_100_OPTIONAL_METHODS(MyType,method##0); TDD_CALL_100_OPTIONAL_METHODS(MyType,method##1); TDD_CALL_100_OPTIONAL_METHODS(MyType,method##2); TDD_CALL_100_OPTIONAL_METHODS(MyType,method##3); TDD_CALL_100_OPTIONAL_METHODS(MyType,method##4); TDD_CALL_100_OPTIONAL_METHODS(MyType,method##5); TDD_CALL_100_OPTIONAL_METHODS(MyType,method##6); TDD_CALL_100_OPTIONAL_METHODS(MyType,method##7); TDD_CALL_100_OPTIONAL_METHODS(MyType,method##8); TDD_CALL_100_OPTIONAL_METHODS(MyType,method##9);

template<typename C> class TddAutoPtr
{
    C* m_p;
public:
    explicit TddAutoPtr(C* p) : m_p(p) {}
    ~TddAutoPtr() { delete m_p; }
};

template<typename T> class ClassRegistrar : public ClassRegistrarBase
{
    Reporter* m_r;
    // all test methods go through this function so that all exceptions/failures are reported.
    template <typename L> bool TryCatchAndReport(L l, _In_z_ const char * testname, _In_z_ const char * message) { return ClassRegistrarBase::TryCatchAndReport(*m_r, ClassName(), l, testname, message); }
public:
    struct TestMethodInfo : public UnitTestInfo
    {
        void (T::*m_pfn)();
        TestMethodInfo * m_pNext;
        TestMethodInfo(const char* g, const char* t, void (T::*pfn)()) : UnitTestInfo(g, t), m_pfn(pfn), m_pNext(0) {}
        virtual ~TestMethodInfo() {}
    };
    class MethodRegistrar
    {
        static const char*& ClassName()
        {
            static const char* s_g = "not set yet";
            return s_g;
        }
        static TestMethodInfo*& TestMethodTable()
        {
            static TestMethodInfo* s_testMethodInfo = 0;
            return s_testMethodInfo;
        }
    public:
        MethodRegistrar(const char* g) { ClassName() = g; }
        ~MethodRegistrar() { DestroyTestMethodTable(GetTestMethodTable()); }

        static TestMethodInfo* GetTestMethodTable() { return TestMethodTable(); }
        static void AddTestMethod(const char* t, void (T::*pfn)())
        {
            TestMethodInfo* p = TestMethodTable();
            if (!p)
                TestMethodTable() = new TestMethodInfo(ClassName(), t, pfn);
            else {
                while (p->m_pNext)
                    p = p->m_pNext;
                p->m_pNext = new TestMethodInfo(ClassName(), t, pfn);
            }
        }
        static void DisconnectTestTable() { TestMethodTable() = 0; } // caller will destroy table
        static void DestroyTestMethodTable(TestMethodInfo* p)
        {
             while (p) {
                 TestMethodInfo* next = p->m_pNext;
                 delete p;
                 p = next;
             }
             TestMethodTable() = 0;
        }
    };

public:
    ClassRegistrar(_In_z_ const char * classname)
    {
        ClassName() = classname;
    }
    static const char *& ClassName()
    {
        static const char * s_classname = "no class name set!";
        return s_classname;
    }

private:
    TDD_MAKE_OPTIONAL_METHOD(T,TestClassInitialize);
    TDD_MAKE_OPTIONAL_METHOD(T,TestClassCleanup);

      TDD_MAKE_10_OPTIONAL_METHODS(T,s_TDD__AddTest__);
     TDD_MAKE_100_OPTIONAL_METHODS(T,s_TDD__AddTest__);
//  TDD_MAKE_1000_OPTIONAL_METHODS(T,s_TDD__AddTest__); // uncomment this line iff your test class has more than 100 test methods.  Compilation will be slow :(

    virtual void RunClassTests(_In_ Discriminator& d, _In_ Reporter& r)
    {
        m_r = &r; // hang onto this so I don't have to keep passing it to TryCatchAndReport

        bool bClassInitializeFunctionWasCalled = false;
        bool bInitializationFailed             = false; // if static ctor or TestClassInitialize failed

        TestMethodInfo* pTestTable = 0;

    #ifdef _CPPUNWIND
        try
    #endif
        {
            MethodRegistrar mrb(ClassName());

            // create test method table
             TDD_CALL_10_OPTIONAL_METHODS(T,s_TDD__AddTest__);
            TDD_CALL_100_OPTIONAL_METHODS(T,s_TDD__AddTest__);
        // TDD_CALL_1000_OPTIONAL_METHODS(T,s_TDD__AddTest__); // uncomment this line iff your test class has more than 100 test methods.  Compilation will be slow :(  A better idea would be to break your test class into multiple smaller ones.
             
            TestMethodInfo* pCurrentTest = mrb.GetTestMethodTable();
            if (!pCurrentTest)
                return; // no tests to run

            pTestTable = pCurrentTest;    // I'm dong this because a few of my unit tests call this method recursively.  
            mrb.DisconnectTestTable();    // Since MethodRegistrar holds a static, that doesn't work.
                                          // So I'm placing the table in a local variable and will destroy it myself after the "try/catch" block

            do {
                if (d.WantTest(*pCurrentTest)) {
                    r.ForEachTest(*pCurrentTest);

                    if (GetModuleInitializationFailed() == true) { // module initialization failed; report that every test can't run
                        r.ForEachFailure(TestFailure(pCurrentTest, __LINE__, __FILE__, "test module initialization failure: can't run test!"));
                        continue;
                    }
                    if (bInitializationFailed == true) { // class initialization failed; report that every test can't run
                        r.ForEachFailure(TestFailure(pCurrentTest, __LINE__, __FILE__, "test class initialization failure: can't run test!"));
                        continue;
                    }

                    // initialize module only once
                    if (GetModuleInitializeFunctionWasCalled() == false) {
                        GetModuleInitializeFunctionWasCalled() = true;
                        GetModuleInitializationFailed() |= TryCatchAndReport([](){ GetModuleInitialize()(); }, "TestModuleInitialize", "unknown exception from TestModuleInitialize");
                        if (GetModuleInitializationFailed() == true)
                            continue; // already reported failure; can't proceed
                    }

                    // initialize test class only once
                    if (bClassInitializeFunctionWasCalled == false) {
                        bClassInitializeFunctionWasCalled = true;
                        bInitializationFailed |= TryCatchAndReport([](){ CallTestClassInitialize(); }, "TestClassInitialize", "unknown exception from TestClassInitialize");
                        if (bInitializationFailed == true)
                            continue; // already reported failure; can't proceed
                    }

                    // create a new instance of the test class for each test that the user wants to run
                    T* pTestClass = 0;
                    bInitializationFailed |= TryCatchAndReport([&pTestClass]() { pTestClass = new T(); }, "constructor", "unknown exception:  continuing anyway");
                    if (bInitializationFailed == true)
                        continue; // already reported failure; can't proceed

                    T& testClass = *pTestClass;
                    TddAutoPtr<T> tap(pTestClass);

                    // TestInitialize
                    if (false == TryCatchAndReport([&testClass](){ static_cast<TestClassBase&>(testClass).TestInitialize(); }, "TestInitialize", "unknown exception from TestInitialize"))
                    {   // all init'ed, run the test
                        TryCatchAndReport([&testClass, pCurrentTest]() { (testClass.*(pCurrentTest->m_pfn))(); }, pCurrentTest->testname, "unknown exception:  continuing anyway");
                    }

                    // TestCleanup (no matter what)
                    TryCatchAndReport([&testClass](){ static_cast<TestClassBase&>(testClass).TestCleanup(); }, "TestCleanup", "unknown exception from TestCleanup");
                }
            } while (0 != (pCurrentTest = pCurrentTest->m_pNext)); // next test!
        
    #ifdef _CPPUNWIND
        } catch (...) {
            // inexplicable exception:  I tried very hard to wrap any possibly throwing calls inside TryCatchAndReport, but something must have gotten through
            UnitTestInfo uti(ClassName(), "inexplicable exception");
            r.ForEachFailure (TestFailure(&uti, __LINE__, __FILE__, "An unexpected exception was thrown"));
    #endif
        }

        if (true == bClassInitializeFunctionWasCalled)
            TryCatchAndReport([](){ CallTestClassCleanup(); }, "TestClassCleanup", "unknown exception from TestClassCleanup");

        if (pTestTable)
            MethodRegistrar::DestroyTestMethodTable(pTestTable);
    }
};

struct NamespaceResolver
{
    const char* name;

    // precisely strcpy_s - written here so that tdd.h doesn't include any other headers
    static void Copy(_Inout_count_(count) char* destination, int count, const char* source)
    {
        while(count-- > 0)
        {
            *destination = *source++;
            if (!*destination++)
                break;
        }
    }

    // basically, strcmp() - written here so that tdd.h doesn't include any other headers
    static const char* TrimClassName(_Inout_count_(count) char* namespaces, int count, const char* className)
    {
        for(int i=0; i<count; ++i)
        {
            bool same = true;
            for(int j=0; i+j<count && className[j]; ++j)
            {
                if (namespaces[i+j] != className[j])
                {
                    same = false;
                    break;
                }
            }
            if (same == true)
            {
                namespaces[i] = 0;
                break;
            }
        }
        return namespaces;
    }
};
template<typename T> struct TheClassTypedefer { typedef T TheClass; };

struct TMI { TMI(void(*pfn)()) { TDD::ClassRegistrarBase::GetModuleInitialize() = pfn; } };
struct TMC { TMC(void(*pfn)()) { TDD::ClassRegistrarBase::GetModuleCleanup()    = pfn; } };

} // namespace TDD


#ifdef __PRETTY_FUNCTION__ // allow for differences on Mac and gcc
    #define TDD__FUNCTION__ __PRETTY_FUNCTION__
#else
    #define TDD__FUNCTION__ __FUNCTION__
#endif

#define TESTCLASS(classname) \
    struct classname##_TddNamespaceResolver : public TDD::NamespaceResolver { classname##_TddNamespaceResolver() { name = TDD__FUNCTION__; } \
    static const char* GetNameSpace() { static char s_sig[sizeof(TDD__FUNCTION__)+20] = {0}; Copy(s_sig, sizeof(s_sig), classname##_TddNamespaceResolver().name);  return TrimClassName(s_sig, sizeof(s_sig), "_TddNamespaceResolver"); } }; \
    class classname; TDD::TddAutoPtr<TDD::ClassRegistrar<classname> > g_##classname##_variable(new TDD::ClassRegistrar<classname>(classname##_TddNamespaceResolver::GetNameSpace())); \
    class classname : public TDD::TestClassBase, private TDD::TheClassTypedefer<classname>

#define __TDD_CONCAT2__(x,y) x##y
#define __TDD_CONCAT1__(x,y) __TDD_CONCAT2__(x,y)
#define __TDD_ADDTEST__ __TDD_CONCAT1__(public: static void s_TDD__AddTest__,__COUNTER__) // if your compiler doesn't suppoert __COUNTER__, try __LINE__. It'll work, but you'll probably have to turn on TDD_MAKE_1000_OPTIONAL_METHODS/TDD_CALL_1000_OPTIONAL_METHODS, which will be slow.

#define TESTMETHOD(methodname) __TDD_ADDTEST__() { ::TDD::ClassRegistrar<TheClass>::MethodRegistrar::AddTestMethod(#methodname, &TheClass::methodname##_test_method); } virtual void methodname##_test_method() // virtual to avoid PREfast warning 25007

#define TEST_CLASS(className)              TESTCLASS(className)
#define TEST_METHOD(methodName)            TESTMETHOD(methodName)
#define TEST_MODULE_INITIALIZE(n)          void TestModuleInitialize(); ::TDD::TMI __TDD__tmi__(TestModuleInitialize); void TestModuleInitialize()
#define TEST_MODULE_CLEANUP(n)             void TestModuleCleanup   (); ::TDD::TMC __TDD__tmc__(TestModuleCleanup);    void TestModuleCleanup()
#define TEST_CLASS_INITIALIZE(ignoreName)  public: static  void TestClassInitialize()
#define TEST_CLASS_CLEANUP(ignoreName)     public: static  void TestClassCleanup()
#define TEST_METHOD_INITIALIZE(ignoreName) public: virtual void TestInitialize()
#define TEST_METHOD_CLEANUP(ignoreName)    public: virtual void TestCleanup()

// in case you want to disable slow tests:  no registration mechanism => no tests
#define SKIP_TEST_CLASS(classname) class classname : public TDD::TestClassBase, private TDD::TheClassTypedefer<classname>
#define SKIP_TEST_METHOD(a) void a(void)

// VS CppUnitTest.h compatibility:  not implemented, but provided so that VS tests will build on other platforms.
#define BEGIN_TEST_CLASS_ATTRIBUTE()
#define       TEST_CLASS_ATTRIBUTE(n,v)
#define   END_TEST_CLASS_ATTRIBUTE()
#define BEGIN_TEST_METHOD_ATTRIBUTE(m)
#define       TEST_METHOD_ATTRIBUTE(n,v)
#define   END_TEST_METHOD_ATTRIBUTE()
#define BEGIN_TEST_MODULE_ATTRIBUTE()
#define       TEST_MODULE_ATTRIBUTE(n,v)
#define   END_TEST_MODULE_ATTRIBUTE()
#define TEST_OWNER(o)
#define TEST_DESCRIPTION(d)
#define TEST_PRIORITY(p)
#define TEST_WORKITEM(w)
#define TEST_IGNORE()

#ifndef TDD_DO_NOT_USE_VERIFY_MACROS // conditional compilation for __pragma which only exists in VS

// if _TDD_NO_RETURN_ON_ASSERT_FAILURE is defined then we don't return inside the TDD_VERIFY macros
// if _CPPUNWIND is defined and _TDD_NO_RETURN_ON_ASSERT_FAILURE is not defined then TDD_VERIFY macros
// throw TDDExceptions instead of returning, however if both _TDD_NO_RETURN_ON_ASSERT_FAILURE
// and _CPPUNWIND are defined then no exceptions are thrown inside the TDD_VERIFY macros
#if defined(_TDD_NO_RETURN_ON_ASSERT_FAILURE) || defined(_CPPUNWIND)
    #define TDD_VERIFY_RETURN_ON_FAILURE
    #define TDD_VERIFY_RETURN
#else
    #define TDD_VERIFY_RETURN_ON_FAILURE {if (false == __tdd_b) return;}
    #define TDD_VERIFY_RETURN return
#endif

#define TDD_VERIFY(arg)                  do { bool __tdd_b = (arg); \
                                             TDD::Verifier::Verify(__LINE__, __FILE__, __tdd_b, "TDD_VERIFY("#arg")"); \
                                             TDD_VERIFY_RETURN_ON_FAILURE; __pragma(warning(push)) __pragma(warning(disable:4127)) \
                                         } while(false) __pragma(warning(pop))
#define TDD_VERIFY_EQUAL(arg1, arg2)     do { bool __tdd_b = ((arg1) == (arg2)); \
                                             TDD::Verifier::Verify(__LINE__, __FILE__, __tdd_b, "TDD_VERIFY_EQUAL("#arg1", "#arg2")"); \
                                             TDD_VERIFY_RETURN_ON_FAILURE; __pragma(warning(push)) __pragma(warning(disable:4127)) \
                                         } while(false) __pragma(warning(pop))
#define TDD_VERIFY_NOT_EQUAL(arg1, arg2) do { bool __tdd_b = ((arg1) != (arg2)); \
                                             TDD::Verifier::Verify(__LINE__, __FILE__, __tdd_b, "TDD_VERIFY_NOT_EQUAL("#arg1", "#arg2")"); \
                                             TDD_VERIFY_RETURN_ON_FAILURE; __pragma(warning(push)) __pragma(warning(disable:4127)) \
                                         } while(false) __pragma(warning(pop))
#define TDD_VERIFY_HRESULT(arg)          do { long __tdd_hr = (arg); \
                                              bool __tdd_b = __tdd_hr >= 0; \
                                             TDD::Verifier::Verify(__LINE__, __FILE__, __tdd_b, "TDD_VERIFY_HRESULT("#arg")"); \
                                             TDD_VERIFY_RETURN_ON_FAILURE; __pragma(warning(push)) __pragma(warning(disable:4127)) \
                                         } while(false) __pragma(warning(pop))
#define TDD_FAILURE(errorString)         do { TDD::Verifier::Verify(__LINE__, __FILE__, false, errorString); TDD_VERIFY_RETURN; } __pragma(warning(push)) __pragma(warning(disable:4127)) \
                                         while(false) __pragma(warning(pop))
#define TDD_EXCEPTION(errorString)       do { TDD::Verifier::Verify(__LINE__, __FILE__, false, errorString); TDD_VERIFY_RETURN; } __pragma(warning(push)) __pragma(warning(disable:4127)) \
                                         while(false) __pragma(warning(pop))
#endif


#ifdef TDD_DEFINED_SAL_MACROS
 #undef _In_
 #undef _In_z_
 #undef _Inout_count_
#endif

#endif