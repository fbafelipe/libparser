#ifndef SCANNER_TEST_H
#define SCANNER_TEST_H

#include "parser/ParsingTree.h"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class Scanner;

class ScannerTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ScannerTest);
	
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST(test2);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp();
		void tearDown();
		
		void test1();
		void test2();
		
	private:
		typedef ParsingTree::Token Token;
		
		void tokenAssert(Scanner & scanner, TokenTypeID id);
		void tokenAssert(Scanner & scanner, TokenTypeID id, const std::string & tok);
};

#endif
