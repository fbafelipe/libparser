#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class ParserGrammar;

class FirstFollowTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FirstFollowTest);
	CPPUNIT_TEST(firstTest);
	CPPUNIT_TEST(followTest);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void firstTest();
		void followTest();
		
	private:
		ParserGrammar *grammar;
};

#endif
