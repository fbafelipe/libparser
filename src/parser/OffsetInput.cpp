#include "parser/OffsetInput.h"

#include <cassert>

OffsetInput::OffsetInput(Input *in, int off) : input(in), offset(off), renameInput(false) {
	assert(input);
}

OffsetInput::OffsetInput(Input *in, int off, const std::string & newName) : Input(newName),
		input(in), offset(off), renameInput(true) {
	
	assert(input);
}

OffsetInput::~OffsetInput() {
	delete(input);
}

void OffsetInput::setRenameInput(bool rename) {
	renameInput = rename;
}

void OffsetInput::resetInput() {
	input->resetInput();
}

InputLocation OffsetInput::getCurrentLocation() const {
	InputLocation loc = input->getCurrentLocation();
	return InputLocation(getInputName(), loc.getLine() + offset, loc.getColumn());
}

const std::string & OffsetInput::getInputName() const {
	return renameInput ? inputName : input->getInputName();
}

unsigned int OffsetInput::getInputLine() const {
	return input->getInputLine() + offset;
}

unsigned int OffsetInput::getInputPos() const {
	return input->getInputPos();
}

std::string OffsetInput::getCurrentLine() {
	return input->getCurrentLine();
}

unsigned int OffsetInput::getCurrentLinePos() const {
	return input->getCurrentLinePos();
}

void OffsetInput::markPosition() {
	input->markPosition();
}

void OffsetInput::rollback(unsigned int usedChars) {
	input->rollback(usedChars);
}

void OffsetInput::setInputPos(unsigned int pos) {
	input->setInputPos(pos);
}

char OffsetInput::readChar() {
	return input->nextChar();
}
