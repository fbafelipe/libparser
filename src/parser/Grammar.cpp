#include "parser/Grammar.h"

Grammar::Grammar() : scannerGrammar(NULL), parserGrammar(NULL),
	nextTokenId(0), nextNonTerminalId(0) {}

Grammar::~Grammar() {}

ScannerGrammar *Grammar::getScannerGrammar() const {
	return scannerGrammar;
}

void Grammar::setScannerGrammar(ScannerGrammar *grammar) {
	scannerGrammar = grammar;
}

ParserGrammar *Grammar::getParserGrammar() const {
	return parserGrammar;
}

void Grammar::setParserGrammar(ParserGrammar *grammar) {
	parserGrammar = grammar;
}

TokenTypeID Grammar::getOrCreateTokenId(const std::string & tokenName) {
	TokenMap::iterator it = tokenMap.find(tokenName);
	if (it != tokenMap.end()) return it->second;
	
	// allocate an id to this token
	TokenTypeID id = nextTokenId++;
	tokenMap[tokenName] = id;
	return id;
}

int Grammar::getTokenId(const std::string & tokenName) {
	TokenMap::iterator it = tokenMap.find(tokenName);
	if (it != tokenMap.end()) return it->second;
	
	return -1;
}

const Grammar::TokenMap & Grammar::getTokenIds() const {
	return tokenMap;
}

bool Grammar::hasTokenName(const std::string & tokenName) const {
	TokenMap::const_iterator it = tokenMap.find(tokenName);
	return it != tokenMap.end();
}

unsigned int Grammar::getTokenTypeCount() const {
	return nextTokenId;
}

ParsingTree::NonTerminalID Grammar::getNonTerminalId(
		const std::string & nonTerminalName) {
	
	NonTerminalMap::iterator it = nonTerminalMap.find(nonTerminalName);
	if (it == nonTerminalMap.end()) {
		throw ParserError(std::string("<") + nonTerminalName + "> not defined.");
	}
	
	return it->second;
}

ParsingTree::NonTerminalID Grammar::createNonTerminalId(
		const std::string & nonTerminalName) {
	
	if (nonTerminalMap.find(nonTerminalName) != nonTerminalMap.end()) {
		throw ParserError(nonTerminalName + " already defined.");
	}
	
	// allocate an id to this non terminal
	ParsingTree::NonTerminalID id = nextNonTerminalId++;
	nonTerminalMap[nonTerminalName] = id;
	return id;
}

const Grammar::NonTerminalMap & Grammar::getNonTermonalIds() const {
	return nonTerminalMap;
}

bool Grammar::hasNonTerminalName(const std::string & nonTerminalName) const {
	NonTerminalMap::const_iterator it = nonTerminalMap.find(nonTerminalName);
	return it != nonTerminalMap.end();
}

unsigned int Grammar::getNonTerminalTypeCount() const {
	return nextNonTerminalId;
}

std::string Grammar::getTokenName(TokenTypeID id) const {
	for (TokenMap::const_iterator it = tokenMap.begin(); it != tokenMap.end(); ++it) {
		if (it->second == id) return it->first;
	}
	return std::string("?");
}

std::string Grammar::getNonTerminalName(ParsingTree::NonTerminalID id) const {
	for (NonTerminalMap::const_iterator it = nonTerminalMap.begin(); it != nonTerminalMap.end(); ++it) {
		if (it->second == id) return it->first;
	}
	return std::string("?");
}
