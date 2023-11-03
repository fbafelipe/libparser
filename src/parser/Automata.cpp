#include "parser/Automata.h"

#include "parser/DynamicAutomata.h"
#include "parser/ParserDefs.h"

#include <cassert>
#include <cstring>

#define CHECK_STATE(s) assert((s) >= -1  && (s) < (int)numStates)

Automata::Automata(unsigned int size) : numStates(size) {
	assert(size > 0);
	
	transitionTable = new int *[ALPHABET_SIZE];
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) {
		transitionTable[i] = new int[numStates];
		for (unsigned int j = 0; j < numStates; ++j) transitionTable[i][j] = -1;
	}
	
	createStates();
}

Automata::Automata(const int * const *transitions, unsigned int size) : numStates(size) {
	assert(size > 0);
	
	transitionTable = new int *[ALPHABET_SIZE];
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) {
		transitionTable[i] = new int[numStates];
		for (unsigned int j = 0; j < numStates; ++j) transitionTable[i][j] = transitions[i][j];
	}
	
	createStates();
}

Automata::~Automata() {
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) delete[](transitionTable[i]);
	delete[](transitionTable);
	delete[](finalStates);
}

void Automata::createStates() {
	finalStates = new bool[numStates];
	memset(finalStates, 0, sizeof(bool) * numStates);
}

void Automata::setTransitionTable(const int * const *transitions) {
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) {
		for (unsigned int j = 0; j < numStates; ++j) {
			CHECK_STATE(transitions[i][j]);
			transitionTable[i][j] = transitions[i][j];
		}
	}
}

const int * const *Automata::getTransitionTable() const {
	return transitionTable;
}

unsigned int Automata::getNumStates() const {
	return numStates;
}

void Automata::setTransition(unsigned int state, char input, int destinationState) {
	assert(state < numStates);
	CHECK_STATE(destinationState);
	assert(input >= 0);
	
	transitionTable[(int)input][state] = destinationState;
}

bool Automata::isFinalState(unsigned int state) const {
	assert(state < numStates);
	return finalStates[state];
}

void Automata::setFinalState(unsigned int state) {
	assert(state < numStates);
	finalStates[state] = true;
}

void Automata::setFinalStates(const std::vector<unsigned int> & stateList) {
	for (std::vector<unsigned int>::const_iterator it = stateList.begin(); it != stateList.end(); ++it) {
		setFinalState(*it);
	}
}

bool Automata::accept(const char *input) const {
	int state = 0;
	
	while (*input && state != -1) {
		state = transitionTable[(unsigned int)*(input++)][state];
	}
	
	if (state == -1) return false;
	return finalStates[state];
}

void Automata::sanityCheck() const {
	assert(transitionTable);
	assert(finalStates);
	
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) {
		assert(transitionTable[i]);
		
		for (unsigned int j = 0; j < numStates; ++j) {
			CHECK_STATE(transitionTable[i][j]);
		}
	}
}

bool Automata::operator==(const DynamicAutomata & automata) const {
	return automata == *this;
}

bool Automata::operator!=(const DynamicAutomata & automata) const {
	return !(*this == automata);
}

bool Automata::operator==(const Automata & automata) const {
	DynamicAutomata *dynamicAutomataA = new DynamicAutomata(this);
	DynamicAutomata *dynamicAutomataB = new DynamicAutomata(&automata);
	
	bool result = *dynamicAutomataA == *dynamicAutomataB;
	
	delete(dynamicAutomataA);
	delete(dynamicAutomataB);
	
	return result;
}

bool Automata::operator!=(const Automata & automata) const {
	return !(*this == automata);
}

bool Automata::compareTo(const Automata *automata, StateToState & thisToOther) {
	DynamicAutomata *dynamicAutomataA = new DynamicAutomata(this);
	DynamicAutomata *dynamicAutomataB = new DynamicAutomata(automata);
	
	DynamicAutomata::StateIndex indexB;
	dynamicAutomataB->getStateIndex(indexB);
	
	DynamicAutomata::StateToState thisToOtherMap;
	bool result = dynamicAutomataA->compareTo(dynamicAutomataB, thisToOtherMap);
	
	for (unsigned int i = 0; i < numStates; ++i) {
		thisToOther[i] = indexB[thisToOtherMap[dynamicAutomataA->getState(i)]];
	}
	
	delete(dynamicAutomataA);
	delete(dynamicAutomataB);
	
	return result;
}

std::ostream & operator<<(std::ostream & stream, const Automata & automata) {
	stream << "NumStates: " << automata.numStates << "\n";
	
	// put a * on the columns of final states
	stream << "\t\t";
	for (unsigned int j = 0; j < automata.numStates; ++j) {
		if (automata.finalStates[j]) stream << "*";
		if (j < automata.numStates - 1) stream << "\t";
	}
	stream << "\n";
	
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) {
		
		if (isprint(i)) {
			char buf[2];
			std::string in;
			
			switch (i) {
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
					buf[0] = i;
					buf[1] = '\0';
					in = std::string(buf);
			}
			
			stream << i << " \'" << in << "\'\t";
		}
		else stream << i << "\t\t";
		
		for (unsigned int j = 0; j < automata.numStates; ++j) {
			if (j > 0) stream << "\t" << automata.transitionTable[i][j];
			else stream << automata.transitionTable[i][j];
			
		}
		stream << "\n";
	}
	
	return stream;
}
