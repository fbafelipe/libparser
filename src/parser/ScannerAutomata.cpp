#include "parser/ScannerAutomata.h"

#include "parser/Automata.h"
#include "parser/Input.h"
#include "parser/InputLocation.h"

#include <cstring>

ScannerAutomata::ScannerAutomata(Automata *a) : automata(a) {
	unsigned int numStates = automata->getNumStates();
	stateTokenTypeId = new int[numStates];
	for (unsigned int i = 0; i < numStates; ++i) stateTokenTypeId[i] = -1;
}

ScannerAutomata::~ScannerAutomata() {
	delete(automata);
	delete[](stateTokenTypeId);
}

Automata *ScannerAutomata::getAutomata() const {
	return automata;
}

void ScannerAutomata::addIgnoredToken(TokenTypeID id) {
	ignoredTokens.insert(id);
}

const ScannerAutomata::TokenTypeIDSet & ScannerAutomata::getIgnoredTokens() const {
	return ignoredTokens;
}

int ScannerAutomata::getStateTokenTypeId(unsigned int state) const {
	assert(state < automata->getNumStates());
	return stateTokenTypeId[state];
}

void ScannerAutomata::setStateTokenTypeId(unsigned int state, TokenTypeID tokenId) {
	assert(state < automata->getNumStates());
	assert(automata->isFinalState(state));
	stateTokenTypeId[state] = tokenId;
}

ParsingTree::Token *ScannerAutomata::nextToken(Input *input) const {
	int reconizedToken;
	std::string buffer;
	unsigned int tokenUsedChars;
	
	InputLocation inputLocation;
	
	do {
		inputLocation = input->getCurrentLocation();
		
		int state = 0;
		
		buffer.clear();
		
		tokenUsedChars = 0;
		reconizedToken = -1;
		
		unsigned int usedChars = 0;
		char lastChar = '\0';
		
		input->markPosition();
		
		while (state != -1) {
			lastChar = input->nextChar();
			++usedChars;
			buffer.push_back(lastChar);
			
			state = automata->getTransition(state, lastChar);
			if (state == -1) break;
			
			if (automata->isFinalState(state)) {
				int token = stateTokenTypeId[state];
				assert(token != -1);
				
				reconizedToken = token;
				tokenUsedChars = usedChars;
			}
		}
		
		if (lastChar == '\0' && usedChars == 1) {
			// end of file reached without reading any character
			return NULL;
		}
		
		if (reconizedToken == -1) {
			throw ParserError(input, std::string("Unexpected character \'") + lastChar + "\'.");
		}
		
		assert(tokenUsedChars > 0);
		
		if (usedChars != tokenUsedChars) input->rollback(tokenUsedChars);
	} while (ignoredTokens.find(reconizedToken) != ignoredTokens.end());
	
	// use the usedChars first characters from buffer
	assert(buffer.size() >= tokenUsedChars);
	std::string tokenName;
	for (unsigned int i = 0; i < tokenUsedChars; ++i) {
		tokenName.push_back(buffer[i]);
	}
	
	return new ParsingTree::Token(reconizedToken, tokenName, inputLocation);
}

void ScannerAutomata::sanityCheck() const {
	automata->sanityCheck();
}

bool ScannerAutomata::operator==(const ScannerAutomata & automata) const {
	if (ignoredTokens != automata.ignoredTokens) return false;
	if (this->automata->getNumStates() != automata.automata->getNumStates()) return false;
	
	Automata::StateToState thisToOther;
	if (!this->automata->compareTo(automata.automata, thisToOther)) return false;
	
	unsigned int numStates = this->automata->getNumStates();
	for (unsigned int i = 0; i < numStates; ++i) {
		if (stateTokenTypeId[i] != automata.stateTokenTypeId[thisToOther[i]]) return false;
	}
	
	return true;
}

bool ScannerAutomata::operator!=(const ScannerAutomata & automata) const {
	return !(*this == automata);
}

std::ostream & operator<<(std::ostream & stream, const ScannerAutomata & automata) {
	stream << "Ignored:";
	for (ScannerAutomata::TokenTypeIDSet::const_iterator it = automata.ignoredTokens.begin();
			it != automata.ignoredTokens.end(); ++it) {
		stream << " " << *it;
	}
	stream << "\n";
	
	stream << *automata.automata;
	
	stream << "Reconized:\n\t";
	
	unsigned int numStates = automata.automata->getNumStates();
	for (unsigned int i = 0; i < numStates; ++i) {
		stream << "\t" << automata.stateTokenTypeId[i];
	}
	stream << "\n";
	
	return stream;
}
