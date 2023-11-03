#include "parser/InputLocation.h"

#include <cstdio>
#include <cstring>

InputLocation::InputLocation() : name("input"), line(0), column(0) {}

InputLocation::InputLocation(const std::string & n) : name("input"), line(0), column(0) {}

InputLocation::InputLocation(const std::string & n, unsigned int l,
		unsigned int c) : name(n), line(l), column(c) {}

const std::string & InputLocation::getName() const {
	return name;
}

void InputLocation::setName(const std::string & n) {
	name = n;
}

unsigned int InputLocation::getLine() const {
	return line;
}

void InputLocation::setLine(unsigned int l) {
	line = l;
}

unsigned int InputLocation::getColumn() const {
	return column;
}

void InputLocation::setColumn(unsigned int c) {
	column = c;
}

InputLocation & InputLocation::operator=(const InputLocation & location) {
	name = location.name;
	line = location.line;
	column = location.column;
	
	return *this;
}

std::ostream & operator<<(std::ostream & stream, const InputLocation & location) {
	char lineBuf[32];
	sprintf(lineBuf, "%u", location.line);
	
	stream << location.name << ":" << lineBuf;
	
	return stream;
}
