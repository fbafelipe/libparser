#include "parser/RegexGrammar.h"

#include <cassert>

RegexGrammar::RegexGrammar() {
	tokenClosureId = 0;
	tokenOneClosureId = 0;
	tokenOptionalId = 0;
	tokenComposedStartId = 0;
	tokenComposedEndId = 0;
	tokenOrId = 0;
	tokenSymbolListStartId = 0;
	tokenSymbolListEndId = 0;
	tokenNotId = 0;
	tokenSymbolId = 0;
	
	// non terminals
	regexId = 0;
	regexOrId = 0;
	expressionId = 0;
	factorId = 0;
	symbolSetId = 0;
	symbolListId = 0;
}

RegexGrammar::~RegexGrammar() {}

const Pointer<ScannerAutomata> & RegexGrammar::getScannerAutomata() const {
	assert(scannerAutomata);
	return scannerAutomata;
}

const Pointer<ParserTable> & RegexGrammar::getParserTable() const {
	assert(parserTable);
	return parserTable;
}
