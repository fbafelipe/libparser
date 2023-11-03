#include "parser/ScannerGrammar.h"

#include "parser/Automata.h"
#include "parser/AutomataOperations.h"
#include "parser/Grammar.h"
#include "parser/Regex.h"
#include "parser/ScannerAutomata.h"

#include <cassert>

/*****************************************************************************
 * ScannerGrammar
 *****************************************************************************/
ScannerGrammar::ScannerGrammar() {}

ScannerGrammar::~ScannerGrammar() {
	for (TokenTypeList::iterator it = grammar.begin(); it != grammar.end(); ++it) {
		delete(*it);
	}
}

void ScannerGrammar::addTokenType(TokenType *tokenType) {
	grammar.push_back(tokenType);
}

ScannerAutomata *ScannerGrammar::compile(Grammar *g) const {
	assert(!grammar.empty());
	
	DynamicAutomataVector dAutomataVector;
	dAutomataVector.reserve(grammar.size());
	
	// maps the state to the token it reconize
	StateToToken stateToken;
	
	for (TokenTypeList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
		DynamicAutomata *automata = new DynamicAutomata((*it)->getRegex()->getAutomata());
		dAutomataVector.push_back(automata);
		
		// mark the final states of this automata to reconize this token
		StateSet finalStates;
		automata->getFinalStates(finalStates);
		
		for (StateSet::const_iterator it2 = finalStates.begin(); it2 != finalStates.end(); ++it2) {
			stateToken[*it2] = g->getOrCreateTokenId((*it)->getTypeName());
		}
	}
	
	DynamicAutomata *resultAutomata = mergeAllAutomatas(dAutomataVector, stateToken);
	minimizeAutomata(resultAutomata, stateToken);
	ScannerAutomata *scannerAutomata = convertAutomata(resultAutomata, stateToken);
	delete(resultAutomata);
	
	// setup the ignored tokens
	for (TokenTypeList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
		if ((*it)->isIgnored()) {
			scannerAutomata->addIgnoredToken(g->getOrCreateTokenId((*it)->getTypeName()));
		}
	}
	
	return scannerAutomata;
}

/*****************************************************************************
 * Auxiliar methods
 *****************************************************************************/
ScannerAutomata *ScannerGrammar::convertAutomata(DynamicAutomata *dAutomata,
		const StateToToken & stateToken) {
	
	Automata *automata = dAutomata->toAutomata();
	
	StateSet finalStates;
	dAutomata->getFinalStates(finalStates);
	
	DynamicAutomata::StateIndex stateIndex;
	dAutomata->getStateIndex(stateIndex);
	
	ScannerAutomata *scannerAutomata = new ScannerAutomata(automata);
	
	for (StateSet::const_iterator it = finalStates.begin(); it != finalStates.end(); ++it) {
		StateToToken::const_iterator stateTokenIt = stateToken.find(*it);
		if (stateTokenIt != stateToken.end()) {
			scannerAutomata->setStateTokenTypeId(stateIndex[*it], stateTokenIt->second);
		}
	}
	
	return scannerAutomata;
}

void ScannerGrammar::minimizeAutomata(DynamicAutomata *automata, StateToToken & stateToken) {
	StateSetToState oldToNew;
	StateList oldStates;
	
	automata->removeEpsilonTransitions();
	automata->determinize(oldToNew, oldStates);
	updateStateTokens(stateToken, oldToNew);
	
	StateToState mOldToNew;
	automata->minimizeNoFinalMerge(mOldToNew, oldStates);
	updateStateTokens(stateToken, mOldToNew);
	
	for (StateList::iterator it = oldStates.begin(); it != oldStates.end(); ++it) {
		delete(*it);
	}
}

DynamicAutomata *ScannerGrammar::mergeAllAutomatas(const DynamicAutomataVector & dAutomataVector,
		StateToToken & stateToken) {
	
	assert(!dAutomataVector.empty());
	if (dAutomataVector.size() == 1) return dAutomataVector[0];
	
	DynamicAutomataVector::const_iterator it = dAutomataVector.begin();
	DynamicAutomata *automataA = *(it++);
	DynamicAutomata *automataB = *(it++);
	
	DynamicAutomata *result = mergeAutomatas(automataA, automataB, stateToken);
	deleteAutomataAndClearStates(automataA, stateToken);
	deleteAutomataAndClearStates(automataB, stateToken);
	
	for (; it != dAutomataVector.end(); ++it) {
		automataA = result;
		automataB = *it;
		result = mergeAutomatas(automataA, automataB, stateToken);
		deleteAutomataAndClearStates(automataA, stateToken);
		deleteAutomataAndClearStates(automataB, stateToken);
	}
	
	return result;
}

DynamicAutomata *ScannerGrammar::mergeAutomatas(DynamicAutomata *automataA,
		DynamicAutomata *automataB, StateToToken & stateToken) {
	
	StateToState oldStatesAToNewStates;
	StateToState oldStatesBToNewStates;
	
	DynamicAutomata *result = AutomataOperations::automataUnion(
			automataA, oldStatesAToNewStates,
			automataB, oldStatesBToNewStates);
	
	// update the stateToken map
	updateStateTokens(stateToken, oldStatesAToNewStates);
	updateStateTokens(stateToken, oldStatesBToNewStates);
	
	return result;
}

void ScannerGrammar::deleteAutomataAndClearStates(DynamicAutomata *automata,
		StateToToken & stateToken) {
	
	const StateList & states = automata->getStates();
	for (StateList::const_iterator it = states.begin(); it != states.end(); ++it) {
		StateToToken::iterator stIt = stateToken.find(*it);
		if (stIt != stateToken.end()) stateToken.erase(stIt);
	}
	
	delete(automata);
}

void ScannerGrammar::updateStateTokens(StateToToken & stateToken, const StateToState & oldToNew) {
	for (StateToState::const_iterator it = oldToNew.begin(); it != oldToNew.end(); ++it) {
		updateStateToken(stateToken, it->first, it->second);
	}
}

void ScannerGrammar::updateStateTokens(StateToToken & stateToken, const StateSetToState & oldToNew) {
	for (StateSetToState::const_iterator setIt = oldToNew.begin(); setIt != oldToNew.end(); ++setIt) {
		for (StateSet::const_iterator it = setIt->first.begin(); it != setIt->first.end(); ++it) {
			updateStateToken(stateToken, *it, setIt->second);
		}
	}
}

void ScannerGrammar::updateStateToken(StateToToken & stateToken, State *oldState, State *newState) {
	// update only the final states
	if (!newState->isFinalState()) return;
	
	StateToToken::iterator stateTokenIt = stateToken.find(oldState);
	
	// do not update ignored tokens
	if (stateTokenIt == stateToken.end()) return;
	
	TokenTypeID id = stateTokenIt->second;
	
	StateToToken::iterator newTokenIt = stateToken.find(newState);
	if (newTokenIt == stateToken.end()) stateToken[newState] = id;
	else {
		// the other token also reconize in this state
		// keep the id with highest priority (lower number)
		if (newTokenIt->second > id) newTokenIt->second = id;
	}
}
