#include "parser/FileInput.h"

#include "parser/IOError.h"

#include <cassert>
#include <cstdlib>
#include <exception>

#define BUFFER_SIZE 8192

// when testing some times it's useful to reduce the size of the buffer
//#define BUFFER_SIZE 64

FileInput::FileInput(const std::string & file) : Input(file) {
	filePointer = fopen(file.c_str(), "r");
	if (!filePointer) throw IOError(std::string("Cannot read file ") + file);
	
	// NOTE: must create the buffer after opening the file
	// or an if the fine is not found we would need to delete the buffer
	buffer = new char[BUFFER_SIZE];
	
	setInputPos(0);
}

FileInput::FileInput(const char *file) : Input(file) {
	filePointer = fopen(file, "r");
	if (!filePointer) throw IOError(std::string("Cannot read file ") + file);
	
	// NOTE: must create the buffer after opening the file
	// or an if the fine is not found we would need to delete the buffer
	buffer = new char[BUFFER_SIZE];
	
	setInputPos(0);
}

FileInput::FileInput(FILE *fp) : filePointer(fp) {
	buffer = new char[BUFFER_SIZE];
	
	setInputPos(0);
}

FileInput::~FileInput() {
	fclose(filePointer);
	delete[](buffer);
}

char FileInput::readChar() {
	// assert that there are no '\0' in the middle of the buffer
	assert(bufPos == bufSize || (bufPos < bufSize && buffer[bufPos]));
	
	if (bufPos >= bufSize) {
		setInputPos(inputPos);
		if (bufSize == 0) return '\0';
	}
	
	assert(bufPos < bufSize);
	
	++inputPos;
	return buffer[bufPos++];
}

void FileInput::setInputPos(unsigned int pos) {
	inputPos = pos;
	fseek(filePointer, inputPos, SEEK_SET);
	bufSize = fread(buffer, sizeof(char), BUFFER_SIZE, filePointer);
	bufPos = 0;
}

void FileInput::sanityCheck() const {
	Input::sanityCheck();
	
	unsigned int realPos = bufPos + ftell(filePointer) - bufSize;
	
	if (realPos != inputPos) abort();
}
