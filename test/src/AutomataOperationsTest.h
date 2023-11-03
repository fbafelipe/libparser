#ifndef AUTOMATA_OPERATIONS_TEST_H
#define AUTOMATA_OPERATIONS_TEST_H

#include "parser/DynamicAutomata.h"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class AutomataOperationsTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(AutomataOperationsTest);
	
	CPPUNIT_TEST(unionTest1);
	CPPUNIT_TEST(unionTest2);
	CPPUNIT_TEST(unionTest3);
	CPPUNIT_TEST(unionTest4);
	CPPUNIT_TEST(unionTest5);
	CPPUNIT_TEST(unionTest6);
	CPPUNIT_TEST(unionTest7);
	CPPUNIT_TEST(unionTest8);
	
	CPPUNIT_TEST(concatenationTest1);
	CPPUNIT_TEST(concatenationTest2);
	CPPUNIT_TEST(concatenationTest3);
	CPPUNIT_TEST(concatenationTest4);
	
	CPPUNIT_TEST(closureTest1);
	CPPUNIT_TEST(closureTest2);
	CPPUNIT_TEST(closureTest3);
	
	CPPUNIT_TEST(multOperationTest1);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void unionTest1();
		void unionTest2();
		void unionTest3();
		void unionTest4();
		void unionTest5();
		void unionTest6();
		void unionTest7();
		void unionTest8();
		
		void concatenationTest1();
		void concatenationTest2();
		void concatenationTest3();
		void concatenationTest4();
		
		void closureTest1();
		void closureTest2();
		void closureTest3();
		
		void multOperationTest1();
		
	private:
		typedef DynamicAutomata::StateToState StateToState;
		
		void checkUnionConnections(DynamicAutomata *automataA, StateToState & oldAToNew,
				DynamicAutomata *automataB, StateToState & oldBToNew,
				DynamicAutomata *result, bool finalStateIntersect);
};

#endif
