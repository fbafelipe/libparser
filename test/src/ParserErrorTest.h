#ifndef PARSER_ERROR_TEST_H
#define PARSER_ERROR_TEST_H

#include "parser/ParsingTree.h"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class Grammar;
class Parser;

class ParserErrorTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ParserErrorTest);
	
	CPPUNIT_TEST(ll1Test1);
	CPPUNIT_TEST(ll1Test1WithRecovery);
	CPPUNIT_TEST(ll1Test2);
	CPPUNIT_TEST(ll1Test2WithRecovery);
	
	CPPUNIT_TEST(slr1Test1);
	CPPUNIT_TEST(slr1Test1WithRecovery);
	CPPUNIT_TEST(slr1Test2);
	CPPUNIT_TEST(slr1Test2WithRecovery);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void ll1Test1();
		void ll1Test1WithRecovery();
		void ll1Test2();
		void ll1Test2WithRecovery();
		
		void slr1Test1();
		void slr1Test1WithRecovery();
		void slr1Test2();
		void slr1Test2WithRecovery();
		
	private:
		typedef ParsingTree::Node Node;
		typedef ParsingTree::NonTerminal NonTerminal;
		typedef ParsingTree::Token Token;
		
		enum ParserType {
			LL1,
			SLR1
		};
		
		Parser *loadParser(ParserType type, Grammar **g, const char *scannerFile,
				const char *parserFile, const char *inputFile,
				unsigned int conflicts = 0);
		
		void testParser1(Parser *parser, Grammar *grammar);
		void testParser1WithRecovery(Parser *parser, Grammar *grammar);
		
		void testParser2(Parser *parser, Grammar *grammar);
		void testParser2WithRecovery(Parser *parser, Grammar *grammar);
};


#endif
