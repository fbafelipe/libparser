#include "ParserErrorTest.h"


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

CPPUNIT_TEST_SUITE_REGISTRATION(ParserErrorTest);

void ParserErrorTest::setUp() {}

void ParserErrorTest::tearDown() {}

void ParserErrorTest::ll1Test1() {
	// parser1.bnf is not LL1
}

void ParserErrorTest::ll1Test1WithRecovery() {
	// parser1.bnf is not LL1
}

void ParserErrorTest::ll1Test2() {
	Grammar *grammar;
	Parser *parser = loadParser(LL1, &grammar, "scanner2.bnf", "parser2.bnf", "input_parser_error2.txt");
	
	testParser2(parser, grammar);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	delete(parser);
}

void ParserErrorTest::ll1Test2WithRecovery() {
	Grammar *grammar;
	Parser *parser = loadParser(LL1, &grammar, "scanner2.bnf", "parser2.bnf", "input_parser_error2.txt");
	
	testParser2WithRecovery(parser, grammar);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	delete(parser);
}

void ParserErrorTest::slr1Test1() {
	Grammar *grammar;
	Parser *parser = loadParser(SLR1, &grammar, "scanner1.bnf", "parser1.bnf", "input_parser_error1.txt");
	
	testParser1(parser, grammar);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	delete(parser);
}

void ParserErrorTest::slr1Test1WithRecovery() {
	Grammar *grammar;
	Parser *parser = loadParser(SLR1, &grammar, "scanner1.bnf", "parser1.bnf", "input_parser_error1.txt");
	
	testParser1WithRecovery(parser, grammar);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	delete(parser);
}

void ParserErrorTest::slr1Test2() {
	Grammar *grammar;
	Parser *parser = loadParser(SLR1, &grammar, "scanner2.bnf", "parser2.bnf", "input_parser_error2.txt");
	
	testParser2(parser, grammar);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	delete(parser);
}

void ParserErrorTest::slr1Test2WithRecovery() {
	Grammar *grammar;
	Parser *parser = loadParser(SLR1, &grammar, "scanner2.bnf", "parser2.bnf", "input_parser_error2.txt");
	
	testParser2WithRecovery(parser, grammar);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	delete(parser);
}

Parser *ParserErrorTest::loadParser(ParserType type, Grammar **g, const char *scannerFile,
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
 * ParserErrorTest::testParser1
 *****************************************************************************/
void ParserErrorTest::testParser1(Parser *parser, Grammar *grammar) {
	try {
		parser->parse();
		
		// should have thrown a ParserError
		CPPUNIT_ASSERT(false);
	}
	catch (ParserError & error) {
		CPPUNIT_ASSERT(error.getLineNumber() == 2);
	}
}

void ParserErrorTest::testParser1WithRecovery(Parser *parser, Grammar *grammar) {
	// NOTE the current error recovery algorithm discards bad tokens
	// if the error recovery algorithm changes, this test must be rewritten
	
	Parser::ParserErrorList errorList;
	ParsingTree::Node *node = parser->parseWithErrorRecovery(errorList);
	
	CPPUNIT_ASSERT(node == NULL);
	
	CPPUNIT_ASSERT(errorList.size() == 2);
	CPPUNIT_ASSERT(errorList[0].getLineNumber() == 2);
	CPPUNIT_ASSERT(errorList[1].getLineNumber() == 3);
}

/*****************************************************************************
 * ParserErrorTest::testParser2
 *****************************************************************************/
void ParserErrorTest::testParser2(Parser *parser, Grammar *grammar) {
	try {
		parser->parse();
		
		// should have thrown a ParserError
		CPPUNIT_ASSERT(false);
	}
	catch (ParserError & error) {
		CPPUNIT_ASSERT(error.getLineNumber() == 3);
	}
}

void ParserErrorTest::testParser2WithRecovery(Parser *parser, Grammar *grammar) {
	// NOTE the current error recovery algorithm discards bad tokens
	// if the error recovery algorithm changes, this test must be rewritten
	
	Parser::ParserErrorList errorList;
	ParsingTree::Node *node = parser->parseWithErrorRecovery(errorList);
	
	CPPUNIT_ASSERT(node == NULL);
	
	CPPUNIT_ASSERT(errorList.size() == 3);
	CPPUNIT_ASSERT(errorList[0].getLineNumber() == 3);
	CPPUNIT_ASSERT(errorList[1].getLineNumber() == 5);
	CPPUNIT_ASSERT(errorList[2].getLineNumber() == 8);
}
