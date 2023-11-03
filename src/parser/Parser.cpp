#include "parser/Parser.h"

#include "parser/ParserTable.h"
#include "parser/Scanner.h"

#include <cassert>

Parser::Parser(const Pointer<ParserTable> & table, Scanner *scan) :
		parserTable(table), scanner(scan), parserAction(NULL) {
	assert(parserTable);
	assert(scanner);
}

Parser::~Parser() {
	delete(scanner);
}

void Parser::setParserAction(ParserAction *action) {
	parserAction = action;
}

ParserAction *Parser::getParserAction() const {
	return parserAction;
}

ParsingTree::Node *Parser::parse() {
	assert(parserTable);
	return parserTable->parse(scanner, parserAction);
}

ParsingTree::Node *Parser::parseWithErrorRecovery(ParserErrorList & errorList) {
	assert(parserTable);
	return parserTable->parseWithErrorRecovery(scanner, parserAction, errorList);
}

Scanner *Parser::getScanner() const {
	return scanner;
}

const Pointer<ParserTable> & Parser::getParserTable() const {
	return parserTable;
}
