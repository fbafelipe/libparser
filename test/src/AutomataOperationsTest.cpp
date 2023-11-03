#include "AutomataOperationsTest.h"

#include "AutomataTestUtility.h"

#include "parser/Automata.h"
#include "parser/AutomataOperations.h"
#include "parser/DynamicAutomata.h"

#include <string>

CPPUNIT_TEST_SUITE_REGISTRATION(AutomataOperationsTest);

typedef DynamicAutomata::State State;
typedef DynamicAutomata::StateSet StateSet;
typedef DynamicAutomata::StateToState StateToState;

void AutomataOperationsTest::setUp() {}

void AutomataOperationsTest::tearDown() {}

void AutomataOperationsTest::unionTest1() {
	DynamicAutomata *automataA = createAutomata("a");
	DynamicAutomata *automataB = createAutomata("b");
	DynamicAutomata *result = AutomataOperations::automataUnion(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->accept("a"));
	CPPUNIT_ASSERT(automata->accept("b"));
	CPPUNIT_ASSERT(!automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("ab"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::unionTest2() {
	DynamicAutomata *automataA = createAutomata("+");
	DynamicAutomata *automataB = createAutomata("b");
	DynamicAutomata *result = AutomataOperations::automataUnion(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->accept("+"));
	CPPUNIT_ASSERT(automata->accept("b"));
	CPPUNIT_ASSERT(!automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("+b"));
	CPPUNIT_ASSERT(!automata->accept("b+"));
	CPPUNIT_ASSERT(!automata->accept("-b"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::unionTest3() {
	DynamicAutomata *automataA = createAutomata("abc");
	DynamicAutomata *automataB = createAutomata("def");
	DynamicAutomata *result = AutomataOperations::automataUnion(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->accept("abc"));
	CPPUNIT_ASSERT(automata->accept("def"));
	CPPUNIT_ASSERT(!automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("ab"));
	CPPUNIT_ASSERT(!automata->accept("de"));
	CPPUNIT_ASSERT(!automata->accept("abcdef"));
	CPPUNIT_ASSERT(!automata->accept("abcdefabcdef"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::unionTest4() {
	DynamicAutomata *automataA = createAutomata("abcde1");
	DynamicAutomata *automataB = createAutomata("abcde2");
	DynamicAutomata *result = AutomataOperations::automataUnion(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->accept("abcde1"));
	CPPUNIT_ASSERT(automata->accept("abcde2"));
	CPPUNIT_ASSERT(!automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("abcde12"));
	CPPUNIT_ASSERT(!automata->accept("abcde21"));
	CPPUNIT_ASSERT(!automata->accept("abcde"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::unionTest5() {
	DynamicAutomata *automataA = createAutomata("abcde");
	DynamicAutomata *automataB = createAutomata("abcde");
	DynamicAutomata *result = AutomataOperations::automataUnion(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->accept("abcde"));
	CPPUNIT_ASSERT(!automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("abcde12"));
	CPPUNIT_ASSERT(!automata->accept("abcde21"));
	CPPUNIT_ASSERT(!automata->accept("abcd"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::unionTest6() {
	DynamicAutomata *automataA = createAutomata("");
	DynamicAutomata *automataB = createAutomata("abcde");
	DynamicAutomata *result = AutomataOperations::automataUnion(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->accept("abcde"));
	CPPUNIT_ASSERT(automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("aabcde"));
	CPPUNIT_ASSERT(!automata->accept("ab"));
	CPPUNIT_ASSERT(!automata->accept("abcd"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::unionTest7() {
	DynamicAutomata *automataA = createAutomata("");
	DynamicAutomata *automataB = createAutomata("");
	DynamicAutomata *result = AutomataOperations::automataUnion(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->accept(""));
	CPPUNIT_ASSERT(automata->getNumStates() == 1);
	CPPUNIT_ASSERT(automata->isFinalState(0));
	CPPUNIT_ASSERT(!automata->accept("a"));
	CPPUNIT_ASSERT(!automata->accept("ab"));
	CPPUNIT_ASSERT(!automata->accept("b"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::unionTest8() {
	DynamicAutomata *automataA = createAutomata("b");
	DynamicAutomata *automataB = createAutomata("+");
	
	AutomataOperations::StateToState oldAToNew;
	AutomataOperations::StateToState oldBToNew;
	DynamicAutomata *result = AutomataOperations::automataUnion(
			automataA, oldAToNew, automataB, oldBToNew);
	
	checkUnionConnections(automataA, oldAToNew, automataB, oldBToNew, result, false);
	
	delete(automataA);
	delete(automataB);
	delete(result);
}

void AutomataOperationsTest::concatenationTest1() {
	DynamicAutomata *automataA = createAutomata("a");
	DynamicAutomata *automataB = createAutomata("b");
	DynamicAutomata *result = AutomataOperations::automataConcatenation(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->getNumStates() == 3);
	CPPUNIT_ASSERT(automata->accept("ab"));
	CPPUNIT_ASSERT(!automata->accept("a"));
	CPPUNIT_ASSERT(!automata->accept("b"));
	CPPUNIT_ASSERT(!automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("abab"));
	CPPUNIT_ASSERT(!automata->accept("aa"));
	CPPUNIT_ASSERT(!automata->accept("bb"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::concatenationTest2() {
	DynamicAutomata *automataA = createAutomata("a");
	DynamicAutomata *automataB = createAutomata("");
	DynamicAutomata *result = AutomataOperations::automataConcatenation(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->getNumStates() == 2);
	CPPUNIT_ASSERT(automata->accept("a"));
	CPPUNIT_ASSERT(!automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("aa"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::concatenationTest3() {
	DynamicAutomata *automataA = createAutomata("");
	DynamicAutomata *automataB = createAutomata("");
	DynamicAutomata *result = AutomataOperations::automataConcatenation(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->getNumStates() == 1);
	CPPUNIT_ASSERT(automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("a"));
	CPPUNIT_ASSERT(!automata->accept("aa"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::concatenationTest4() {
	DynamicAutomata *automataA = createAutomata("abcde");
	DynamicAutomata *automataB = createAutomata("fghij");
	DynamicAutomata *result = AutomataOperations::automataConcatenation(automataA, automataB);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->getNumStates() == 11);
	CPPUNIT_ASSERT(automata->accept("abcdefghij"));
	CPPUNIT_ASSERT(!automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("abcde"));
	CPPUNIT_ASSERT(!automata->accept("abcdeabcde"));
	CPPUNIT_ASSERT(!automata->accept("fghij"));
	CPPUNIT_ASSERT(!automata->accept("fghijfghij"));
	CPPUNIT_ASSERT(!automata->accept("abcdefghijabcdefghij"));
	
	delete(automataA);
	delete(automataB);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::closureTest1() {
	DynamicAutomata *automataA = createAutomata("a");
	DynamicAutomata *result = AutomataOperations::automataClosure(automataA);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->getNumStates() == 1);
	CPPUNIT_ASSERT(automata->accept(""));
	CPPUNIT_ASSERT(automata->accept("a"));
	CPPUNIT_ASSERT(automata->accept("aa"));
	CPPUNIT_ASSERT(automata->accept("aaa"));
	CPPUNIT_ASSERT(automata->accept("aaaa"));
	CPPUNIT_ASSERT(automata->accept("aaaaa"));
	CPPUNIT_ASSERT(!automata->accept("b"));
	CPPUNIT_ASSERT(!automata->accept("ab"));
	CPPUNIT_ASSERT(!automata->accept("aab"));
	
	delete(automataA);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::closureTest2() {
	DynamicAutomata *automataA = createAutomata("ab");
	DynamicAutomata *result = AutomataOperations::automataClosure(automataA);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->getNumStates() == 2);
	CPPUNIT_ASSERT(automata->accept(""));
	CPPUNIT_ASSERT(automata->accept("ab"));
	CPPUNIT_ASSERT(automata->accept("abab"));
	CPPUNIT_ASSERT(automata->accept("ababab"));
	CPPUNIT_ASSERT(automata->accept("abababab"));
	CPPUNIT_ASSERT(automata->accept("ababababab"));
	CPPUNIT_ASSERT(!automata->accept("a"));
	CPPUNIT_ASSERT(!automata->accept("b"));
	CPPUNIT_ASSERT(!automata->accept("aab"));
	CPPUNIT_ASSERT(!automata->accept("abb"));
	
	delete(automataA);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::closureTest3() {
	DynamicAutomata *automataA = createAutomata("");
	DynamicAutomata *result = AutomataOperations::automataClosure(automataA);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->getNumStates() == 1);
	CPPUNIT_ASSERT(automata->accept(""));
	CPPUNIT_ASSERT(!automata->accept("a"));
	CPPUNIT_ASSERT(!automata->accept("b"));
	
	delete(automataA);
	delete(result);
	delete(automata);
}

void AutomataOperationsTest::multOperationTest1() {
	// L(G) = "(b*ab*ab*)*|(a*ba*ba*)*ba*"
	
	DynamicAutomata *automataA = createAutomata("a");
	DynamicAutomata *automataB = createAutomata("b");
	
	// a*
	DynamicAutomata *automataC = AutomataOperations::automataClosure(automataA);
	
	// b*
	DynamicAutomata *automataD = AutomataOperations::automataClosure(automataB);
	
	// b*a
	DynamicAutomata *automataE = AutomataOperations::automataConcatenation(automataD, automataA);
	
	// b*ab*
	DynamicAutomata *automataF = AutomataOperations::automataConcatenation(automataE, automataD);
	
	// b*ab*a
	DynamicAutomata *automataG = AutomataOperations::automataConcatenation(automataF, automataA);
	
	// b*ab*ab*
	DynamicAutomata *automataH = AutomataOperations::automataConcatenation(automataG, automataD);
	
	// (b*ab*ab*)*
	DynamicAutomata *aEven = AutomataOperations::automataClosure(automataH);
	
	// a*b
	DynamicAutomata *automataI = AutomataOperations::automataConcatenation(automataC, automataB);
	
	// a*ba*
	DynamicAutomata *automataJ = AutomataOperations::automataConcatenation(automataI, automataC);
	
	// a*ba*b
	DynamicAutomata *automataK = AutomataOperations::automataConcatenation(automataJ, automataB);
	
	// a*ba*ba*
	DynamicAutomata *automataL = AutomataOperations::automataConcatenation(automataK, automataC);
	
	//(a*ba*ba*)*
	DynamicAutomata *automataM = AutomataOperations::automataClosure(automataL);
	
	//(a*ba*ba*)*b
	DynamicAutomata *automataN = AutomataOperations::automataConcatenation(automataM, automataB);
	
	//(a*ba*ba*)*ba*
	DynamicAutomata *bOdd = AutomataOperations::automataConcatenation(automataN, automataC);
	
	// (b*ab*ab*)*|(a*ba*ba*)*ba*
	DynamicAutomata *result = AutomataOperations::automataUnion(aEven, bOdd);
	result->determineAndMinimize();
	
	Automata *automata = result->toAutomata();
	CPPUNIT_ASSERT(automata->getNumStates() == 4);
	
	CPPUNIT_ASSERT(automata->accept(""));
	CPPUNIT_ASSERT(automata->accept("aa"));
	CPPUNIT_ASSERT(automata->accept("aaaa"));
	CPPUNIT_ASSERT(automata->accept("aab"));
	CPPUNIT_ASSERT(automata->accept("baa"));
	CPPUNIT_ASSERT(automata->accept("aba"));
	CPPUNIT_ASSERT(automata->accept("abababa"));
	CPPUNIT_ASSERT(automata->accept("bbaa"));
	CPPUNIT_ASSERT(automata->accept("aabb"));
	
	CPPUNIT_ASSERT(automata->accept("ba"));
	CPPUNIT_ASSERT(automata->accept("babb"));
	CPPUNIT_ASSERT(automata->accept("babab"));
	CPPUNIT_ASSERT(automata->accept("bbb"));
	CPPUNIT_ASSERT(automata->accept("bbbbb"));
	
	CPPUNIT_ASSERT(!automata->accept("a"));
	CPPUNIT_ASSERT(!automata->accept("abb"));
	CPPUNIT_ASSERT(!automata->accept("aaabb"));
	CPPUNIT_ASSERT(!automata->accept("ababa"));
	
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
	
	delete(automataA);
	delete(automataB);
	delete(automataC);
	delete(automataD);
	delete(automataE);
	delete(automataF);
	delete(automataG);
	delete(automataH);
	delete(automataI);
	delete(automataJ);
	delete(automataK);
	delete(automataL);
	delete(automataM);
	delete(automataN);
	delete(result);
	delete(aEven);
	delete(bOdd);
	delete(automata);
	delete(expected);
}

void AutomataOperationsTest::checkUnionConnections(DynamicAutomata *automataA, StateToState & oldAToNew,
		DynamicAutomata *automataB, StateToState & oldBToNew,
		DynamicAutomata *result, bool finalStateIntersect) {
	
	StateSet finalStates;
	result->getFinalStates(finalStates);
	
	for (unsigned int i = 0; i < automataA->getNumStates(); ++i) {
		CPPUNIT_ASSERT(oldAToNew.find(automataA->getState(i)) != oldAToNew.end());
	}
	for (unsigned int i = 0; i < automataB->getNumStates(); ++i) {
		CPPUNIT_ASSERT(oldBToNew.find(automataB->getState(i)) != oldBToNew.end());
	}
	
	StateSet finalStatesA;
	automataA->getFinalStates(finalStatesA);
	
	for (StateSet::iterator it = finalStatesA.begin(); it != finalStatesA.end(); ++it) {
		CPPUNIT_ASSERT(oldAToNew[*it]->isFinalState());
	}
	
	StateSet finalStatesB;
	automataB->getFinalStates(finalStatesB);
	
	for (StateSet::iterator it = finalStatesB.begin(); it != finalStatesB.end(); ++it) {
		CPPUNIT_ASSERT(oldBToNew[*it]->isFinalState());
	}
	
	if (!finalStateIntersect) {
		for (StateSet::iterator aIt = finalStatesA.begin(); aIt != finalStatesA.end(); ++aIt) {
			for (StateSet::iterator bIt = finalStatesB.begin(); bIt != finalStatesB.end(); ++bIt) {
				CPPUNIT_ASSERT(oldAToNew[*aIt] != oldBToNew[*bIt]);
			}
		}
	}
}
