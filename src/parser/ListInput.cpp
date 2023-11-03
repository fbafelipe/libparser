#include "parser/ListInput.h"

#include <cassert>

ListInput::ListInput() : current(0) {}

ListInput::~ListInput() {
	for (InputList::const_iterator it = inputList.begin(); it != inputList.end(); ++it) {
		delete(*it);
	}
}

void ListInput::resetInput() {
	for (int i = current; i >= 0; --i) inputList[i]->resetInput();
	current = 0;
}

void ListInput::addInput(Input *input) {
	inputList.push_back(input);
}

Input *ListInput::currentInput() const {
	assert(current < inputList.size());
	return inputList[current];
}

bool ListInput::inputFinished() {
	Input *currentIn = currentInput();

	if (current + 1 >= inputList.size()) return false;
	++current;
	
	assert(current < inputList.size());

	inputOffset += currentIn->getInputPos();
	
	assert(currentInput());
	assert(currentInput()->getInputPos() == 0);
	
	return true;
}
