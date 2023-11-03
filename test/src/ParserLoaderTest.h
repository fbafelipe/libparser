#ifndef PARSER_LOADER_TEST_H
#define PARSER_LOADER_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class Grammar;
class ScannerAutomata;
class ParserTable;

class ParserLoaderTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ParserLoaderTest);
	
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST(test2);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void test1();
		void test2();
		
	protected:
		enum ParserType {
			LL1,
			SLR1
		};
		
		void testGrammar(ParserType type, Grammar *grammar);
		void testAutomata(ScannerAutomata *automata);
		void testAutomataBin(Grammar *grammar, ScannerAutomata *automata);
		void testTable(ParserTable *table);
		void testTableBin(Grammar *grammar, ParserTable *table);
		void testBin(Grammar *grammar, ScannerAutomata *automata, ParserTable *table);
};

#endif
