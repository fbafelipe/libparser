#ifndef DYNAMIC_AUTOMATA_TEST_H
#define DYNAMIC_AUTOMATA_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class DynamicAutomataTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(DynamicAutomataTest);
	CPPUNIT_TEST(minimizationSimpleTest);
	CPPUNIT_TEST(minimizationDeadStatesTest);
	CPPUNIT_TEST(minimizationEmptyLanguageTest);
	CPPUNIT_TEST(conversionTest);
	CPPUNIT_TEST(conversionTest2);
	CPPUNIT_TEST(minimizeStateMappingTest);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void minimizationSimpleTest();
		void minimizationDeadStatesTest();
		void minimizationEmptyLanguageTest();
		
		void conversionTest();
		void conversionTest2();
		
		void minimizeStateMappingTest();
};

#endif
