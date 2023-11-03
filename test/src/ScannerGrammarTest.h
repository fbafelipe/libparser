#ifndef SCANNER_GRAMMAR_TEST_H
#define SCANNER_GRAMMAR_TEST_H

#include "parser/ScannerGrammar.h"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class ScannerGrammarTest : public CppUnit::TestFixture, private ScannerGrammar {
	CPPUNIT_TEST_SUITE(ScannerGrammarTest);
	
	CPPUNIT_TEST(tokenMapTest1);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void tokenMapTest1();
		
	private:
		int findToken(DynamicAutomata *automata, const char *input, const StateToToken & tokenMap);
};

#endif
