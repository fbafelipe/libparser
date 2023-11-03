#include "parser/MultInput.h"

#include <cassert>

MultInput::MultInput() : inputOffset(0) {}

MultInput::~MultInput() {}

InputLocation MultInput::getCurrentLocation() const {
	return currentInput()->getCurrentLocation();
}

const std::string & MultInput::getInputName() const {
	return currentInput()->getInputName();
}

unsigned int MultInput::getInputLine() const {
	return currentInput()->getInputLine();
}

unsigned int MultInput::getInputPos() const {
	return currentInput()->getInputPos();
}

std::string MultInput::getCurrentLine() {
	return currentInput()->getCurrentLine();
}

unsigned int MultInput::getCurrentLinePos() const {
	return currentInput()->getCurrentLinePos();
}

void MultInput::markPosition() {
	Input::markPosition();
	currentInput()->markPosition();
}

void MultInput::rollback(unsigned int usedChars) {
	Input *current = currentInput();
	
	// WARNING: rollback between inputs won't work!
	assert(inputPos - markedPos <= current->getInputPos());
	
	if (markedPos < inputOffset) {
		// the markedPos was in another input
		// calculate the used characters from this input
		if (usedChars >= inputOffset - markedPos) usedChars -= inputOffset - markedPos;
		else usedChars = 0;
		
		inputPos = inputOffset;
	}
	
	current->rollback(usedChars);
}

void MultInput::setInputPos(unsigned int pos) {
	// can't set the position before the current file
	assert(pos >= inputOffset);
	
	currentInput()->setInputPos(pos - inputOffset);
	inputPos = pos;
}

char MultInput::readChar() {
	char c;
	
	while ((c = currentInput()->nextChar()) == '\0') {
		if (!inputFinished()) break;
	}
	
	return c;
}
