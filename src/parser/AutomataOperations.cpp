#include "parser/AutomataOperations.h"

#include <cassert>

AutomataOperations::AutomataOperations() {}

/*****************************************************************************
 * AutomataOperations::automataUnion
 *****************************************************************************/
DynamicAutomata *AutomataOperations::automataUnion(const DynamicAutomata *automataA,
		const DynamicAutomata *automataB) {
	StateToState oldStatesAToNewStates;
	StateToState oldStatesBToNewStates;
	return automataUnion(automataA, oldStatesAToNewStates, automataB, oldStatesBToNewStates);
}

DynamicAutomata *AutomataOperations::automataUnion(
		const DynamicAutomata *automataA, StateToState & oldStatesAToNewStates,
		const DynamicAutomata *automataB, StateToState & oldStatesBToNewStates) {
	
	DynamicAutomata *result = new DynamicAutomata();
	
	State *initialState = result->getInitialState();
	
	unsigned int aStatesOffset = 1;
	unsigned int bStatesOffset = aStatesOffset + automataA->getNumStates();
	
	unsigned int aNumStates = automataA->getNumStates();
	unsigned int bNumStates = automataB->getNumStates();
	
	// create and map states from automataA
	for (unsigned int i = 0; i < aNumStates; ++i) {
		oldStatesAToNewStates[automataA->getState(i)] = result->createState();
	}
	
	// create and map states from automataB
	for (unsigned int i = 0; i < bNumStates; ++i) {
		oldStatesBToNewStates[automataB->getState(i)] = result->createState();
	}
	
	// add epsilon transitions from result's initialState to
	// automataA initialState and automataB initialState
	initialState->addEpsilonTransition(result->getState(aStatesOffset));
	initialState->addEpsilonTransition(result->getState(bStatesOffset));
	
	// add the transitions from automataA
	for (unsigned int i = 0; i < aNumStates; ++i) {
		State *aState = automataA->getState(i);
		State *resultState = result->getState(aStatesOffset + i);
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			const StateSet *stateList = aState->getTransitions(input);
			if (stateList) {
				for (StateSet::iterator it = stateList->begin(); it != stateList->end(); ++it) {
					resultState->addTransition(input, oldStatesAToNewStates[*it]);
				}
			}
		}
		
		// add the epsilon transitions
		const StateSet *stateList = aState->getEpsilonTransitions();
		for (StateSet::iterator it = stateList->begin(); it != stateList->end(); ++it) {
			resultState->addEpsilonTransition(oldStatesAToNewStates[*it]);
		}
	}
	
	// add the transitions from automataB
	for (unsigned int i = 0; i < bNumStates; ++i) {
		State *bState = automataB->getState(i);
		State *resultState = result->getState(bStatesOffset + i);
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			const StateSet *stateList = bState->getTransitions(input);
			if (stateList) {
				for (StateSet::iterator it = stateList->begin(); it != stateList->end(); ++it) {
					resultState->addTransition(input, oldStatesBToNewStates[*it]);
				}
			}
		}
		
		// add the epsilon transitions
		const StateSet *stateList = bState->getEpsilonTransitions();
		for (StateSet::iterator it = stateList->begin(); it != stateList->end(); ++it) {
			resultState->addEpsilonTransition(oldStatesBToNewStates[*it]);
		}
	}
	
	// set the final states from automataA
	StateSet finalStatesA;
	automataA->getFinalStates(finalStatesA);
	for (StateSet::iterator it = finalStatesA.begin(); it != finalStatesA.end(); ++it) {
		assert(oldStatesAToNewStates[*it]);
		oldStatesAToNewStates[*it]->setFinalState(true);
	}
	
	// set the final states from automataB
	StateSet finalStatesB;
	automataB->getFinalStates(finalStatesB);
	for (StateSet::iterator it = finalStatesB.begin(); it != finalStatesB.end(); ++it) {
		assert(oldStatesBToNewStates[*it]);
		oldStatesBToNewStates[*it]->setFinalState(true);
	}
	
	return result;
}

/*****************************************************************************
 * AutomataOperations::automataConcatenation
 *****************************************************************************/
DynamicAutomata *AutomataOperations::automataConcatenation(const DynamicAutomata *automataA,
		const DynamicAutomata *automataB) {
	StateToState oldStatesAToNewStates;
	StateToState oldStatesBToNewStates;
	return automataConcatenation(automataA, oldStatesAToNewStates, automataB, oldStatesBToNewStates);
}

DynamicAutomata *AutomataOperations::automataConcatenation(
		const DynamicAutomata *automataA, StateToState & oldStatesAToNewStates,
		const DynamicAutomata *automataB, StateToState & oldStatesBToNewStates) {
	
	DynamicAutomata *result = automataA->clone();
	
	// the final states from A in result
	StateSet appendStates;
	
	// setup the oldStates map and the appendStates set
	unsigned int automataANumStates = automataA->getNumStates();
	for (unsigned int i = 0; i < automataANumStates; ++i) {
		oldStatesAToNewStates[automataA->getState(i)] = result->getState(i);
		
		if (result->getState(i)->isFinalState()) {
			appendStates.insert(result->getState(i));
			
			// remove the final state property
			result->getState(i)->setFinalState(false);
		}
	}
	
	// add the B states in the result automata
	unsigned int automataBNumStates = automataB->getNumStates();
	for (unsigned int i = 0; i < automataBNumStates; ++i) result->createState();
	
	// add the transitions from B
	StateIndex bStateIndex;
	automataB->getStateIndex(bStateIndex);
	for (unsigned int i = 0; i < automataBNumStates; ++i) {
		State *resultState = result->getState(automataANumStates + i);
		State *bState = automataB->getState(i);
		oldStatesBToNewStates[bState] = resultState;
		
		if (bState->isFinalState()) resultState->setFinalState(true);
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			const StateSet *stateList = bState->getTransitions(input);
			if (stateList) {
				for (StateSet::const_iterator it = stateList->begin(); it != stateList->end(); ++it) {
					unsigned int targetIndex = automataANumStates + bStateIndex[*it];
					assert(targetIndex < result->getNumStates());
					State *target = result->getState(targetIndex);
					resultState->addTransition(input, target);
				}
			}
		}
		
		// copy the epsilon transitions
		const StateSet *stateList = bState->getEpsilonTransitions();
		for (StateSet::const_iterator it = stateList->begin(); it != stateList->end(); ++it) {
			unsigned int targetIndex = automataANumStates + bStateIndex[*it];
			assert(targetIndex < result->getNumStates());
			State *target = result->getState(targetIndex);
			resultState->addEpsilonTransition(target);
		}
	}
	
	// add epsilon transitions from the appendPoints to B initialState
	assert(automataB->getInitialState());
	State *resultBInitialState = oldStatesBToNewStates[automataB->getInitialState()];
	for (StateSet::const_iterator it = appendStates.begin(); it != appendStates.end(); ++it) {
		(*it)->addEpsilonTransition(resultBInitialState);
	}
	
	return result;
}

/*****************************************************************************
 * AutomataOperations::automataClosure
 *****************************************************************************/
DynamicAutomata *AutomataOperations::automataClosure(const DynamicAutomata *automata) {
	StateToState oldStatesToNewStates;
	return automataClosure(automata, oldStatesToNewStates);
}

DynamicAutomata *AutomataOperations::automataClosure(const DynamicAutomata *automata,
		StateToState & oldStatesToNewStates) {
	
	DynamicAutomata *result = automata->clone();
	
	State *initialState = result->getInitialState();
	
	// add an epsilon transition from the final states to the initial state
	StateSet finalStates;
	result->getFinalStates(finalStates);
	for (StateSet::const_iterator it = finalStates.begin(); it != finalStates.end(); ++it) {
		(*it)->addEpsilonTransition(initialState);
	}
	
	// set the initial state as a final state
	initialState->setFinalState(true);
	
	// setup the oldStatesToNewStates map
	unsigned int numStates = result->getNumStates();
	for (unsigned int i = 0; i < numStates; ++i) {
		oldStatesToNewStates[automata->getState(i)] = result->getState(i);
	}
	
	return result;
}
