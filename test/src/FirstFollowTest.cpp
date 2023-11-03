#include "FirstFollowTest.h"

#include "parser/ParserGrammar.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FirstFollowTest);

typedef ParserGrammar::NodeTypeList NodeTypeList;
typedef ParserGrammar::NodeType NodeType;
typedef ParserGrammar::Rule Rule;
typedef ParserGrammar::SymbolSet SymbolSet;
typedef ParserGrammar::NonTerminalFirst NonTerminalFirst;
typedef ParserGrammar::NonTerminalFollow NonTerminalFollow;

void FirstFollowTest::setUp() {
	grammar = new ParserGrammar();
	
	/*
	 * Grammar:
	 * S -> A a
	 * A -> S | B C
	 * B -> b | EPSILON
	 * C -> c | EPSILON
	 * 
	 * NonTerminalIDs:
	 * 0: S
	 * 1: A
	 * 2: B
	 * 3: C
	 * 
	 * TokenIDs:
	 * 0: a
	 * 1: b
	 * 2: c
	 */
	
	NodeTypeList nodeList;
	
	// S -> A a
	nodeList.push_back(NodeType(ParserGrammar::NON_TERMINAL, 1));
	nodeList.push_back(NodeType(ParserGrammar::TOKEN, 0));
	grammar->addRule(new Rule(0, nodeList));
	nodeList.clear();
	
	// A -> S
	nodeList.push_back(NodeType(ParserGrammar::NON_TERMINAL, 0));
	grammar->addRule(new Rule(1, nodeList));
	nodeList.clear();
	
	// A -> B C
	nodeList.push_back(NodeType(ParserGrammar::NON_TERMINAL, 2));
	nodeList.push_back(NodeType(ParserGrammar::NON_TERMINAL, 3));
	grammar->addRule(new Rule(1, nodeList));
	nodeList.clear();
	
	// B -> b
	nodeList.push_back(NodeType(ParserGrammar::TOKEN, 1));
	grammar->addRule(new Rule(2, nodeList));
	nodeList.clear();
	
	// B -> EPSILON
	grammar->addRule(new Rule(2, nodeList));
	
	// C -> c
	nodeList.push_back(NodeType(ParserGrammar::TOKEN, 2));
	grammar->addRule(new Rule(3, nodeList));
	nodeList.clear();
	
	// C -> EPSILON
	grammar->addRule(new Rule(3, nodeList));
}

void FirstFollowTest::tearDown() {
	delete(grammar);
}

void FirstFollowTest::firstTest() {
	grammar->sanityCheck();
	
	NonTerminalFirst firsts;
	grammar->getNonTerminalFirst(firsts);
	
	
	// expected result
	/*
	 * TokenIDs:
	 * 0: a
	 * 1: b
	 * 2: c
	 */
	SymbolSet s;
	s.insert(0);
	s.insert(1);
	s.insert(2);
	
	SymbolSet a;
	a.insert(0);
	a.insert(1);
	a.insert(2);
	a.insert(EPSILON);
	
	SymbolSet b;
	b.insert(1);
	b.insert(EPSILON);
	
	SymbolSet c;
	c.insert(2);
	c.insert(EPSILON);
	
	CPPUNIT_ASSERT(firsts.find(0) != firsts.end());
	CPPUNIT_ASSERT(firsts.find(1) != firsts.end());
	CPPUNIT_ASSERT(firsts.find(2) != firsts.end());
	CPPUNIT_ASSERT(firsts.find(3) != firsts.end());
	
	CPPUNIT_ASSERT(firsts[0] == s);
	CPPUNIT_ASSERT(firsts[1] == a);
	CPPUNIT_ASSERT(firsts[2] == b);
	CPPUNIT_ASSERT(firsts[3] == c);
}

void FirstFollowTest::followTest() {
	NonTerminalFollow follows;
	grammar->getNonTerminalFollow(follows);
	
	
	// expected result
	/*
	 * TokenIDs:
	 * 0: a
	 * 1: b
	 * 2: c
	 */
	SymbolSet s;
	s.insert(PARSER_END);
	s.insert(0);
	
	SymbolSet a;
	a.insert(0);
	
	SymbolSet b;
	b.insert(0);
	b.insert(2);
	
	SymbolSet c;
	c.insert(0);
	
	CPPUNIT_ASSERT(follows.find(0) != follows.end());
	CPPUNIT_ASSERT(follows.find(1) != follows.end());
	CPPUNIT_ASSERT(follows.find(2) != follows.end());
	CPPUNIT_ASSERT(follows.find(3) != follows.end());
	
	CPPUNIT_ASSERT(follows[0] == s);
	CPPUNIT_ASSERT(follows[1] == a);
	CPPUNIT_ASSERT(follows[2] == b);
	CPPUNIT_ASSERT(follows[3] == c);
}
