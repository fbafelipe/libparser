#include "parser/Input.h"

Input::Input(const std::string & name) : inputName(name), inputPos(0), inputLine(1),
		lastLinePos(0), markedPos(0), markedLine(1), markedLastLinePos(0) {}

Input::Input() : inputName("input"), inputPos(0), inputLine(1), lastLinePos(0),
		markedPos(0), markedLine(1), markedLastLinePos(0) {}

Input::~Input() {}

InputLocation Input::getCurrentLocation() const {
	return InputLocation(getInputName(), getInputLine(), getCurrentLinePos());
}

void Input::resetInput() {
	setInputPos(0);
	inputLine = 1;
	lastLinePos = 0;
	
	markedPos = 0;
	markedLine = 1;
	markedLastLinePos = 0;
}

void Input::markPosition() {
	markedPos = inputPos;
	markedLine = inputLine;
	markedLastLinePos = lastLinePos;
}

void Input::rollback(unsigned int usedChars) {
	assert(markedPos + usedChars <= inputPos);
	
	setInputPos(markedPos);
	inputLine = markedLine;
	lastLinePos = markedLastLinePos;
	
	// now we need to advance usedChar chars in the input
	// can't just sum to inputPos becouse inputLine wouldn't be updated
	for (unsigned int i = 0; i < usedChars; ++i) nextChar();
}

const std::string & Input::getInputName() const {
	return inputName;
}

void Input::setInputName(const std::string & name) {
	inputName = name;
}

unsigned int Input::getInputLine() const {
	return inputLine;
}

unsigned int Input::getInputPos() const {
	return inputPos;
}

std::string Input::getCurrentLine() {
	// save mark
	unsigned int mPos = markedPos;
	unsigned int mLine = markedLine;
	unsigned int mLastLine = markedLastLinePos;
	unsigned int iPos = inputPos;
	unsigned int iLine = inputLine;
	unsigned int iLastLine = lastLinePos;
	
	setInputPos(lastLinePos);
	
	std::string line;
	
	char c;
	do {
		c = nextChar();
		if (c && c != '\n') line.push_back(c);
	} while (c && c != '\n');
	
	// restore mark
	markedPos = mPos;
	markedLine = mLine;
	markedLastLinePos = mLastLine;
	
	setInputPos(iPos);
	inputLine = iLine;
	lastLinePos = iLastLine;
	
	return line;
}

unsigned int Input::getCurrentLinePos() const {
	if (inputPos < lastLinePos) return 0;
	return inputPos - lastLinePos;
}

void Input::dumpInput(std::ostream & stream) {
	char c;
	while ((c = nextChar())) stream.put(c);
}

void Input::sanityCheck() const {
	assert(markedPos <= inputPos);
	assert(markedLine <= inputLine);
	assert(markedLastLinePos <= lastLinePos);
}
