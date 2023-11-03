#include "parser/MemoryInput.h"

#include <cassert>
#include <cstring>

MemoryInput::MemoryInput(const char *buf, unsigned int size) : bufSize(size) {
	buffer = new char[bufSize];
	memcpy(buffer, buf, bufSize);
}

MemoryInput::MemoryInput(const std::string & str) {
	unsigned int len = str.length();
	buffer = new char[len];
	memcpy(buffer, str.c_str(), len);
	bufSize = len;
}

MemoryInput::MemoryInput(const char *buf, unsigned int size, const std::string & inputName) : Input(inputName), bufSize(size) {
	buffer = new char[bufSize];
	memcpy(buffer, buf, bufSize);
}

MemoryInput::MemoryInput(const std::string & str, const std::string & inputName) : Input(inputName) {
	unsigned int len = str.length();
	buffer = new char[len];
	memcpy(buffer, str.c_str(), len);
	bufSize = len;
}

MemoryInput::~MemoryInput() {
	delete[](buffer);
}

void MemoryInput::setInputPos(unsigned int pos) {
	assert(pos <= bufSize);
	inputPos = pos;
}

char MemoryInput::readChar() {
	assert(inputPos <= bufSize);
	
	// assert that there are no '\0' in the middle of the buffer
	assert(inputPos == bufSize || (inputPos < bufSize && buffer[inputPos]));
	
	if (inputPos < bufSize) return buffer[inputPos++];
	return '\0';
}

void MemoryInput::sanityCheck() const {
	Input::sanityCheck();
	assert(inputPos <= bufSize);
}
