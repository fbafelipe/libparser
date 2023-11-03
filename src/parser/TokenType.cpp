#include "parser/TokenType.h"

#include "parser/Regex.h"

TokenType::TokenType(const std::string & name, const std::string & rexp) 
		: typeName(name), ignored(false) {
	
	regex = new Regex(rexp);
}

TokenType::TokenType(const std::string & name, Regex *rexp)
		: typeName(name), regex(rexp), ignored(false) {}

TokenType::~TokenType() {
	delete(regex);
}

bool TokenType::isIgnored() const {
	return ignored;
}

void TokenType::setIgnored(bool ig) {
	ignored = ig;
}

const std::string & TokenType::getTypeName() const {
	return typeName;
}

const Regex *TokenType::getRegex() const {
	return regex;
}
