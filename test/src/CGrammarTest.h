#ifndef C_GRAMMAR_TEST_H
#define C_GRAMMAR_TEST_H

#include "ParserLoaderTest.h"

#include "parser/ParserTable.h"
#include "parser/ParsingTree.h"
#include "parser/Pointer.h"
#include "parser/ScannerAutomata.h"
#include "parser/TokenType.h"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

class Grammar;
class Scanner;

class CGrammarTest : public ParserLoaderTest {
	CPPUNIT_TEST_SUITE(CGrammarTest);
	
	CPPUNIT_TEST(scannerLoaderTest);
	CPPUNIT_TEST(parserLoaderTest);
	
	CPPUNIT_TEST(scannerTest);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void scannerLoaderTest();
		void parserLoaderTest();
		
		void scannerTest();
		
	private:
		typedef ParsingTree::Token Token;
		
		void testTokens(Scanner & scanner, unsigned int numToks,
				TokenTypeID *ids, const std::string *toks);
		
		void tokenAssert(Scanner & scanner, TokenTypeID id);
		void tokenAssert(Scanner & scanner, TokenTypeID id, const std::string & tok);
		
		Grammar *grammar;
		Pointer<ScannerAutomata> scannerAutomata;
		Pointer<ParserTable> parserTable;
};

#endif
