#ifndef INPUT_TEST_H
#define INPUT_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

class Input;

class InputTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(InputTest);
	
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST(test2);
	
	CPPUNIT_TEST(dynamicTest1);
	CPPUNIT_TEST(dynamicTest2);
	CPPUNIT_TEST(dynamicTest3);
	CPPUNIT_TEST(dynamicTest4);
	CPPUNIT_TEST(dynamicTest5);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void test1();
		void test2();
		
		void dynamicTest1();
		void dynamicTest2();
		void dynamicTest3();
		void dynamicTest4();
		void dynamicTest5();
		
	private:
		void doDynamicTest(unsigned int size);
		void doTest(Input *input, const std::string & inputBuf);
};

#endif
