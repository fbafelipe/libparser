#include "parser/RegexBufferGrammar.h"

#include "parser/ParserLoader.h"
#include "RegexLoaderBuffer.h"

RegexBufferGrammar::RegexBufferGrammar() {
	scannerAutomata = ParserLoader::bufferToAutomata(regex_loader_buffer_scanner);
	parserTable = ParserLoader::bufferToTable(regex_loader_buffer_parser);
	
	setupTokens();
	setupNonTerminals();
}

RegexBufferGrammar::~RegexBufferGrammar() {}


void RegexBufferGrammar::setupTokens() {
	tokenClosureId = REGEXLOADERBUFFER_TOKEN_CLOSURE;
	tokenComposedEndId = REGEXLOADERBUFFER_TOKEN_COMPOSED_END;
	tokenComposedStartId = REGEXLOADERBUFFER_TOKEN_COMPOSED_START;
	tokenNotId = REGEXLOADERBUFFER_TOKEN_NOT;
	tokenOneClosureId = REGEXLOADERBUFFER_TOKEN_ONE_CLOSURE;
	tokenOptionalId = REGEXLOADERBUFFER_TOKEN_OPTIONAL;
	tokenOrId = REGEXLOADERBUFFER_TOKEN_OR;
	tokenSymbolId = REGEXLOADERBUFFER_TOKEN_SYMBOL;
	tokenSymbolListEndId = REGEXLOADERBUFFER_TOKEN_SYMBOL_LIST_END;
	tokenSymbolListStartId = REGEXLOADERBUFFER_TOKEN_SYMBOL_LIST_START;
}

void RegexBufferGrammar::setupNonTerminals() {
	expressionId = REGEXLOADERBUFFER_NONTERMINAL_EXPRESSION;
	factorId = REGEXLOADERBUFFER_NONTERMINAL_FACTOR;
	regexId = REGEXLOADERBUFFER_NONTERMINAL_REGEX;
	regexOrId = REGEXLOADERBUFFER_NONTERMINAL_REGEX_OR;
	symbolListId = REGEXLOADERBUFFER_NONTERMINAL_SYMBOL_LIST;
	symbolSetId = REGEXLOADERBUFFER_NONTERMINAL_SYMBOL_SET;
}
