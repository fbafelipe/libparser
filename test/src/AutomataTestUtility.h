#ifndef AUTOMATA_TEST_UTILITY_H
#define AUTOMATA_TEST_UTILITY_H

#include "parser/DynamicAutomata.h"

#include <string>

DynamicAutomata *createAutomata(const std::string & str);

bool stateMapsTo(const DynamicAutomata::StateSetToState & oldToNew,
		DynamicAutomata::State *oldState, DynamicAutomata::State *newState);

#endif
