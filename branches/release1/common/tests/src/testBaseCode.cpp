#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

#include "BaseCode.h"
#include "ICD9.h"
#include "ICD10.h"
#include "ICDExceptions.h"
#include <string.h>

class BaseCodeTester : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(BaseCodeTester);
	CPPUNIT_TEST(testCreateCodeFromBufferICD9);
	CPPUNIT_TEST(testCreateCodeFromBufferICD10);
	CPPUNIT_TEST(testCreateCodeFromBufferBad);
	CPPUNIT_TEST_SUITE_END();
public:
	void testCreateCodeFromBufferICD9()
	{
		void* buf = new char[ICD9_BUF_LEN];
		char* ptr = (char*)buf;

		int type = codeToInt(ICD9_t);
		const char* str = "abs6";

		memcpy(ptr, &type, sizeof(int));
		ptr += sizeof(int);

		memcpy(ptr, str, ICD9_STR_LEN);

		BaseCode* code = BaseCode::createCodeFromBuffer(buf);

		CPPUNIT_ASSERT(code->getType() == ICD9_t);
		CPPUNIT_ASSERT(code->getCode() == "abs6");
		
		delete code;
		delete (char*)buf;
	}

	void testCreateCodeFromBufferICD10()
	{
		void* buf = new char[ICD10_BUF_LEN];
		char* ptr = (char*)buf;

		int type = codeToInt(ICD10_t);
		const char* str = "abs6352";

		memcpy(ptr, &type, sizeof(int));
		ptr += sizeof(int);

		memcpy(ptr, str, ICD10_STR_LEN);

		BaseCode* code = BaseCode::createCodeFromBuffer(buf);

		CPPUNIT_ASSERT(code->getType() == ICD10_t);
		CPPUNIT_ASSERT(code->getCode() == "abs6352");
		
		delete code;
		delete (char*)buf;
	}

	void testCreateCodeFromBufferBad()
	{
		CPPUNIT_ASSERT_THROW(BaseCode::createCodeFromBuffer(NULL), NullPointerException);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(BaseCodeTester);

int main(void)
{
	CppUnit::Test* suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
	CppUnit::TextTestRunner runner;
	runner.addTest(suite);
	return runner.run();
}
