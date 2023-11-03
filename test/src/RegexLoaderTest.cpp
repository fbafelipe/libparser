#include "RegexLoaderTest.h"

#include "parser/RegexBufferGrammar.h"
#include "parser/RegexProceduralGrammar.h"
#include "parser/ScannerAutomata.h"

CPPUNIT_TEST_SUITE_REGISTRATION(RegexLoaderTest);

void RegexLoaderTest::setUp() {
	bufferLoader = new RegexBufferGrammar();
	proceduralLoader = new RegexProceduralGrammar();
}

void RegexLoaderTest::tearDown() {
	delete(bufferLoader);
	delete(proceduralLoader);
}

void RegexLoaderTest::regexScannerLoaderTest() {
	ScannerAutomata *bufAutomata = bufferLoader->getScannerAutomata();
	ScannerAutomata *procAutomata = proceduralLoader->getScannerAutomata();
	
	CPPUNIT_ASSERT(*bufAutomata == *procAutomata);
	
	CPPUNIT_ASSERT(bufferLoader->tokenClosureId == proceduralLoader->tokenClosureId);
	CPPUNIT_ASSERT(bufferLoader->tokenOneClosureId == proceduralLoader->tokenOneClosureId);
	CPPUNIT_ASSERT(bufferLoader->tokenOptionalId == proceduralLoader->tokenOptionalId);
	CPPUNIT_ASSERT(bufferLoader->tokenComposedStartId == proceduralLoader->tokenComposedStartId);
	CPPUNIT_ASSERT(bufferLoader->tokenComposedEndId == proceduralLoader->tokenComposedEndId);
	CPPUNIT_ASSERT(bufferLoader->tokenOrId == proceduralLoader->tokenOrId);
	CPPUNIT_ASSERT(bufferLoader->tokenSymbolListStartId == proceduralLoader->tokenSymbolListStartId);
	CPPUNIT_ASSERT(bufferLoader->tokenSymbolListEndId == proceduralLoader->tokenSymbolListEndId);
	CPPUNIT_ASSERT(bufferLoader->tokenNotId == proceduralLoader->tokenNotId);
	CPPUNIT_ASSERT(bufferLoader->tokenSymbolId == proceduralLoader->tokenSymbolId);
}

void RegexLoaderTest::regexParserLoaderTest() {
	ParserTable *bufTable = bufferLoader->getParserTable();
	ParserTable *procTable = proceduralLoader->getParserTable();
	
	CPPUNIT_ASSERT(*bufTable == *procTable);
	
	CPPUNIT_ASSERT(bufferLoader->regexId == proceduralLoader->regexId);
	CPPUNIT_ASSERT(bufferLoader->regexOrId == proceduralLoader->regexOrId);
	CPPUNIT_ASSERT(bufferLoader->expressionId == proceduralLoader->expressionId);
	CPPUNIT_ASSERT(bufferLoader->factorId == proceduralLoader->factorId);
	CPPUNIT_ASSERT(bufferLoader->symbolSetId == proceduralLoader->symbolSetId);
	CPPUNIT_ASSERT(bufferLoader->symbolListId == proceduralLoader->symbolListId);
}
