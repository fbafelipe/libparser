#include "AutomataTestUtility.h"

#include "parser/DynamicAutomata.h"

typedef DynamicAutomata::State State;

DynamicAutomata *createAutomata(const std::string & str) {
	DynamicAutomata *automata = new DynamicAutomata();
	
	State *s = automata->getInitialState();
	
	for (unsigned int i = 0; i < str.size(); ++i) {
		State *t = automata->createState();
		s->addTransition(str[i], t);
		s = t;
	}
	
	s->setFinalState(true);
	
	return automata;
}

bool stateMapsTo(const DynamicAutomata::StateSetToState & oldToNew,
		DynamicAutomata::State *oldState, DynamicAutomata::State *newState) {
	
	for (DynamicAutomata::StateSetToState::const_iterator it = oldToNew.begin();
			it != oldToNew.end(); ++it) {
		
		if (it->first.find(oldState) != it->first.end()) {
			if (it->second == newState) return true;
		}
	}
	
	return false;
}
