#include "parser/GrammarLoaderBufferLoader.h"

#include "parser/ParserLoader.h"
#include "GrammarLoaderParserBuffer.h"
#include "GrammarLoaderScannerBuffer.h"

GrammarLoaderBufferLoader::GrammarLoaderBufferLoader() {
	scannerScannerAutomata = ParserLoader::bufferToAutomata(scanner_loader_buffer_scanner);
	parserScannerAutomata = ParserLoader::bufferToAutomata(parser_loader_buffer_scanner);
	
	setupScannerTokens();
	setupParserTokens();
}

GrammarLoaderBufferLoader::~GrammarLoaderBufferLoader() {}

const Pointer<ScannerAutomata> & GrammarLoaderBufferLoader::getScannerScannerAutomata() const {
	return scannerScannerAutomata;
}

const Pointer<ScannerAutomata> & GrammarLoaderBufferLoader::getParserScannerAutomata() const {
	return parserScannerAutomata;
}

void GrammarLoaderBufferLoader::setupScannerTokens() {
	scannerCommentId = GRAMMARLOADERSCANNERBUFFER_TOKEN_COMMENT;
	scannerIgnoreId = GRAMMARLOADERSCANNERBUFFER_TOKEN_IGNORE;
	scannerReduceId = GRAMMARLOADERSCANNERBUFFER_TOKEN_REDUCE;
	scannerTokenIdId = GRAMMARLOADERSCANNERBUFFER_TOKEN_TOKEN_ID;
	scannerRegexId = GRAMMARLOADERSCANNERBUFFER_TOKEN_REGEX;
	scannerRuleEndId = GRAMMARLOADERSCANNERBUFFER_TOKEN_RULE_END;
	scannerWhiteSpaceId = GRAMMARLOADERSCANNERBUFFER_TOKEN_WHITESPACE;
}

void GrammarLoaderBufferLoader::setupParserTokens() {
	parserCommentId = GRAMMARLOADERPARSERBUFFER_TOKEN_COMMENT;
	parserNonTerminalId = GRAMMARLOADERPARSERBUFFER_TOKEN_NON_TERMINAL;
	parserReduceId = GRAMMARLOADERPARSERBUFFER_TOKEN_REDUCE;
	parserRuleEndId = GRAMMARLOADERPARSERBUFFER_TOKEN_RULE_END;
	parserOrId = GRAMMARLOADERPARSERBUFFER_TOKEN_OR;
	parserTokenId = GRAMMARLOADERPARSERBUFFER_TOKEN_TOKEN;
	parserWhitespaceId = GRAMMARLOADERPARSERBUFFER_TOKEN_WHITESPACE;
}
