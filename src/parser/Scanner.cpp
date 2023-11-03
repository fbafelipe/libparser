#include "parser/Scanner.h"

#include "parser/Input.h"

#include <cassert>

Scanner::Scanner(const Pointer<ScannerAutomata> & a) : automata(a), input(NULL) {
	assert(automata);
}

Scanner::Scanner(const Pointer<ScannerAutomata> & a, Input *in) : automata(a), input(in) {
	assert(automata);
}

Scanner::~Scanner() {
	delete(input);
}

ParsingTree::Token *Scanner::nextToken() {
	assert(input);
	return automata->nextToken(input);
}

Input *Scanner::getInput() const {
	return input;
}

void Scanner::setInput(Input *in) {
	input = in;
}

const Pointer<ScannerAutomata> & Scanner::getScannerAutomata() const {
	return automata;
}
