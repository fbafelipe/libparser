#include "ScannerTest.h"

#include "TestDefs.h"

#include "parser/FileInput.h"
#include "parser/Grammar.h"
#include "parser/GrammarLoader.h"
#include "parser/ScannerGrammar.h"
#include "parser/Pointer.h"
#include "parser/Scanner.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ScannerTest);

void ScannerTest::setUp() {}

void ScannerTest::tearDown() {}

void ScannerTest::test1() {
	Grammar *grammar = new Grammar();
	GrammarLoader::loadScanner(grammar, GRAMMAR_FOLDER + "scanner1.bnf");
	ScannerGrammar *scannerGrammar = grammar->getScannerGrammar();
	Pointer<ScannerAutomata> automata = scannerGrammar->compile(grammar);
	
	TokenTypeID aId = grammar->getTokenId("A");
	TokenTypeID numId = grammar->getTokenId("NUM");
	TokenTypeID idId = grammar->getTokenId("ID");
	
	delete(scannerGrammar);
	delete(grammar);
	
	Input *input = new FileInput(INPUT_FOLDER + "input1.txt");
	Scanner scanner(automata, input);
	
	tokenAssert(scanner, aId, "a");
	tokenAssert(scanner, numId, "32");
	tokenAssert(scanner, idId, "aa");
	tokenAssert(scanner, idId, "a1");
	tokenAssert(scanner, numId, "1.1");
	
	CPPUNIT_ASSERT(!scanner.nextToken());
}

void ScannerTest::test2() {
	Grammar *grammar = new Grammar();
	GrammarLoader::loadScanner(grammar, GRAMMAR_FOLDER + "scanner_c_preproc.bnf");
	ScannerGrammar *scannerGrammar = grammar->getScannerGrammar();
	Pointer<ScannerAutomata> automata = scannerGrammar->compile(grammar);
	
	Input *input = new FileInput(INPUT_FOLDER + "input2.txt");
	Scanner scanner(automata, input);
	
	TokenTypeID idId = grammar->getTokenId("IDENTIFIER");
	TokenTypeID constantId = grammar->getTokenId("CONSTANT");
	TokenTypeID commaId = grammar->getTokenId("COMMA");
	TokenTypeID mulId = grammar->getTokenId("MUL");
	TokenTypeID pOpenId = grammar->getTokenId("P_OPEN");
	TokenTypeID pCloseId = grammar->getTokenId("P_CLOSE");
	TokenTypeID bOpenId = grammar->getTokenId("B_OPEN");
	TokenTypeID bCloseId = grammar->getTokenId("B_CLOSE");
	TokenTypeID beginId = grammar->getTokenId("BEGIN");
	TokenTypeID endId = grammar->getTokenId("END");
	
	TokenTypeID directiveEndId = grammar->getTokenId("DIRECTIVE_END");
	TokenTypeID instEndId = grammar->getTokenId("INST_END");
	
	// in the preprocessor, some keywords are recognized as identifier
	// (since they are not keywords in the preprocessor)
	tokenAssert(scanner, idId, "int");
	tokenAssert(scanner, idId, "main");
	tokenAssert(scanner, pOpenId, "(");
	tokenAssert(scanner, idId, "int");
	tokenAssert(scanner, idId, "argc");
	tokenAssert(scanner, commaId, ",");
	tokenAssert(scanner, idId, "char");
	tokenAssert(scanner, mulId, "*");
	tokenAssert(scanner, idId, "argv");
	tokenAssert(scanner, bOpenId, "[");
	tokenAssert(scanner, bCloseId, "]");
	tokenAssert(scanner, pCloseId, ")");
	tokenAssert(scanner, beginId, "{");
	tokenAssert(scanner, directiveEndId, "\n");
	tokenAssert(scanner, idId, "return");
	tokenAssert(scanner, constantId, "0");
	tokenAssert(scanner, instEndId, ";");
	tokenAssert(scanner, directiveEndId, "\n");
	tokenAssert(scanner, endId, "}");
	tokenAssert(scanner, directiveEndId, "\n");
	tokenAssert(scanner, directiveEndId, "\n");
	assert(!scanner.nextToken());
	
	delete(scannerGrammar);
	delete(grammar);
}

void ScannerTest::tokenAssert(Scanner & scanner, TokenTypeID id) {
	Token *token = scanner.nextToken();
	CPPUNIT_ASSERT(token);
	CPPUNIT_ASSERT(token->getTokenTypeId() == id);
	delete(token);
}

void ScannerTest::tokenAssert(Scanner & scanner, TokenTypeID id, const std::string & tok) {
	Token *token = scanner.nextToken();
	CPPUNIT_ASSERT(token);
	
	CPPUNIT_ASSERT(token->getTokenTypeId() == id);
	CPPUNIT_ASSERT(token->getToken() == tok);
	delete(token);
}
