#ifndef GRAMMAR_LOADER_LOADER_TEST_H
#define GRAMMAR_LOADER_LOADER_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class GrammarLoaderBufferLoader;
class GrammarLoaderProceduralLoader;

class GrammarLoaderLoaderTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(GrammarLoaderLoaderTest);
	CPPUNIT_TEST(scannerLoaderTest);
	CPPUNIT_TEST(parserLoaderTest);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void scannerLoaderTest();
		void parserLoaderTest();
		
	private:
		GrammarLoaderBufferLoader *bufferLoader;
		GrammarLoaderProceduralLoader *proceduralLoader;
};

#endif
