#ifndef PARSER_TEST_H
#define PARSER_TEST_H

#include "parser/ParsingTree.h"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class Grammar;
class Parser;

class ParserTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ParserTest);
	
	CPPUNIT_TEST(ll1Test1);
	CPPUNIT_TEST(ll1Test2);
	
	CPPUNIT_TEST(slr1Test1);
	CPPUNIT_TEST(slr1Test2);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void ll1Test1();
		void ll1Test2();
		
		void slr1Test1();
		void slr1Test2();
		
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
		void test1StartDepth1(Grammar *grammar, NonTerminal *nonTerminal);
		void test1StartDepth2(Grammar *grammar, NonTerminal *nonTerminal);
		void test1FooDepth1(Grammar *grammar, NonTerminal *nonTerminal);
		void test1FooDepth2(Grammar *grammar, NonTerminal *nonTerminal);
		void test1Bar(Grammar *grammar, NonTerminal *nonTerminal, const char *toks);
		
		void testParser2(Parser *parser, Grammar *grammar);
		void test2Rule1(Grammar *grammar, NonTerminal *nonTerminal);
		void test2Rule2(Grammar *grammar, NonTerminal *nonTerminal);
};

#endif
