#include "DynamicAutomataTest.h"

#include "AutomataTestUtility.h"

#include "parser/Automata.h"
#include "parser/DynamicAutomata.h"

CPPUNIT_TEST_SUITE_REGISTRATION(DynamicAutomataTest);

void DynamicAutomataTest::setUp() {}

void DynamicAutomataTest::tearDown() {}

void DynamicAutomataTest::minimizationSimpleTest() {
	DynamicAutomata *automata = new DynamicAutomata();
	
	DynamicAutomata::State *a = automata->getInitialState();
	DynamicAutomata::State *b = automata->createState();
	DynamicAutomata::State *c = automata->createState();
	
	b->setFinalState(true);
	c->setFinalState(true);
	
	a->addTransition('a', b);
	b->addTransition('a', c);
	c->addTransition('a', b);
	
	DynamicAutomata *minimized = automata->clone();
	minimized->determineAndMinimize();
	
	CPPUNIT_ASSERT(minimized->getNumStates() == 2);
	
	Automata *expected = new Automata(2);
	expected->setFinalState(1);
	expected->setTransition(0, 'a', 1);
	expected->setTransition(1, 'a', 1);
	
	CPPUNIT_ASSERT(*minimized == *expected);
	
	delete(expected);
	delete(minimized);
	delete(automata);
}

void DynamicAutomataTest::minimizationDeadStatesTest() {
	DynamicAutomata *automata = new DynamicAutomata();
	
	DynamicAutomata::State *s = automata->getInitialState();
	DynamicAutomata::State *a1 = automata->createState();
	DynamicAutomata::State *a2 = automata->createState();
	DynamicAutomata::State *b1 = automata->createState();
	DynamicAutomata::State *b2 = automata->createState();
	
	a1->setFinalState(true);
	
	s->addTransition('a', a1);
	s->addTransition('b', b1);
	a1->addTransition('a', a2);
	a2->addTransition('a', a1);
	b1->addTransition('b', b2);
	
	DynamicAutomata *minimized = automata->clone();
	minimized->determineAndMinimize();
	
	CPPUNIT_ASSERT(minimized->getNumStates() == 2);
	
	Automata *expected = new Automata(2);
	expected->setFinalState(1);
	expected->setTransition(0, 'a', 1);
	expected->setTransition(1, 'a', 0);
	
	CPPUNIT_ASSERT(*minimized == *expected);
	
	delete(expected);
	delete(minimized);
	delete(automata);
}

void DynamicAutomataTest::minimizationEmptyLanguageTest() {
	DynamicAutomata *automata = new DynamicAutomata();
	
	DynamicAutomata::State *s = automata->getInitialState();
	DynamicAutomata::State *a = automata->getInitialState();
	
	s->addTransition('a', a);
	a->addTransition('a', a);
	
	DynamicAutomata *minimized = automata->clone();
	minimized->determineAndMinimize();
	
	CPPUNIT_ASSERT(minimized->getNumStates() == 1);
	
	Automata *expected = new Automata(1);
	
	CPPUNIT_ASSERT(*minimized == *expected);
	
	delete(expected);
	delete(minimized);
	delete(automata);
}

void DynamicAutomataTest::conversionTest() {
	Automata *automata = new Automata(4);
	automata->setFinalState(3);
	automata->setTransition(0, 'a', 1);
	automata->setTransition(1, 'b', 2);
	automata->setTransition(2, 'c', 3);
	
	CPPUNIT_ASSERT(*automata == *automata);
	
	DynamicAutomata *dAutomata = new DynamicAutomata(automata);
	
	CPPUNIT_ASSERT(*dAutomata == *dAutomata);
	
	CPPUNIT_ASSERT(*dAutomata == *automata);
	CPPUNIT_ASSERT(*automata == *dAutomata);
	
	Automata *automata2 = dAutomata->toAutomata();
	CPPUNIT_ASSERT(*automata2 == *automata);
	CPPUNIT_ASSERT(*automata == *automata2);
	
	CPPUNIT_ASSERT(*dAutomata == *automata2);
	CPPUNIT_ASSERT(*automata2 == *dAutomata);
	
	dAutomata->determineAndMinimize();
	CPPUNIT_ASSERT(*dAutomata == *dAutomata);
	
	CPPUNIT_ASSERT(*dAutomata == *automata);
	CPPUNIT_ASSERT(*automata == *dAutomata);
	
	CPPUNIT_ASSERT(*dAutomata == *automata2);
	CPPUNIT_ASSERT(*automata2 == *dAutomata);
	
	DynamicAutomata *dAutomata2 = new DynamicAutomata(automata2);
	dAutomata2->determineAndMinimize();
	CPPUNIT_ASSERT(*dAutomata2 == *dAutomata2);
	
	CPPUNIT_ASSERT(*dAutomata == *dAutomata2);
	CPPUNIT_ASSERT(*dAutomata2 == *dAutomata);
	
	CPPUNIT_ASSERT(*dAutomata2 == *automata);
	CPPUNIT_ASSERT(*automata == *dAutomata2);
	
	CPPUNIT_ASSERT(*dAutomata2 == *automata2);
	CPPUNIT_ASSERT(*automata2 == *dAutomata2);
	
	
	// check num states
	CPPUNIT_ASSERT(automata->getNumStates() == 4);
	CPPUNIT_ASSERT(automata2->getNumStates() == 4);
	CPPUNIT_ASSERT(dAutomata->getNumStates() == 4);
	CPPUNIT_ASSERT(dAutomata2->getNumStates() == 4);
	
	delete(automata);
	delete(automata2);
	delete(dAutomata);
	delete(dAutomata2);
}

void DynamicAutomataTest::conversionTest2() {
	Automata *automata = new Automata(2);
	automata->setTransition(0, 'a', 1);
	automata->setFinalState(1);
	
	DynamicAutomata *dAutomata = new DynamicAutomata(automata);
	dAutomata->determineAndMinimize();
	
	CPPUNIT_ASSERT(*automata == *dAutomata);
	CPPUNIT_ASSERT(*dAutomata == *automata);
	
	CPPUNIT_ASSERT(automata->getNumStates() == 2);
	CPPUNIT_ASSERT(dAutomata->getNumStates() == 2);
	
	delete(automata);
	delete(dAutomata);
}

void DynamicAutomataTest::minimizeStateMappingTest() {
	// L(G) = {'a' is even or 'b' is odd}
	
	DynamicAutomata *automata = new DynamicAutomata();
	
	DynamicAutomata::State *s = automata->getInitialState();
	
	DynamicAutomata::State *aEven = automata->createState();
	DynamicAutomata::State *aNotEven = automata->createState();
	
	DynamicAutomata::State *bNotOdd = automata->createState();
	DynamicAutomata::State *bOdd = automata->createState();
	
	s->addEpsilonTransition(aEven);
	s->addEpsilonTransition(bNotOdd);
	
	aEven->addTransition('a', aNotEven);
	aEven->addTransition('b', aEven);
	aNotEven->addTransition('a', aEven);
	aNotEven->addTransition('b', aNotEven);
	
	bNotOdd->addTransition('a', bNotOdd);
	bNotOdd->addTransition('b', bOdd);
	bOdd->addTransition('a', bOdd);
	bOdd->addTransition('b', bNotOdd);
	
	aEven->setFinalState(true);
	bOdd->setFinalState(true);
	
	DynamicAutomata::StateSetToState oldToNew;
	DynamicAutomata::StateList oldStates;
	automata->determineAndMinimize(oldToNew, oldStates);
	
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
	
	// check the state mapping
	DynamicAutomata::State *state = automata->getInitialState();
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, s, state));
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, aEven, state));
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, bNotOdd, state));
	
	state = state->getTransition('a');
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, aNotEven, state));
	
	state = state->getTransition('a');
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, s, state));
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, aEven, state));
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, bNotOdd, state));
	
	state = state->getTransition('b');
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, bOdd, state));
	
	state = state->getTransition('b');
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, s, state));
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, aEven, state));
	CPPUNIT_ASSERT(stateMapsTo(oldToNew, bNotOdd, state));
	
	delete(automata);
	delete(expected);
	
	for (DynamicAutomata::StateList::iterator it = oldStates.begin(); it != oldStates.end(); ++it) {
		delete(*it);
	}
}
