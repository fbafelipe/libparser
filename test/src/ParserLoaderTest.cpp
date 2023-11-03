#include "ParserLoaderTest.h"

#include "TestDefs.h"

#include "parser/Grammar.h"
#include "parser/GrammarLoader.h"
#include "parser/ParserGrammar.h"
#include "parser/ParserLoader.h"
#include "parser/ParserTable.h"
#include "parser/ParserTableLL1.h"
#include "parser/ParserTableSLR1.h"
#include "parser/Regex.h"
#include "parser/ScannerAutomata.h"
#include "parser/ScannerGrammar.h"

#include <cstring>

CPPUNIT_TEST_SUITE_REGISTRATION(ParserLoaderTest);

static bool grammarEquals(Grammar *g1, Grammar *g2);

void ParserLoaderTest::setUp() {}

void ParserLoaderTest::tearDown() {}

void ParserLoaderTest::test1() {
	testGrammar(SLR1, GrammarLoader::loadGrammar(GRAMMAR_FOLDER + "scanner1.bnf",
			GRAMMAR_FOLDER + "parser1.bnf"));
}

void ParserLoaderTest::test2() {
	testGrammar(LL1, GrammarLoader::loadGrammar(GRAMMAR_FOLDER + "scanner2.bnf",
				GRAMMAR_FOLDER + "parser2.bnf"));
}

void ParserLoaderTest::testGrammar(ParserType type, Grammar *grammar) {
	ParserGrammar *parserGrammar = grammar->getParserGrammar();
	
	ScannerAutomata *automata = grammar->getScannerGrammar()->compile(grammar);
	ParserTable *table = NULL;
	
	if (parserGrammar) {
		parserGrammar->sanityCheck();
		
		switch (type) {
			case LL1:
				table = grammar->getParserGrammar()->compileLL1(grammar);
				break;
			case SLR1:
				table = grammar->getParserGrammar()->compileSLR1(grammar);
				break;
		}
	}
	
	testAutomata(automata);
	testAutomataBin(grammar, automata);
	if (table) {
		testTable(table);
		testTableBin(grammar, table);
		testBin(grammar, automata, table);
	}
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(automata);
	delete(table);
	delete(grammar);
}

void ParserLoaderTest::testAutomata(ScannerAutomata *automata) {
	unsigned int size;
	unsigned char *buf = ParserLoader::automataToBuffer(automata, &size);
	ScannerAutomata *bufAutomata = ParserLoader::bufferToAutomata(buf);
	
	unsigned int newSize;
	unsigned char *newBuf = ParserLoader::automataToBuffer(bufAutomata, &newSize);
	
	CPPUNIT_ASSERT(*automata == *bufAutomata);
	CPPUNIT_ASSERT(newSize == size);
	CPPUNIT_ASSERT(!memcmp(buf, newBuf, size * sizeof(unsigned char)));
	
	delete[](buf);
	delete[](newBuf);
	delete(bufAutomata);
}

void ParserLoaderTest::testAutomataBin(Grammar *grammar, ScannerAutomata *automata) {
	unsigned int size;
	unsigned char *buf = ParserLoader::saveBuffer(grammar, automata, NULL, &size);
	
	ScannerAutomata *bufAutomata;
	Grammar *bufGrammar;
	ParserLoader::loadBuffer(buf, &bufGrammar, &bufAutomata, NULL);
	
	unsigned int newSize;
	unsigned char *newBuf = ParserLoader::saveBuffer(bufGrammar, bufAutomata, NULL, &newSize);
	
	CPPUNIT_ASSERT(*automata == *bufAutomata);
	CPPUNIT_ASSERT(newSize == size);
	CPPUNIT_ASSERT(!memcmp(buf, newBuf, size * sizeof(unsigned char)));
	CPPUNIT_ASSERT(grammarEquals(grammar, bufGrammar));
	
	delete[](buf);
	delete[](newBuf);
	delete(bufGrammar);
	delete(bufAutomata);
}

void ParserLoaderTest::testTable(ParserTable *table) {
	unsigned int size;
	unsigned char *buf = ParserLoader::tableToBuffer(table, &size);
	ParserTable *bufTable = ParserLoader::bufferToTable(buf);
	
	unsigned int newSize;
	unsigned char *newBuf = ParserLoader::tableToBuffer(bufTable, &newSize);
	
	CPPUNIT_ASSERT(*table == *bufTable);
	CPPUNIT_ASSERT(newSize == size);
	CPPUNIT_ASSERT(!memcmp(buf, newBuf, size * sizeof(unsigned char)));
	
	delete[](buf);
	delete[](newBuf);
	delete(bufTable);
}

void ParserLoaderTest::testTableBin(Grammar *grammar, ParserTable *table) {
	unsigned int size;
	unsigned char *buf = ParserLoader::saveBuffer(grammar, NULL, table, &size);
	
	ParserTable *bufTable;
	Grammar *bufGrammar;
	ParserLoader::loadBuffer(buf, &bufGrammar, NULL, &bufTable);
	
	unsigned int newSize;
	unsigned char *newBuf = ParserLoader::saveBuffer(bufGrammar, NULL, bufTable, &newSize);
	
	CPPUNIT_ASSERT(*table == *bufTable);
	CPPUNIT_ASSERT(newSize == size);
	CPPUNIT_ASSERT(!memcmp(buf, newBuf, size * sizeof(unsigned char)));
	CPPUNIT_ASSERT(grammarEquals(grammar, bufGrammar));
	
	delete[](buf);
	delete[](newBuf);
	delete(bufGrammar);
	delete(bufTable);
}

void ParserLoaderTest::testBin(Grammar *grammar, ScannerAutomata *automata, ParserTable *table) {
	unsigned int size;
	unsigned char *buf = ParserLoader::saveBuffer(grammar, automata, table, &size);
	
	ScannerAutomata *bufAutomata;
	ParserTable *bufTable;
	Grammar *bufGrammar;
	ParserLoader::loadBuffer(buf, &bufGrammar, &bufAutomata, &bufTable);
	
	unsigned int newSize;
	unsigned char *newBuf = ParserLoader::saveBuffer(bufGrammar, bufAutomata, bufTable, &newSize);
	
	CPPUNIT_ASSERT(*automata == *bufAutomata);
	CPPUNIT_ASSERT(*table == *bufTable);
	CPPUNIT_ASSERT(newSize == size);
	CPPUNIT_ASSERT(!memcmp(buf, newBuf, size * sizeof(unsigned char)));
	CPPUNIT_ASSERT(grammarEquals(grammar, bufGrammar));
	
	delete[](buf);
	delete[](newBuf);
	delete(bufGrammar);
	delete(bufAutomata);
	delete(bufTable);
}

static bool grammarEquals(Grammar *g1, Grammar *g2) {
	return g1->getTokenIds() == g2->getTokenIds()
			&& g1->getNonTermonalIds() == g2->getNonTermonalIds();
}
