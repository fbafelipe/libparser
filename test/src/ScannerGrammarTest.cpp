#include "ScannerGrammarTest.h"

#include "AutomataTestUtility.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ScannerGrammarTest);

void ScannerGrammarTest::setUp() {}

void ScannerGrammarTest::tearDown() {}

void ScannerGrammarTest::tokenMapTest1() {
	DynamicAutomata *automataA = createAutomata("a");
	DynamicAutomata *automataB = createAutomata("b");
	DynamicAutomata *automataC = createAutomata("+");
	
	StateToToken tokenMap;
	StateSet finalStates;
	
	automataA->getFinalStates(finalStates);
	for (StateSet::iterator it = finalStates.begin(); it != finalStates.end(); ++it) {
		tokenMap[*it] = 0;
	}
	finalStates.clear();
	
	automataB->getFinalStates(finalStates);
	for (StateSet::iterator it = finalStates.begin(); it != finalStates.end(); ++it) {
		tokenMap[*it] = 1;
	}
	finalStates.clear();
	
	automataC->getFinalStates(finalStates);
	for (StateSet::iterator it = finalStates.begin(); it != finalStates.end(); ++it) {
		tokenMap[*it] = 2;
	}
	
	DynamicAutomata *resultAB = mergeAutomatas(automataA, automataB, tokenMap);
	DynamicAutomata *result = mergeAutomatas(resultAB, automataC, tokenMap);
	
	minimizeAutomata(result, tokenMap);
	
	CPPUNIT_ASSERT(findToken(result, "a", tokenMap) == 0);
	CPPUNIT_ASSERT(findToken(result, "b", tokenMap) == 1);
	CPPUNIT_ASSERT(findToken(result, "+", tokenMap) == 2);
	CPPUNIT_ASSERT(findToken(result, "c", tokenMap) == -1);
	CPPUNIT_ASSERT(findToken(result, "", tokenMap) == -1);
	
	delete(automataA);
	delete(automataB);
	delete(automataC);
	delete(resultAB);
	delete(result);
}

int ScannerGrammarTest::findToken(DynamicAutomata *automata,
		const char *input, const StateToToken & tokenMap) {
	
	State *s = automata->getInitialState();
	while (s && *input) s = s->getTransition(*(input++));
	
	if (!s) return -1;
	
	StateToToken::const_iterator it = tokenMap.find(s);
	if (it == tokenMap.end()) return -1;
	
	return it->second;
}
