#include "ParserTest.h"

#include "TestDefs.h"

#include "parser/FileInput.h"
#include "parser/Grammar.h"
#include "parser/GrammarLoader.h"
#include "parser/Parser.h"
#include "parser/ParserCompileResult.h"
#include "parser/ParserGrammar.h"
#include "parser/ParserTable.h"
#include "parser/ParserTableLL1.h"
#include "parser/ParserTableSLR1.h"
#include "parser/ParsingTree.h"
#include "parser/Scanner.h"
#include "parser/ScannerAutomata.h"
#include "parser/ScannerGrammar.h"

#include <cstdlib>

CPPUNIT_TEST_SUITE_REGISTRATION(ParserTest);

void ParserTest::setUp() {}

void ParserTest::tearDown() {}

void ParserTest::ll1Test1() {
	// parser1.bnf is not LL1
}

void ParserTest::ll1Test2() {
	Grammar *grammar;
	Parser *parser = loadParser(LL1, &grammar, "scanner2.bnf", "parser2.bnf", "input_parser2.txt");
	
	testParser2(parser, grammar);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	delete(parser);
}

void ParserTest::slr1Test1() {
	Grammar *grammar;
	Parser *parser = loadParser(SLR1, &grammar, "scanner1.bnf", "parser1.bnf", "input_parser1.txt");
	
	testParser1(parser, grammar);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	delete(parser);
}

void ParserTest::slr1Test2() {
	Grammar *grammar;
	Parser *parser = loadParser(SLR1, &grammar, "scanner2.bnf", "parser2.bnf", "input_parser2.txt");
	
	testParser2(parser, grammar);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	delete(parser);
}

Parser *ParserTest::loadParser(ParserType type, Grammar **g, const char *scannerFile,
		const char *parserFile, const char *inputFile, unsigned int conflicts) {
	
	Grammar *grammar = GrammarLoader::loadGrammar(
			GRAMMAR_FOLDER + scannerFile,
			GRAMMAR_FOLDER + parserFile);
	
	*g = grammar;
	
	ScannerAutomata *scannerAutomata = grammar->getScannerGrammar()->compile(grammar);
	
	ParserTable *table = NULL;
	ParserCompileResult compileResult;
	
	switch (type) {
		case LL1:
			table = grammar->getParserGrammar()->compileLL1(grammar, &compileResult);
			break;
		case SLR1:
			table = grammar->getParserGrammar()->compileSLR1(grammar, &compileResult);
			break;
		default:
			abort();
	}
	
	CPPUNIT_ASSERT(compileResult.getConflictsCount() == conflicts);
	
	Scanner *scanner = new Scanner(scannerAutomata,
			new FileInput(INPUT_FOLDER + inputFile));
	
	return new Parser(table, scanner);
}

/*****************************************************************************
 * ParserTest::testParser1
 *****************************************************************************/
void ParserTest::testParser1(Parser *parser, Grammar *grammar) {
	ParsingTree::Node *node = parser->parse();
	
	CPPUNIT_ASSERT(node->getNodeType() == ParsingTree::NODE_NON_TERMINAL);
	
	test1StartDepth1(grammar, (NonTerminal *)node);
	
	delete(node);
}

void ParserTest::test1StartDepth1(Grammar *grammar, NonTerminal *nonTerminal) {
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("START"));
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 0);
	
	Token *tok = nonTerminal->getTokenAt(0);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("A"));
	CPPUNIT_ASSERT(tok->getToken() == "a");
	
	tok = nonTerminal->getTokenAt(2);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("A"));
	CPPUNIT_ASSERT(tok->getToken() == "a");
	
	test1StartDepth2(grammar, nonTerminal->getNonTerminalAt(1));
}

void ParserTest::test1StartDepth2(Grammar *grammar, NonTerminal *nonTerminal) {
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("START"));
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 1);
	
	Token *tok = nonTerminal->getTokenAt(0);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("A"));
	CPPUNIT_ASSERT(tok->getToken() == "a");
	
	tok = nonTerminal->getTokenAt(2);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("A"));
	CPPUNIT_ASSERT(tok->getToken() == "a");
	
	test1FooDepth1(grammar, nonTerminal->getNonTerminalAt(1));
}

void ParserTest::test1FooDepth1(Grammar *grammar, NonTerminal *nonTerminal) {
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("FOO"));
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 0);
	
	Token *tok = nonTerminal->getTokenAt(0);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("NUM"));
	CPPUNIT_ASSERT(tok->getToken() == "1");
	
	tok = nonTerminal->getTokenAt(2);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("NUM"));
	CPPUNIT_ASSERT(tok->getToken() == "2");
	
	test1FooDepth2(grammar, nonTerminal->getNonTerminalAt(1));
}

void ParserTest::test1FooDepth2(Grammar *grammar, NonTerminal *nonTerminal) {
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("FOO"));
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 1);
	
	Token *tok = nonTerminal->getTokenAt(0);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("NUM"));
	CPPUNIT_ASSERT(tok->getToken() == "3");
	
	tok = nonTerminal->getTokenAt(2);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("NUM"));
	CPPUNIT_ASSERT(tok->getToken() == "4");
	
	test1Bar(grammar, nonTerminal->getNonTerminalAt(1), "dcb");
}

void ParserTest::test1Bar(Grammar *grammar, NonTerminal *nonTerminal, const char *toks) {
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("BAR"));
	
	if (!toks[1]) {
		CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 1);
		
		Token *tok = nonTerminal->getTokenAt(0);
		CPPUNIT_ASSERT(tok->getToken().size() == 1);
		CPPUNIT_ASSERT(tok->getToken()[0] == *toks);
	}
	else {
		Token *tok = nonTerminal->getTokenAt(1);
		CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("ID"));
		CPPUNIT_ASSERT(tok->getToken().size() == 1);
		CPPUNIT_ASSERT(tok->getToken()[0] == *toks);
		
		test1Bar(grammar, nonTerminal->getNonTerminalAt(0), toks + 1);
	}
}

/*****************************************************************************
 * ParserTest::testParser2
 *****************************************************************************/
void ParserTest::testParser2(Parser *parser, Grammar *grammar) {
	ParsingTree::Node *node = parser->parse();
	
	CPPUNIT_ASSERT(node->getNodeType() == ParsingTree::NODE_NON_TERMINAL);
	
	NonTerminal *nonTerminal = (NonTerminal *)node;
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("PARSER"));
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 0);
	test2Rule1(grammar, nonTerminal->getNonTerminalAt(0));
	
	nonTerminal = nonTerminal->getNonTerminalAt(1);
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("PARSER"));
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 0);
	test2Rule2(grammar, nonTerminal->getNonTerminalAt(0));
	
	nonTerminal = nonTerminal->getNonTerminalAt(1);
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("PARSER"));
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 1);
	CPPUNIT_ASSERT(nonTerminal->getNodeList().empty());
	
	delete(node);
}

void ParserTest::test2Rule1(Grammar *grammar, NonTerminal *nonTerminal) {
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("RULE"));
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 0);
	
	Token *tok = nonTerminal->getTokenAt(0);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("NON_TERMINAL"));
	CPPUNIT_ASSERT(tok->getToken() == "<START>");
	
	tok = nonTerminal->getTokenAt(1);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("REDUCE"));
	CPPUNIT_ASSERT(tok->getToken() == "::=");
}

void ParserTest::test2Rule2(Grammar *grammar, NonTerminal *nonTerminal) {
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalId() == grammar->getNonTerminalId("RULE"));
	CPPUNIT_ASSERT(nonTerminal->getNonTerminalRule() == 0);
	
	Token *tok = nonTerminal->getTokenAt(0);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("NON_TERMINAL"));
	CPPUNIT_ASSERT(tok->getToken() == "<FOO>");
	
	tok = nonTerminal->getTokenAt(1);
	CPPUNIT_ASSERT((int)tok->getTokenTypeId() == grammar->getTokenId("REDUCE"));
	CPPUNIT_ASSERT(tok->getToken() == "::=");
}
