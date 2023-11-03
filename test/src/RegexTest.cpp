#include "RegexTest.h"

#include "parser/Automata.h"
#include "parser/ParserDefs.h"
#include "parser/Regex.h"

#include <cctype>

CPPUNIT_TEST_SUITE_REGISTRATION(RegexTest);

void RegexTest::setUp() {}

void RegexTest::tearDown() {}

void RegexTest::simpleTest() {
	Regex regex1("a");
	Regex regex2("b");
	Regex regex3(".");
	Regex regex4("\\d");
	Regex regex5("\\w");
	Regex regex6("\\n");
	Regex regex7("\\t");
	Regex regex8("\\s");
	Regex regex9("\\W");
	
	CPPUNIT_ASSERT(regex1.matches("a"));
	CPPUNIT_ASSERT(!regex1.matches("b"));
	CPPUNIT_ASSERT(!regex1.matches("aa"));
	
	CPPUNIT_ASSERT(regex2.matches("b"));
	CPPUNIT_ASSERT(!regex2.matches("c"));
	
	CPPUNIT_ASSERT(regex3.matches("x"));
	CPPUNIT_ASSERT(regex3.matches("a"));
	CPPUNIT_ASSERT(regex3.matches("\n"));
	CPPUNIT_ASSERT(regex3.matches("2"));
	CPPUNIT_ASSERT(!regex3.matches("xx"));
	
	for (unsigned int i = 0; i < 10; ++i) {
		char buf[2];
		buf[0] = '0' + i;
		buf[1] = '\0';
		CPPUNIT_ASSERT(regex4.matches(buf));
	}
	CPPUNIT_ASSERT(!regex4.matches("a"));
	CPPUNIT_ASSERT(!regex4.matches("_"));
	CPPUNIT_ASSERT(!regex4.matches("."));
	CPPUNIT_ASSERT(!regex4.matches("00"));
	
	CPPUNIT_ASSERT(regex5.matches("z"));
	CPPUNIT_ASSERT(regex5.matches("_"));
	CPPUNIT_ASSERT(regex5.matches("A"));
	CPPUNIT_ASSERT(!regex5.matches("AA"));
	
	CPPUNIT_ASSERT(regex6.matches("\n"));
	CPPUNIT_ASSERT(regex7.matches("\t"));
	CPPUNIT_ASSERT(!regex7.matches("\t\t"));
	
	CPPUNIT_ASSERT(regex8.matches(" "));
	CPPUNIT_ASSERT(regex8.matches("\n"));
	CPPUNIT_ASSERT(regex8.matches("\t"));
	CPPUNIT_ASSERT(!regex8.matches("\t\t"));
	CPPUNIT_ASSERT(!regex8.matches("a"));
	CPPUNIT_ASSERT(!regex8.matches("0"));
	
	CPPUNIT_ASSERT(regex9.matches(" "));
	CPPUNIT_ASSERT(regex9.matches("\n"));
	CPPUNIT_ASSERT(regex9.matches("+"));
	CPPUNIT_ASSERT(!regex9.matches("a"));
	CPPUNIT_ASSERT(!regex9.matches("z"));
	CPPUNIT_ASSERT(!regex9.matches("_"));
}

void RegexTest::orTest() {
	Regex regex1("a|b");
	Regex regex2("\\d|\\w");
	Regex regex3("a(a|b)");
	
	char buf[2];
	buf[1] = '\0';
	
	for (unsigned int i = 1; i < ALPHABET_SIZE; ++i) {
		buf[0] = i;
		if (i == 'a' || i == 'b') CPPUNIT_ASSERT(regex1.matches(buf));
		else CPPUNIT_ASSERT(!regex1.matches(buf));
	}
	
	for (unsigned int i = 1; i < ALPHABET_SIZE; ++i) {
		buf[0] = i;
		if (isalnum(i) || i == '_') CPPUNIT_ASSERT(regex2.matches(buf));
		else CPPUNIT_ASSERT(!regex2.matches(buf));
	}
	
	CPPUNIT_ASSERT(regex3.matches("aa"));
	CPPUNIT_ASSERT(regex3.matches("ab"));
	CPPUNIT_ASSERT(!regex3.matches("a"));
	CPPUNIT_ASSERT(!regex3.matches(""));
}

void RegexTest::optionalTest() {
	Regex regex1("a?");
	Regex regex2("aa?");
	
	CPPUNIT_ASSERT(regex1.matches("a"));
	CPPUNIT_ASSERT(regex1.matches(""));
	CPPUNIT_ASSERT(!regex1.matches("aa"));
	CPPUNIT_ASSERT(!regex1.matches("b"));
	
	CPPUNIT_ASSERT(regex2.matches("a"));
	CPPUNIT_ASSERT(regex2.matches("aa"));
	CPPUNIT_ASSERT(!regex2.matches(""));
	CPPUNIT_ASSERT(!regex2.matches("b"));
	CPPUNIT_ASSERT(!regex2.matches("bb"));
	CPPUNIT_ASSERT(!regex2.matches("aab"));
	CPPUNIT_ASSERT(!regex2.matches("ab"));
}

void RegexTest::closureTest() {
	Regex regex1("a*");
	Regex regex2("a+");
	
	std::string sentence;
	for (unsigned int i = 0; i < 20; ++i) {
		CPPUNIT_ASSERT(regex1.matches(sentence));
		CPPUNIT_ASSERT(!regex1.matches(sentence + "b"));
		CPPUNIT_ASSERT(!regex1.matches(std::string("b") + sentence));
		sentence += "a";
	}
	
	sentence = "";
	CPPUNIT_ASSERT(!regex2.matches(sentence));
	for (unsigned int i = 0; i < 20; ++i) {
		sentence += "a";
		CPPUNIT_ASSERT(regex2.matches(sentence));
		CPPUNIT_ASSERT(!regex2.matches(sentence + "b"));
		CPPUNIT_ASSERT(!regex2.matches(std::string("b") + sentence));
	}
}

void RegexTest::subexpressionTest() {
	Regex regex("(b*ab*ab*)*b*|(a*ba*ba*)*a*ba*");
	
	CPPUNIT_ASSERT(regex.matches(""));
	CPPUNIT_ASSERT(regex.matches("aa"));
	CPPUNIT_ASSERT(regex.matches("aaaa"));
	CPPUNIT_ASSERT(regex.matches("aab"));
	CPPUNIT_ASSERT(regex.matches("baa"));
	CPPUNIT_ASSERT(regex.matches("aba"));
	CPPUNIT_ASSERT(regex.matches("abababa"));
	CPPUNIT_ASSERT(regex.matches("bbaa"));
	CPPUNIT_ASSERT(regex.matches("aabb"));
	
	CPPUNIT_ASSERT(regex.matches("ba"));
	CPPUNIT_ASSERT(regex.matches("babb"));
	CPPUNIT_ASSERT(regex.matches("babab"));
	CPPUNIT_ASSERT(regex.matches("bbb"));
	CPPUNIT_ASSERT(regex.matches("bbbbb"));
	
	CPPUNIT_ASSERT(!regex.matches("a"));
	CPPUNIT_ASSERT(!regex.matches("abb"));
	CPPUNIT_ASSERT(!regex.matches("aaabb"));
	CPPUNIT_ASSERT(!regex.matches("ababa"));
	
	const Automata *automata = regex.getAutomata();
	CPPUNIT_ASSERT(automata->getNumStates() == 4);
	
	Automata *expected = new Automata(4);
	// state 0: 'a' even and 'b' even
	// state 1: 'a' odd and 'b' even
	// state 2: 'a' even and 'b' odd
	// state 3: 'a' odd and 'b' odd
	expected->setTransition(0, 'a', 1);
	expected->setTransition(0, 'b', 2);
	expected->setTransition(1, 'a', 0);
	expected->setTransition(1, 'b', 3);
	expected->setTransition(2, 'a', 3);
	expected->setTransition(2, 'b', 0);
	expected->setTransition(3, 'a', 2);
	expected->setTransition(3, 'b', 1);
	
	expected->setFinalState(0);
	expected->setFinalState(2);
	expected->setFinalState(3);
	
	CPPUNIT_ASSERT(*automata == *expected);
	CPPUNIT_ASSERT(*expected == *automata);
	
	delete(expected);
}

void RegexTest::emptyTest() {
	Regex regex("");
	
	CPPUNIT_ASSERT(regex.matches(""));
	CPPUNIT_ASSERT(!regex.matches("a"));
}
