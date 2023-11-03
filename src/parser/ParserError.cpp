#include "parser/ParserError.h"

#include "parser/Input.h"
#include "parser/InputLocation.h"

#include <cstdio>

ParserError::ParserError() : message("ParserError") {}

ParserError::ParserError(const std::string & msg) : inputName("input"),
		rawMessage(msg), message(msg), lineNum(0), lineCol(0) {}

ParserError::ParserError(Input *input, const std::string & msg) : rawMessage(msg) {
	inputName = input->getInputName();
	lineNum = input->getInputLine();
	lineCol = input->getCurrentLinePos();
	currentLine = input->getCurrentLine();
	
	init();
}

ParserError::ParserError(Input *input, unsigned int line, unsigned int col,
		const std::string & msg) : rawMessage(msg), lineNum(line), lineCol(col) {
	
	inputName = input->getInputName();
	currentLine = input->getCurrentLine();
	init();
}

ParserError::ParserError(const InputLocation & location, const std::string & msg) :
		inputName(location.getName()), rawMessage(msg),
		lineNum(location.getLine()), lineCol(location.getColumn()) {
	
	char lineNumStr[32];
	sprintf(lineNumStr, "%u", lineNum);
	
	message = inputName + ":" + lineNumStr + ": error: " + rawMessage;
}

ParserError::~ParserError() throw() {}

void ParserError::init() {
	char lineNumStr[32];
	sprintf(lineNumStr, "%u", lineNum);
	
	message = inputName + ":" + lineNumStr + ": error: ";
	message += rawMessage + "\n" + currentLine + "\n";
	
	for (int i = 0; i < (int)lineCol - 1; ++i) {
		if (currentLine[i] == '\t') {
			for (unsigned int j = 0; j < 8; ++j) message.push_back('-');
		}
		else message.push_back('-');
	}
	message.push_back('^');
}

const std::string & ParserError::getInputName() const {
	return inputName;
}

const std::string & ParserError::getRawMessage() const {
	return rawMessage;
}

const std::string & ParserError::getMessage() const {
	return message;
}

const char *ParserError::what() const throw() {
	return message.c_str();
}

unsigned int ParserError::getLineNumber() const {
	return lineNum;
}

unsigned int ParserError::getLineColumn() const {
	return lineCol;
}
