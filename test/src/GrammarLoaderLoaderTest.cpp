#include "GrammarLoaderLoaderTest.h"

#include "parser/GrammarLoaderBufferLoader.h"
#include "parser/GrammarLoaderProceduralLoader.h"
#include "parser/ScannerAutomata.h"

CPPUNIT_TEST_SUITE_REGISTRATION(GrammarLoaderLoaderTest);

void GrammarLoaderLoaderTest::setUp() {
	bufferLoader = new GrammarLoaderBufferLoader();
	proceduralLoader = new GrammarLoaderProceduralLoader();
}

void GrammarLoaderLoaderTest::tearDown() {
	delete(bufferLoader);
	delete(proceduralLoader);
}

void GrammarLoaderLoaderTest::scannerLoaderTest() {
	ScannerAutomata *bufAutomata = bufferLoader->getScannerScannerAutomata();
	ScannerAutomata *procAutomata = proceduralLoader->getScannerScannerAutomata();
	
	CPPUNIT_ASSERT(*bufAutomata == *procAutomata);
	
	CPPUNIT_ASSERT(bufferLoader->scannerCommentId == proceduralLoader->scannerCommentId);
	CPPUNIT_ASSERT(bufferLoader->scannerIgnoreId == proceduralLoader->scannerIgnoreId);
	CPPUNIT_ASSERT(bufferLoader->scannerReduceId == proceduralLoader->scannerReduceId);
	CPPUNIT_ASSERT(bufferLoader->scannerTokenIdId == proceduralLoader->scannerTokenIdId);
	CPPUNIT_ASSERT(bufferLoader->scannerRegexId == proceduralLoader->scannerRegexId);
	CPPUNIT_ASSERT(bufferLoader->scannerRuleEndId == proceduralLoader->scannerRuleEndId);
	CPPUNIT_ASSERT(bufferLoader->scannerWhiteSpaceId == proceduralLoader->scannerWhiteSpaceId);
}

void GrammarLoaderLoaderTest::parserLoaderTest() {
	ScannerAutomata *bufAutomata = bufferLoader->getParserScannerAutomata();
	ScannerAutomata *procAutomata = proceduralLoader->getParserScannerAutomata();
	
	CPPUNIT_ASSERT(*bufAutomata == *procAutomata);
	
	CPPUNIT_ASSERT(bufferLoader->parserCommentId == proceduralLoader->parserCommentId);
	CPPUNIT_ASSERT(bufferLoader->parserNonTerminalId == proceduralLoader->parserNonTerminalId);
	CPPUNIT_ASSERT(bufferLoader->parserReduceId == proceduralLoader->parserReduceId);
	CPPUNIT_ASSERT(bufferLoader->parserRuleEndId == proceduralLoader->parserRuleEndId);
	CPPUNIT_ASSERT(bufferLoader->parserOrId == proceduralLoader->parserOrId);
	CPPUNIT_ASSERT(bufferLoader->parserTokenId == proceduralLoader->parserTokenId);
	CPPUNIT_ASSERT(bufferLoader->parserWhitespaceId == proceduralLoader->parserWhitespaceId);
}
