#ifndef REGEX_LOADER_TEST_H
#define REGEX_LOADER_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class RegexBufferGrammar;
class RegexProceduralGrammar;

class RegexLoaderTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(RegexLoaderTest);
	CPPUNIT_TEST(regexScannerLoaderTest);
	CPPUNIT_TEST(regexParserLoaderTest);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void regexScannerLoaderTest();
		void regexParserLoaderTest();
		
	private:
		RegexBufferGrammar *bufferLoader;
		RegexProceduralGrammar *proceduralLoader;
};

#endif
