#include "parser/GrammarLoaderProceduralLoader.h"

#include "parser/Grammar.h"
#include "parser/Regex.h"
#include "parser/ScannerGrammar.h"

/*****************************************************************************
 * Scanner Tokens
 *****************************************************************************/ 
#define SCANNER_COMMENT "SCANNER_COMMENT"
#define SCANNER_IGNORE "SCANNER_IGNORE"
#define SCANNER_REDUCE "SCANNER_REDUCE"
#define SCANNER_TOKEN_ID "SCANNER_TOKEN_ID"
#define SCANNER_REGEX "SCANNER_REGEX"
#define SCANNER_RULE_END "SCANNER_RULE_END"
#define SCANNER_WHITESPACE "SCANNER_WHITE_SPACE"

/*****************************************************************************
 * Parser Tokens
 *****************************************************************************/
#define PARSER_COMMENT "PARSER_COMMENT"
#define PARSER_NON_TERMINAL "PARSER_NON_TERMINAL"
#define PARSER_REDUCE "PARSER_REDUCE"
#define PARSER_RULE_END "PARSER_RULE_END"
#define PARSER_OR "PARSER_OR"
#define PARSER_TOKEN "PARSER_TOKEN"
#define PARSER_WHITESPACE "PARSER_WHITESPACE"

GrammarLoaderProceduralLoader::GrammarLoaderProceduralLoader() {
	Grammar *scannerGrammarGrammar = createScannerGrammarGrammar();
	Grammar *parserGrammarGrammar = createParserGrammarGrammar();
	
	ScannerGrammar *sGrammar = scannerGrammarGrammar->getScannerGrammar();
	ScannerGrammar *pGrammar = parserGrammarGrammar->getScannerGrammar();
	
	scannerScannerAutomata = sGrammar->compile(scannerGrammarGrammar);
	parserScannerAutomata = pGrammar->compile(parserGrammarGrammar);
	
	delete(sGrammar);
	delete(scannerGrammarGrammar);
	delete(pGrammar);
	delete(parserGrammarGrammar);
}

GrammarLoaderProceduralLoader::~GrammarLoaderProceduralLoader() {}

const Pointer<ScannerAutomata> & GrammarLoaderProceduralLoader::getScannerScannerAutomata() const {
	return scannerScannerAutomata;
}

const Pointer<ScannerAutomata> & GrammarLoaderProceduralLoader::getParserScannerAutomata() const {
	return parserScannerAutomata;
}

void GrammarLoaderProceduralLoader::createToken(Grammar *grammar, const std::string & name,
		TokenTypeID *id, const char *pattern, bool ignored) {
	
	ScannerGrammar *scannerGrammar = grammar->getScannerGrammar();
	
	TokenType *tokenType = new TokenType(name, new Regex(pattern));
	scannerGrammar->addTokenType(tokenType);
	*id = grammar->getOrCreateTokenId(name);
	
	if (ignored) tokenType->setIgnored(true);
}

// ignore <COMMENT> ::= "(/\*([^\*]|(\*+[^\*/]))*\*+/)|(//[^\n]*)";
/* <IGNORE> ::= "ignore";
 * <REDUCE> ::= "::=";
 * <TOKEN_ID> ::= "<\w(\w|\d)*>";
 * <REGEX> ::= "\"([^\"\\]|(\\.))*\"";
 * <RULE_END> ::= ";";
 * 
 * ignore <WHITESPACE> ::= "\s+";
 */
Grammar *GrammarLoaderProceduralLoader::createScannerGrammarGrammar() {
	Grammar *grammar = new Grammar();
	grammar->setScannerGrammar(new ScannerGrammar());
	
	createToken(grammar, SCANNER_COMMENT, &scannerCommentId,
			"(/\\*([^\\*]|(\\*+[^\\*/]))*\\*+/)|(//[^\n]*)", true);
	
	createToken(grammar, SCANNER_IGNORE, &scannerIgnoreId, "ignore");
	createToken(grammar, SCANNER_REDUCE, &scannerReduceId, "::=");
	createToken(grammar, SCANNER_TOKEN_ID, &scannerTokenIdId, "<\\w(\\w|\\d)*>");
	createToken(grammar, SCANNER_REGEX, &scannerRegexId, "\"([^\"\\\\]|(\\\\.))*\"");
	createToken(grammar, SCANNER_RULE_END, &scannerRuleEndId, ";");
	
	createToken(grammar, SCANNER_WHITESPACE, &scannerWhiteSpaceId, "\\s+", true);
	
	return grammar;
}

// ignore <COMMENT> ::= "(/\*([^\*]|(\*+[^\*/]))*\*+/)|(//[^\n]*)";
/* <NON_TERMINAL> ::= "<\w(\w|\d)*>";
 * <REDUCE> ::= "::=";
 * <RULE_END> ::= ";";
 * <OR> ::= "\|";
 * <TOKEN> ::= "(\w|_)(\w|\d|_)*";
 * 
 * ignore <WHITESPACE> ::= "\s+";
 */
Grammar *GrammarLoaderProceduralLoader::createParserGrammarGrammar() {
	Grammar *grammar = new Grammar();
	grammar->setScannerGrammar(new ScannerGrammar());
	
	createToken(grammar, PARSER_COMMENT, &parserCommentId,
			"(/\\*([^\\*]|(\\*+[^\\*/]))*\\*+/)|(//[^\n]*)", true);
	
	createToken(grammar, PARSER_NON_TERMINAL, &parserNonTerminalId, "<\\w(\\w|\\d)*>");
	createToken(grammar, PARSER_REDUCE, &parserReduceId, "::=");
	createToken(grammar, PARSER_RULE_END, &parserRuleEndId, ";");
	createToken(grammar, PARSER_OR, &parserOrId, "\\|");
	
	createToken(grammar, PARSER_TOKEN, &parserTokenId, "(\\w|_)(\\w|\\d|_)*");
	
	createToken(grammar, PARSER_WHITESPACE, &parserWhitespaceId, "\\s+", true);
	
	return grammar;
}
