#include "CGrammarTest.h"

#include "AutomataTestUtility.h"
#include "TestDefs.h"

#include "parser/FileInput.h"
#include "parser/Grammar.h"
#include "parser/GrammarLoader.h"
#include "parser/ParserGrammar.h"
#include "parser/Scanner.h"
#include "parser/ScannerGrammar.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CGrammarTest);

void CGrammarTest::setUp() {
	grammar = GrammarLoader::loadGrammar(GRAMMAR_FOLDER + "scanner_c.bnf",
			GRAMMAR_FOLDER + "parser_c.bnf");
	
	scannerAutomata = grammar->getScannerGrammar()->compile(grammar);
	parserTable = grammar->getParserGrammar()->compileSLR1(grammar);
}

void CGrammarTest::tearDown() {
	if (grammar) {
		delete (grammar->getScannerGrammar());
		delete (grammar->getParserGrammar());
		delete (grammar);
	}
	scannerAutomata = NULL;
	parserTable = NULL;
}

void CGrammarTest::scannerLoaderTest() {
	testAutomata(scannerAutomata);
}

void CGrammarTest::parserLoaderTest() {
	testTable(parserTable);
}

void CGrammarTest::scannerTest() {
	Scanner scanner(scannerAutomata, new FileInput(INPUT_FOLDER + "input_c1.txt"));
	
	TokenTypeID staticId = grammar->getOrCreateTokenId("STATIC");
	TokenTypeID pOpenId = grammar->getOrCreateTokenId("P_OPEN");
	TokenTypeID pCloseId = grammar->getOrCreateTokenId("P_CLOSE");
	TokenTypeID commaId = grammar->getOrCreateTokenId("COMMA");
	TokenTypeID voidId = grammar->getOrCreateTokenId("VOID");
	TokenTypeID charId = grammar->getOrCreateTokenId("CHAR");
	TokenTypeID intId = grammar->getOrCreateTokenId("INT");
	TokenTypeID bOpenId = grammar->getOrCreateTokenId("B_OPEN");
	TokenTypeID bCloseId = grammar->getOrCreateTokenId("B_CLOSE");
	TokenTypeID instEndId = grammar->getOrCreateTokenId("INST_END");
	TokenTypeID beginId = grammar->getOrCreateTokenId("BEGIN");
	TokenTypeID endId = grammar->getOrCreateTokenId("END");
	TokenTypeID constantId = grammar->getOrCreateTokenId("CONSTANT");
	TokenTypeID stringId = grammar->getOrCreateTokenId("STRING_LITERAL");
	TokenTypeID idId = grammar->getOrCreateTokenId("IDENTIFIER");
	TokenTypeID mulId = grammar->getOrCreateTokenId("MUL");
	TokenTypeID returnId = grammar->getOrCreateTokenId("RETURN");
	
	// STATIC VOID IDENTIFIER P_OPEN VOID P_CLOSE INST_END
	// static void foo(void);
	TokenTypeID idsLine1[7] = {staticId, voidId, idId, pOpenId, voidId, pCloseId, instEndId};
	std::string line1[7] = {"static", "void", "foo", "(", "void", ")", ";"};
	testTokens(scanner, 7, idsLine1, line1);
	
	// STATIC VOID IDENTIFIER P_OPEN VOID P_OPEN MUL IDENTIFIER P_CLOSE P_OPEN P_CLOSE P_CLOSE INST_END
	// static void bar(void (*f)());
	TokenTypeID idsLine2[13] = {staticId, voidId, idId, pOpenId, voidId, pOpenId,
			mulId, idId, pCloseId, pOpenId, pCloseId, pCloseId, instEndId};
	std::string line2[13] = {"static", "void", "bar", "(", "void", "(", "*", "f",
			")", "(", ")", ")", ";"};
	testTokens(scanner, 13, idsLine2, line2);
	
	
	// INT IDENTIFIER P_OPEN INT IDENTIFIDER COMMA CHAR MUL IDENTIFIER B_OPEN B_CLOSE P_CLOSE BEGIN
	// int main(int argc, char *argv[]) {
	TokenTypeID idsLine3[13] = {intId, idId, pOpenId, intId, idId, commaId, charId,
			mulId, idId, bOpenId, bCloseId, pCloseId, beginId};
	std::string line3[13] = {"int", "main", "(", "int", "argc", ",", "char", "*",
			"argv", "[", "]", ")", "{"};
	testTokens(scanner, 13, idsLine3, line3);
	
	// IDENTIFIER P_OPEN IDENTIFIER P_CLOSE INST_END
	// bar(foo);
	TokenTypeID idsLine4[5] = {idId, pOpenId, idId, pCloseId, instEndId};
	std::string line4[5] = {"bar", "(", "foo", ")", ";"};
	testTokens(scanner, 5, idsLine4, line4);
	
	// RETURN CONSTANT INST_END
	// return 0;
	TokenTypeID idsLine5[3] = {returnId, constantId, instEndId};
	std::string line5[3] = {"return", "0", ";"};
	testTokens(scanner, 3, idsLine5, line5);
	
	// END
	// }
	tokenAssert(scanner, endId, "}");
	
	// STATIC VOID IDENTIFIER P_OPEN P_CLOSE BEGIN
	// static void foo() {
	TokenTypeID idsLine7[6] = {staticId, voidId, idId, pOpenId, pCloseId, beginId};
	std::string line7[6] = {"static", "void", "foo", "(", ")", "{"};
	testTokens(scanner, 6, idsLine7, line7);
	
	// IDENTIFIER P_OPEN STRING_LITERAL P_CLOSE INST_END
	// printf("foo");
	TokenTypeID idsLine8[5] = {idId, pOpenId, stringId, pCloseId, instEndId};
	std::string line8[5] = {"printf", "(", "\"foo\"", ")", ";"};
	testTokens(scanner, 5, idsLine8, line8);
	
	// END
	// }
	tokenAssert(scanner, endId, "}");
	
	// STATIC VOID IDENTIFIER P_OPEN VOID P_OPEN MUL IDENTIFIER P_CLOSE P_OPEN P_CLOSE P_CLOSE BEGIN
	// static void bar(void (*f)()) {
	TokenTypeID idsLine10[13] = {staticId, voidId, idId, pOpenId, voidId, pOpenId,
			mulId, idId, pCloseId, pOpenId, pCloseId, pCloseId, beginId};
	std::string line10[13] = {"static", "void", "bar", "(", "void", "(", "*", "f",
			")", "(", ")", ")", "{"};
	testTokens(scanner, 13, idsLine10, line10);
	
	// IDENTIFIER P_OPEN P_CLOSE INST_END
	// f();
	TokenTypeID idsLine11[4] = {idId, pOpenId, pCloseId, instEndId};
	std::string line11[4] = {"f", "(", ")", ";"};
	testTokens(scanner, 4, idsLine11, line11);
	
	// END
	// }
	tokenAssert(scanner, endId, "}");
	
	CPPUNIT_ASSERT(scanner.nextToken() == NULL);
}

void CGrammarTest::testTokens(Scanner & scanner, unsigned int numToks,
		TokenTypeID *ids, const std::string *toks) {
	
	for (unsigned int i = 0; i < numToks; ++i) {
		tokenAssert(scanner, ids[i], toks[i]);
	}
}

void CGrammarTest::tokenAssert(Scanner & scanner, TokenTypeID id) {
	Token *token = scanner.nextToken();
	CPPUNIT_ASSERT(token);
	CPPUNIT_ASSERT(token->getTokenTypeId() == id);
	delete(token);
}

void CGrammarTest::tokenAssert(Scanner & scanner, TokenTypeID id, const std::string & tok) {
	Token *token = scanner.nextToken();
	CPPUNIT_ASSERT(token);
	
	CPPUNIT_ASSERT(token->getTokenTypeId() == id);
	CPPUNIT_ASSERT(token->getToken() == tok);
	delete(token);
}
