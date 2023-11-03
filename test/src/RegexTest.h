#ifndef REGEX_TEST_H
#define REGEX_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class RegexTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(RegexTest);
	CPPUNIT_TEST(simpleTest);
	CPPUNIT_TEST(orTest);
	CPPUNIT_TEST(optionalTest);
	CPPUNIT_TEST(closureTest);
	
	CPPUNIT_TEST(subexpressionTest);
	
	CPPUNIT_TEST(emptyTest);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void simpleTest();
		void orTest();
		void optionalTest();
		void closureTest();
		
		void subexpressionTest();
		
		void emptyTest();
};

#endif
