#include "parser/DynamicAutomata.h"

#include "parser/Automata.h"

#include <cassert>
#include <cstring>
#include <queue>
#include <utility>

/*****************************************************************************
 * DynamicAutomata::EquivalenceClasses
 *****************************************************************************/
class DynamicAutomata::EquivalenceClasses {
	public:
		EquivalenceClasses(DynamicAutomata *a, bool finalMerge);
		
		const StateSetList & getEquivalenceClasses() const;
		
	private:
		bool splitEquivalenceClasses();
		bool fillEquivalenceClasses(StateSet & newClass, StateSet::iterator stateAIt);
		bool isEquivalent(State *stateA, State *stateB) const;
		
		void printEquivalences(std::ostream & stream, const StateSetList & eqv) const;
		
		DynamicAutomata *automata;
		
		StateSetList equivalences;
		StateSetList newEquivalences;
		
		StateSetList::iterator currentClass;
		
		StateSet markedStates;
};

const DynamicAutomata::StateSetList & DynamicAutomata::EquivalenceClasses::getEquivalenceClasses() const {
	return equivalences;
}

DynamicAutomata::EquivalenceClasses::EquivalenceClasses(DynamicAutomata *a,
		bool finalMerge) : automata(a) {
	
	StateSet finalStates;
	automata->getFinalStates(finalStates);
	
	StateSet nonFinalStates;
	automata->getNonFinalStates(nonFinalStates);
	nonFinalStates.insert(NULL);
	
	if (finalMerge) equivalences.push_back(finalStates);
	else {
		// do not merge final states
		// put each final state in an equivalence class
		for (StateSet::const_iterator it = finalStates.begin(); it != finalStates.end(); ++it) {
			StateSet equvClass;
			equvClass.insert(*it);
			equivalences.push_back(equvClass);
		}
	}
	
	equivalences.push_back(nonFinalStates);
	
	bool changed = true;
	while (changed) {
		newEquivalences.clear();
		changed = splitEquivalenceClasses();
		equivalences = newEquivalences;
	}
}

bool DynamicAutomata::EquivalenceClasses::splitEquivalenceClasses() {
	bool changed = false;
	
	for (StateSetList::iterator it = equivalences.begin(); it != equivalences.end(); ++it) {
		currentClass = it;
		markedStates.clear();
		
		for (StateSet::iterator it2 = it->begin(); it2 != it->end(); ++it2) {
			if (markedStates.find(*it2) == markedStates.end()) {
				markedStates.insert(*it2);
				
				StateSet newClass;
				newClass.insert(*it2);
				
				changed |= fillEquivalenceClasses(newClass, it2);
				
				newEquivalences.push_back(newClass);
			}
		}
		
		if (markedStates.find(*it->begin()) == markedStates.end()) {
			StateSet newClass;
			newClass.insert(*it->begin());
			newEquivalences.push_back(newClass);
		}
	}
	
	return changed;
}

bool DynamicAutomata::EquivalenceClasses::fillEquivalenceClasses(StateSet & newClass,
		StateSet::iterator stateAIt) {
	bool changed = false;
	
	StateSet::iterator stateBIt = stateAIt;
	for (++stateBIt; stateBIt != currentClass->end(); ++stateBIt) {
		if (markedStates.find(*stateBIt) == markedStates.end()) {
			if (isEquivalent(*stateAIt, *stateBIt)) {
				newClass.insert(*stateBIt);
				markedStates.insert(*stateBIt);
			}
			else changed = true;
		}
	}
	
	return changed;
}

bool DynamicAutomata::EquivalenceClasses::isEquivalent(State *stateA, State *stateB) const {
	// check if both are PHI
	if (!stateA && !stateB) return true;
	
	// check if one of them are PHI
	if (!stateA) {
		// if stateB has any transition to a live state, it is not equivalent
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			const StateSet *stateList = stateB->getTransitions(input);
			if (stateList) {
				assert(!stateList->empty());
				for (StateSetList::const_iterator it = equivalences.begin();
						it != equivalences.end(); ++it) {
					
					if ((it->find(*stateList->begin()) != it->end()) ^ (it->find(NULL) != it->end())) {
						return false;
					}
				}
			}
		}
		
		return true;
	}
	if (!stateB) return isEquivalent(stateB, stateA);
	
	for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
		State *tsa = stateA->getTransition(input);
		State *tsb = stateB->getTransition(input);
		
		for (StateSetList::const_iterator it = equivalences.begin();
				it != equivalences.end(); ++it) {
			
			if ((it->find(tsa) != it->end()) ^ (it->find(tsb) != it->end())) {
				return false;
			}
		}
	}
	
	return true;
}

void DynamicAutomata::EquivalenceClasses::printEquivalences(std::ostream & stream,
		const StateSetList & eqv) const {
	
	StateIndex stateIndex;
	automata->getStateIndex(stateIndex);
	for (StateSetList::const_iterator it = eqv.begin(); it != eqv.end(); ++it) {
		stream << "{";
		for (StateSet::iterator it2 = it->begin(); it2 != it->end(); ++it2) {
			if (it2 != it->begin()) stream << ", ";
			if (*it2) stream << stateIndex[*it2];
			else stream << "PHY";
		}
		stream << "} ";
	}
	stream << "\n";
}

/*****************************************************************************
 * DynamicAutomata::State
 *****************************************************************************/
DynamicAutomata::State::State() : finalState(false) {
	memset(transitions, 0, ALPHABET_SIZE * sizeof(StateList *));
}

DynamicAutomata::State::~State() {
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) delete(transitions[i]);
}

const DynamicAutomata::StateSet *DynamicAutomata::State::getTransitions(char input) const {
	assert(input >= 0);
	return transitions[(int)input];
}

DynamicAutomata::State *DynamicAutomata::State::getTransition(char input) const {
	assert(input >= 0);
	
	StateSet *stateList = transitions[(int)input];
	if (stateList) {
		assert(!stateList->empty());
		return *stateList->begin();
	}
	
	return NULL;
}

void DynamicAutomata::State::addTransition(char input, State *destState) {
	assert(input >= 0);
	
	StateSet *stateList = transitions[(int)input];
	if (!stateList) stateList = transitions[(int)input] = new StateSet();
	
	stateList->insert(destState);
}

void DynamicAutomata::State::removeTransition(char input, State *destState) {
	assert(input >= 0);
	
	StateSet *stateList = transitions[(int)input];
	if (!stateList) return;
	
	StateSet::iterator it = stateList->find(destState);
	if (it != stateList->end()) {
		stateList->erase(it);
		if (stateList->empty()) {
			delete(stateList);
			transitions[(int)input] = NULL;
		}
	}
}

void DynamicAutomata::State::setTransition(char input, State *destState) {
	StateSet *stateList = transitions[(int)input];
	
	if (stateList) stateList->clear();
	else stateList = transitions[(int)input] = new StateSet();
	
	stateList->insert(destState);
}

const DynamicAutomata::StateSet *DynamicAutomata::State::getEpsilonTransitions() {
	return &epsilonTransitions;
}

void DynamicAutomata::State::addEpsilonTransition(State *destState) {
	epsilonTransitions.insert(destState);
}

void DynamicAutomata::State::removeEpsilonTransition(State *destState) {
	StateSet::iterator it = epsilonTransitions.find(destState);
	if (it != epsilonTransitions.end()) epsilonTransitions.erase(it);
}

void DynamicAutomata::State::clearEpsilonTransitions() {
	epsilonTransitions.clear();
}

void DynamicAutomata::State::clearAllTransitions() {
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) {
		delete(transitions[i]);
		transitions[i] = NULL;
	}
	
	epsilonTransitions.clear();
}

bool DynamicAutomata::State::isFinalState() const {
	return finalState;
}

void DynamicAutomata::State::setFinalState(bool finalState) {
	this->finalState = finalState;
}

void DynamicAutomata::State::getEpsilonReachableStates(StateToSet & closure,
		StateSet & marked, StateSet & output) {
	
	StateToSet::const_iterator cIt =  closure.find(this);
	if (cIt != closure.end()) {
		// insert all states from closure[this] to output
		for (StateSet::const_iterator it = cIt->second.begin(); it != cIt->second.end(); ++it) {
			output.insert(*it);
		}
		return;
	}
	
	output.insert(this);
	
	for (StateSet::iterator it = epsilonTransitions.begin();
			it != epsilonTransitions.end(); ++it) {
		if (marked.find(*it) == marked.end()) {
			marked.insert(*it);
			(*it)->getEpsilonReachableStates(closure, marked, output);
		}
	}
}

/*****************************************************************************
 * DynamicAutomata
 *****************************************************************************/
DynamicAutomata::DynamicAutomata() {
	initialState = createState();
}

DynamicAutomata::DynamicAutomata(const DynamicAutomata *automata) {
	StateIndex stateIndex;
	automata->getStateIndex(stateIndex);
	
	// create the initialState
	initialState = createState();
	
	// create the other states
	for (unsigned int i = 1; i < automata->states.size(); ++i) createState();
	
	// add the transitions
	for (unsigned int i = 0; i < automata->states.size(); ++i) {
		if (automata->states[i]->isFinalState()) states[i]->setFinalState(true);
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			const StateSet *stateList = automata->states[i]->getTransitions(input);
			
			if (stateList) {
				for (StateSet::const_iterator it = stateList->begin(); it != stateList->end(); ++it) {
					states[i]->addTransition(input, states[stateIndex[*it]]);
				}
			}
		}
		
		// add the epsilon transitions
		const StateSet *stateList = automata->states[i]->getEpsilonTransitions();
		for (StateSet::const_iterator it = stateList->begin(); it != stateList->end(); ++it) {
			states[i]->addEpsilonTransition(states[stateIndex[*it]]);
		}
	}
}

DynamicAutomata::DynamicAutomata(const Automata *automata) {
	// create the initialState
	initialState = createState();
	
	// create the states
	unsigned int numStates = automata->getNumStates();
	for (unsigned int i = 1; i < numStates; ++i) createState();
	
	// add the transitions
	for (unsigned int i = 0; i < numStates; ++i) {
		State *state = states[i];
		if (automata->isFinalState(i)) state->setFinalState(true);
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			int dest = automata->getTransition(i, input);
			if (dest != -1) {
				assert(dest >= 0 && dest < (int)numStates);
				state->addTransition(input, states[dest]);
			}
		}
	}
}

DynamicAutomata::~DynamicAutomata() {
	for (StateList::iterator it = states.begin(); it != states.end(); ++it) {
		delete(*it);
	}
}

DynamicAutomata::State *DynamicAutomata::getInitialState() const {
	return initialState;
}

DynamicAutomata::State *DynamicAutomata::createState() {
	State *state = new State();
	states.push_back(state);
	return state;
}

void DynamicAutomata::getFinalStates(StateSet & finalStates) const {
	for (StateList::const_iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->isFinalState()) finalStates.insert(*it);
	}
}

void DynamicAutomata::getNonFinalStates(StateSet & nonFinalStates) const {
	for (StateList::const_iterator it = states.begin(); it != states.end(); ++it) {
		if (!(*it)->isFinalState()) nonFinalStates.insert(*it);
	}
}

void DynamicAutomata::getEpsilonClosure(StateToSet & closure) const {
	for (StateList::const_iterator it = states.begin(); it != states.end(); ++it) {
		StateSet marked;
		marked.insert(*it);
		
		StateSet reachable;
		(*it)->getEpsilonReachableStates(closure, marked, reachable);
		closure[*it] = reachable;
	}
}

void DynamicAutomata::removeEpsilonTransitions() {
	StateSet finalStates;
	getFinalStates(finalStates);
	
	StateToSet closure;
	getEpsilonClosure(closure);
	
	for (StateToSet::iterator it = closure.begin(); it != closure.end(); ++it) {
		bool finalState = false;
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			for (StateSet::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
				if (it->first == *it2) continue;
				
				finalState |= (*it2)->isFinalState();
				const StateSet *transitions = (*it2)->getTransitions(input);
				
				if (transitions) {
					for (StateSet::iterator it3 = transitions->begin(); it3 != transitions->end(); ++it3) {
						it->first->addTransition(input, *it3);
					}
				}
			}
		}
		
		if (finalState) it->first->setFinalState(true);
	}
	
	// clear the epsilon transitions
	for (StateList::iterator it = states.begin(); it != states.end(); ++it) {
		(*it)->clearEpsilonTransitions();
	}
}

void DynamicAutomata::determinize() {
	StateSetToState stateMap;
	StateList oldStates;
	determinize(stateMap, oldStates);
	
	for (StateList::iterator it = oldStates.begin(); it != oldStates.end(); ++it) {
		delete(*it);
	}
}

void DynamicAutomata::determinize(StateSetToState & stateMap, StateList & oldStates) {
	StateSet oldFinalStates;
	getFinalStates(oldFinalStates);
	
	std::set<StateSet> marked;
	
	std::queue<StateSet> defineQueue;
	StateSet initialSet;
	initialSet.insert(initialState);
	marked.insert(initialSet);
	defineQueue.push(initialSet);
	
	while (!defineQueue.empty()) {
		StateSet stateSet = defineQueue.front();
		defineQueue.pop();
		
		State *state;
		StateSetToState::iterator stateIt = stateMap.find(stateSet);
		if (stateIt != stateMap.end()) state = stateIt->second;
		else {
			state = new State();
			stateMap[stateSet] = state;
		}
		
		for (StateSet::iterator it = stateSet.begin(); it != stateSet.end(); ++it) {
			if ((*it)->isFinalState()) {
				state->setFinalState(true);
				break;
			}
		}
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			StateSet target;
			
			for (StateSet::iterator it = stateSet.begin(); it != stateSet.end(); ++it) {
				const StateSet *transitions = (*it)->getTransitions(input);
				if (transitions) {
					for (StateSet::iterator it2 = transitions->begin(); it2 != transitions->end(); ++it2) {
						target.insert(*it2);
					}
				}
			}
			
			if (!target.empty()) {
				State *targetState;
				
				std::map<StateSet, State *>::iterator targetIt = stateMap.find(target);
				if (targetIt != stateMap.end()) targetState = targetIt->second;
				else {
					targetState = new State();
					stateMap[target] = targetState;
				}
				
				state->addTransition(input, targetState);
				
				if (marked.find(target) == marked.end()) {
					marked.insert(target);
					defineQueue.push(target);
				}
			}
		}
	}
	
	// put the states in the old state list
	for (StateList::iterator it = states.begin(); it != states.end(); ++it) {
		oldStates.push_back(*it);
	}
	states.clear();
	
	initialState = stateMap[initialSet];
	// make sure the initial state is the first
	states.push_back(initialState);
	
	// add the new states
	for (std::map<StateSet, State *>::iterator it = stateMap.begin(); it != stateMap.end(); ++it) {
		if (it->second != initialState) states.push_back(it->second);
	}
}

void DynamicAutomata::minimize() {
	StateToState oldStateToNewState;
	StateList oldStates;
	minimize(oldStateToNewState, oldStates);
	
	for (StateList::iterator it = oldStates.begin(); it != oldStates.end(); ++it) {
		delete(*it);
	}
}

void DynamicAutomata::minimize(StateToState & oldStateToNewState, StateList & oldStates) {
	minimize(oldStateToNewState, oldStates, true);
}

void DynamicAutomata::minimizeNoFinalMerge() {
	StateToState oldStateToNewState;
	StateList oldStates;
	minimize(oldStateToNewState, oldStates);
	
	for (StateList::iterator it = oldStates.begin(); it != oldStates.end(); ++it) {
		delete(*it);
	}
}

void DynamicAutomata::minimizeNoFinalMerge(StateToState & oldStateToNewState,
		StateList & oldStates) {
	minimize(oldStateToNewState, oldStates, false);
}

void DynamicAutomata::minimize(StateToState & oldStateToNewState, StateList & oldStates,
		bool finalMerge) {
	
	StateSet oldFinalStates;
	getFinalStates(oldFinalStates);
	
	State *oldInitialState = initialState;
	
	if (oldFinalStates.empty()) {
		// the automata rejects all sentences
		makeEmptyAutomata();
		oldStateToNewState[oldInitialState] = initialState;
		return;
	}
	
	EquivalenceClasses equivalenceClasses(this, finalMerge);
	const StateSetList & equivalences =  equivalenceClasses.getEquivalenceClasses();
	
	// map the new state to it's class
	StateToSet newStateToClass;
	
	// states that have been reused, they won't be deleted
	StateSet reusedStates;
	
	initialState = NULL;
	
	for (StateSetList::const_iterator it = equivalences.begin(); it != equivalences.end(); ++it) {
		assert(!it->empty());
		
		State *state = NULL;
		
		if (it->find(NULL) == it->end()) {
			// reuse the state
			state = *it->begin();
			reusedStates.insert(state);
			newStateToClass[state] = *it;
		}
		
		for (StateSet::const_iterator it2 = it->begin(); it2 != it->end(); ++it2) {
			if (*it2) {
				if (*it2 == oldInitialState) {
					assert(state); // initialState cannot die
					initialState = state;
				}
				oldStateToNewState[*it2] = state;
			}
		}
	}
	
	for (StateToSet::iterator it = newStateToClass.begin(); it != newStateToClass.end(); ++it) {
		// update the state transitions
		State *newState = it->first;
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			State *state = newState->getTransition(input);
			if (state) {
				State *target = oldStateToNewState[state];
				if (target) newState->setTransition(input, target);
				else newState->removeTransition(input, state);
			}
		}
	}
	
	// put the removed states in the oldStates list
	for (StateList::iterator it = states.begin(); it != states.end(); ++it) {
		if (reusedStates.find(*it) == reusedStates.end()) oldStates.push_back(*it);
	}
	states.clear();
	
	// make sure the initialState is the first
	states.push_back(initialState);
	
	// add the new states
	for (StateToSet::iterator it = newStateToClass.begin(); it != newStateToClass.end(); ++it) {
		if (it->first != initialState) states.push_back(it->first);
	}
	
	assert(initialState);
}

void DynamicAutomata::makeEmptyAutomata() {
	for (StateList::iterator it = states.begin(); it != states.end(); ++it) delete(*it);
	states.clear();
	
	initialState = createState();
}

void DynamicAutomata::makeEmptyAutomata(StateList & oldStates) {
	for (StateList::iterator it = states.begin(); it != states.end(); ++it) {
		oldStates.push_back(*it);
	}
	states.clear();
	
	initialState = createState();
}

void DynamicAutomata::determineAndMinimize() {
	removeEpsilonTransitions();
	determinize();
	minimize();
}

void DynamicAutomata::determineAndMinimize(StateSetToState & stateMap, StateList & oldStates) {
	removeEpsilonTransitions();
	
	StateSetToState tempMap;
	determinize(tempMap, oldStates);
	
	StateToState oldToNew;
	minimize(oldToNew, oldStates);
	
	// update state map to stateMap
	for (StateSetToState::const_iterator it = tempMap.begin(); it != tempMap.end(); ++it) {
		assert(oldToNew.find(it->second) != oldToNew.end());
		stateMap[it->first] = oldToNew[it->second];
	}
}

Automata *DynamicAutomata::toAutomata() const {
	Automata *automata = new Automata(states.size());
	
	StateIndex stateIndex;
	getStateIndex(stateIndex);
	
	for (unsigned int i = 0; i < states.size(); ++i) {
		if (states[i]->isFinalState()) automata->setFinalState(i);
		
		for (unsigned int j = 0; j < ALPHABET_SIZE; ++j) {
			const StateSet *transitions = states[i]->getTransitions((char)j);
			if (transitions) {
				assert(transitions->size() == 1);
				automata->setTransition(i, j, stateIndex[*transitions->begin()]);
			}
		}
	}
	
	return automata;
}

void DynamicAutomata::getStateIndex(StateIndex & stateIndex) const {
	for (unsigned int i = 0; i < states.size(); ++i) {
		stateIndex[states[i]] = i;
	}
}

unsigned int DynamicAutomata::getNumStates() const {
	return states.size();
}

const DynamicAutomata::StateList & DynamicAutomata::getStates() const {
	return states;
}

DynamicAutomata::State *DynamicAutomata::getState(unsigned int index) const {
	assert(index < states.size());
	return states[index];
}

DynamicAutomata *DynamicAutomata::clone() const {
	return new DynamicAutomata(this);
}

bool DynamicAutomata::operator==(const DynamicAutomata & automata) const {
	StateToState thisToOther;
	return compareTo(&automata, thisToOther);
}

bool DynamicAutomata::operator!=(const DynamicAutomata & automata) const {
	return !(*this == automata);
}

bool DynamicAutomata::operator==(const Automata & automata) const {
	DynamicAutomata *dynamicAutomata = new DynamicAutomata(&automata);
	
	bool result = *this == *dynamicAutomata;
	
	delete(dynamicAutomata);
	
	return result;
}

bool DynamicAutomata::operator!=(const Automata & automata) const {
	return !(*this == automata);
}

bool DynamicAutomata::compareTo(const DynamicAutomata *automata, StateToState & thisToOther) const {
	State *initialStateA = initialState;
	State *initialStateB = automata->initialState;
	
	if (initialStateA->isFinalState() != initialStateB->isFinalState()) return false;
	
	std::pair<State *, State *> state = std::make_pair(initialStateA, initialStateB);
	thisToOther[initialStateA] = initialStateB;
	
	std::queue<std::pair<State *, State *> > stateQueue;
	stateQueue.push(state);
	
	StateSet markedA, markedB;
	markedA.insert(initialStateA);
	markedB.insert(initialStateB);
	
	while (!stateQueue.empty()) {
		state = stateQueue.front();
		stateQueue.pop();
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			State *targetA = state.first->getTransition(input);
			State *targetB = state.second->getTransition(input);
			
			if ((bool)targetA != (bool)targetB) return false;
			if (targetA) {
				if (targetA->isFinalState() != targetB->isFinalState()) return false;
				
				bool wasMarkedA = markedA.find(targetA) != markedA.end();
				bool wasMarkedB = markedB.find(targetB) != markedB.end();
				if (wasMarkedA != wasMarkedB) return false;
				
				if (!wasMarkedA) {
					markedA.insert(targetA);
					markedB.insert(targetB);
					stateQueue.push(std::make_pair(targetA, targetB));
					thisToOther[targetA] = targetB;
				}
			}
		}
	}
	
	return true;
}

std::ostream & operator<<(std::ostream & stream, const DynamicAutomata & automata) {
	DynamicAutomata::StateIndex stateIndex;
	automata.getStateIndex(stateIndex);
	
	DynamicAutomata::StateSet finalStates;
	automata.getFinalStates(finalStates);
	
	stream << "Initial State: " << stateIndex[automata.initialState] << "\n";
	
	stream << "Final States:";
	for (DynamicAutomata::StateSet::const_iterator it = finalStates.begin();
			it != finalStates.end(); ++it) {
		stream << " " << stateIndex[*it];
	}
	stream << "\n";
	
	for (unsigned int i = 0; i < automata.states.size(); ++i) {
		DynamicAutomata::State *state = automata.states[i];
		const DynamicAutomata::StateSet *stateList;
		
		for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
			stateList = state->getTransitions(input);
			if (stateList) {
				for (DynamicAutomata::StateSet::const_iterator it = stateList->begin();
						it != stateList->end(); ++it) {
					if (isprint(input)) {
						char buf[2];
						std::string in;
						
						switch (input) {
							case '\n':
								in = std::string("\n");
								break;
							case '\t':
								in = std::string("\t");
								break;
							case '\r':
								in = std::string("\r");
								break;
							default:
								buf[0] = input;
								buf[1] = '\0';
								in = std::string(buf);
						}
						
						stream << "State " << i << " [" << input
								<< " (\'" << in << "\')] -> " << stateIndex[*it] << "\n";
					}
					else stream << "State " << i << " [" << input << "] -> " << stateIndex[*it] << "\n";
				}
			}
		}
		stateList = state->getEpsilonTransitions();
		for (DynamicAutomata::StateSet::const_iterator it = stateList->begin();
				it != stateList->end(); ++it) {
			stream << "State " << i << " [EPSILON] -> " << stateIndex[*it] << "\n";
		}
	}
	
	return stream;
}
