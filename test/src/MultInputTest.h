#ifndef MULT_INPUT_TEST_H
#define MULT_INPUT_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

class Input;

class MultInputTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(MultInputTest);
	
	CPPUNIT_TEST(listInputTest1);
	CPPUNIT_TEST(listInputTest2);
	
	CPPUNIT_TEST(offsetInputTest1);
	CPPUNIT_TEST(offsetInputTest2);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void listInputTest1();
		void listInputTest2();
		
		void offsetInputTest1();
		void offsetInputTest2();
		
	private:
		void doTest(Input *input, const std::string & inputBuf);
		void doOffsetTest(Input *input, int offset, const std::string & inputBuf);
};

#endif
