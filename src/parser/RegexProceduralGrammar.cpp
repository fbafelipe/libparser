#include "parser/RegexProceduralGrammar.h"

#include "parser/Automata.h"
#include "parser/Grammar.h"
#include "parser/ParserGrammar.h"
#include "parser/ParserCompileResult.h"
#include "parser/ParserTableLL1.h"
#include "parser/Regex.h"
#include "parser/ScannerGrammar.h"
#include "parser/TokenType.h"

#include <cassert>

// token names
#define TOKEN_CLOSURE "TOKEN_CLOSURE"
#define TOKEN_ONE_CLOSURE "TOKEN_ONE_CLOSURE"
#define TOKEN_OPTIONAL "TOKEN_OPTIONAL"
#define TOKEN_COMPOSED_START "TOKEN_COMPOSED_START"
#define TOKEN_COMPOSED_END "TOKEN_COMPOSED_END"
#define TOKEN_OR "TOKEN_OR"
#define TOKEN_SYMBOL_LIST_START "TOKEN_SYMBOL_LIST_START"
#define TOKEN_SYMBOL_LIST_END "TOKEN_SYMBOL_LIST_END"
#define TOKEN_NOT "TOKEN_NOT"
#define TOKEN_SYMBOL "TOKEN_SYMBOL"

RegexProceduralGrammar::RegexProceduralGrammar() {
	Grammar *grammar = createRegexGrammar();
	
	scannerAutomata = grammar->getScannerGrammar()->compile(grammar);
	
	ParserCompileResult compileResult;
	parserTable = grammar->getParserGrammar()->compileLL1(grammar, &compileResult);
	assert(compileResult.getConflictsCount() == 0);
	
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
}

RegexProceduralGrammar::~RegexProceduralGrammar() {}

void RegexProceduralGrammar::scannerGrammarAddTokenType(Grammar *grammar,
		const std::string & tokenName, TokenTypeID *id, char c) {
	
	ScannerGrammar *scannerGrammar = grammar->getScannerGrammar();
	
	TokenType *tokenType = new TokenType(tokenName, new Regex(c));
	scannerGrammar->addTokenType(tokenType);
	*id = grammar->getOrCreateTokenId(tokenName);
}

void RegexProceduralGrammar::createRegexScannerGrammar(Grammar *grammar) {
	ScannerGrammar *scannerGrammar = new ScannerGrammar();
	grammar->setScannerGrammar(scannerGrammar);
	
	scannerGrammarAddTokenType(grammar, TOKEN_CLOSURE, &tokenClosureId, '*');
	scannerGrammarAddTokenType(grammar, TOKEN_ONE_CLOSURE, &tokenOneClosureId, '+');
	scannerGrammarAddTokenType(grammar, TOKEN_OPTIONAL, &tokenOptionalId, '?');
	scannerGrammarAddTokenType(grammar, TOKEN_OR, &tokenOrId, '|');
	scannerGrammarAddTokenType(grammar, TOKEN_NOT, &tokenNotId, '^');
	scannerGrammarAddTokenType(grammar, TOKEN_COMPOSED_START, &tokenComposedStartId, '(');
	scannerGrammarAddTokenType(grammar, TOKEN_COMPOSED_END, &tokenComposedEndId, ')');
	scannerGrammarAddTokenType(grammar, TOKEN_SYMBOL_LIST_START, &tokenSymbolListStartId, '[');
	scannerGrammarAddTokenType(grammar, TOKEN_SYMBOL_LIST_END, &tokenSymbolListEndId, ']');
	
	// create a special regex for the TOKEN_SYMBOL
	// this special token accepts [^\\]|\\.
	// there is no problem in accepting characters of others tokens
	// since this token type will be the last
	Automata *automata = new Automata(3);
	for (unsigned int c = 1; c < ALPHABET_SIZE; ++c) {
		automata->setTransition(0, (char)c, 2);
		automata->setTransition(1, (char)c, 2);
	}
	automata->setTransition(0, '\\', 1);
	automata->setFinalState(2);
	Regex *symbolRegex = new Regex(automata);
	
	scannerGrammar->addTokenType(new TokenType(TOKEN_SYMBOL, symbolRegex));
	tokenSymbolId = grammar->getOrCreateTokenId(TOKEN_SYMBOL);
}

/*
 * <REGEX> ::= <EXPRESSION> <REGEX_OR>
 * 		| // epsilon
 * 		;
 * 
 * <REGEX_OR> ::= OR <REGEX>
 * 		| <REGEX>
 * 		;
 * 
 * <EXPRESSION> ::= SYMBOL <FACTOR>
 * 		| COMPOSED_START <REGEX> COMPOSED_END <FACTOR>
 * 		| SYMBOL_LIST_START <SYMBOL_SET> SYMBOL_LIST_END <FACTOR>
 * 		;
 * 
 * <FACTOR> ::= CLOSURE
 * 		| ONE_CLOSURE
 * 		| OPTIONAL
 * 		| // epsilon
 * 		;
 * 
 * <SYMBOL_SET> ::= <SYMBOL_LIST>
 * 		| NOT <SYMBOL_LIST>
 * 		;
 * 
 * <SYMBOL_LIST> ::= SYMBOL <SYMBOL_LIST>
 *		| // epsilon
 *		;
 * 
 */
void RegexProceduralGrammar::createRegexParserGrammar(Grammar *grammar) {
	ParserGrammar *parserGrammar = new ParserGrammar();
	grammar->setParserGrammar(parserGrammar);
	
	regexId = grammar->createNonTerminalId("REGEX");
	regexOrId = grammar->createNonTerminalId("REGEX_OR");
	expressionId = grammar->createNonTerminalId("EXPRESSION");
	factorId = grammar->createNonTerminalId("FATOR");
	symbolSetId = grammar->createNonTerminalId("SYMBOL_SET");
	symbolListId = grammar->createNonTerminalId("SYMBOL_LIST");
	
	createRegexParserGrammarRegex(grammar);
	createRegexParserGrammarRegexOr(grammar);
	createRegexParserGrammarExpression(grammar);
	createRegexParserGrammarFactor(grammar);
	createRegexParserGrammarSymbolSet(grammar);
	createRegexParserGrammarSymbolList(grammar);
}

/*
 * <REGEX> ::= <EXPRESSION> <REGEX_OR>
 * 		| // epsilon
 * 		;
 */
void RegexProceduralGrammar::createRegexParserGrammarRegex(Grammar *grammar) {
	ParserGrammar *parserGrammar = grammar->getParserGrammar();
	ParserGrammar::NodeTypeList nodeList;
	
	ParserGrammar::Type nonTerminal = ParserGrammar::NON_TERMINAL;
	
	// <REGEX> rule 0: <EXPRESSION> <REGEX_OR>
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, expressionId));
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, regexOrId));
	parserGrammar->addRule(new ParserGrammar::Rule(regexId, nodeList));
	nodeList.clear();
	
	// <REGEX> rule 1: epsilon
	parserGrammar->addRule(new ParserGrammar::Rule(regexId, nodeList));
}

/* 
 * <REGEX_OR> ::= OR <REGEX>
 * 		| <REGEX>
 * 		;
 */
void RegexProceduralGrammar::createRegexParserGrammarRegexOr(Grammar *grammar) {
	ParserGrammar *parserGrammar = grammar->getParserGrammar();
	ParserGrammar::NodeTypeList nodeList;
	
	ParserGrammar::Type nonTerminal = ParserGrammar::NON_TERMINAL;
	ParserGrammar::Type token = ParserGrammar::TOKEN;
	
	// <REGEX_OR> rule 0: OR <REGEX>
	nodeList.push_back(ParserGrammar::NodeType(token, tokenOrId));
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, regexId));
	parserGrammar->addRule(new ParserGrammar::Rule(regexOrId, nodeList));
	nodeList.clear();
	
	// <REGEX_OR> rule 1: <REGEX>
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, regexId));
	parserGrammar->addRule(new ParserGrammar::Rule(regexOrId, nodeList));
}

/* 
 * <EXPRESSION> ::= SYMBOL <FACTOR>
 * 		| COMPOSED_START <REGEX> COMPOSED_END <FACTOR>
 * 		| SYMBOL_LIST_START <SYMBOL_SET> SYMBOL_LIST_END <FACTOR>
 * 		;
 */
void RegexProceduralGrammar::createRegexParserGrammarExpression(Grammar *grammar) {
	ParserGrammar *parserGrammar = grammar->getParserGrammar();
	ParserGrammar::NodeTypeList nodeList;
	
	ParserGrammar::Type nonTerminal = ParserGrammar::NON_TERMINAL;
	ParserGrammar::Type token = ParserGrammar::TOKEN;
	
	// <EXPRESSION> rule 0: SYMBOL <FACTOR>
	nodeList.push_back(ParserGrammar::NodeType(token, tokenSymbolId));
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, factorId));
	parserGrammar->addRule(new ParserGrammar::Rule(expressionId, nodeList));
	nodeList.clear();
	
	// <EXPRESSION> rule 1: COMPOSED_START <REGEX> COMPOSED_END <FACTOR>
	nodeList.push_back(ParserGrammar::NodeType(token, tokenComposedStartId));
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, regexId));
	nodeList.push_back(ParserGrammar::NodeType(token, tokenComposedEndId));
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, factorId));
	parserGrammar->addRule(new ParserGrammar::Rule(expressionId, nodeList));
	nodeList.clear();
	
	// <EXPRESSION> rule 2: SYMBOL_LIST_START <SYMBOL_SET> SYMBOL_LIST_END <FACTOR>
	nodeList.push_back(ParserGrammar::NodeType(token, tokenSymbolListStartId));
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, symbolSetId));
	nodeList.push_back(ParserGrammar::NodeType(token, tokenSymbolListEndId));
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, factorId));
	parserGrammar->addRule(new ParserGrammar::Rule(expressionId, nodeList));
}

/* 
 * <FACTOR> ::= CLOSURE
 * 		| ONE_CLOSURE
 * 		| OPTIONAL
 * 		| // epsilon
 * 		;
 */
void RegexProceduralGrammar::createRegexParserGrammarFactor(Grammar *grammar) {
	ParserGrammar *parserGrammar = grammar->getParserGrammar();
	ParserGrammar::NodeTypeList nodeList;
	
	ParserGrammar::Type token = ParserGrammar::TOKEN;
	
	// <FACTOR> rule 0: CLOSURE
	nodeList.push_back(ParserGrammar::NodeType(token, tokenClosureId));
	parserGrammar->addRule(new ParserGrammar::Rule(factorId, nodeList));
	nodeList.clear();
	
	// <FACTOR> rule 1: ONE_CLOSURE
	nodeList.push_back(ParserGrammar::NodeType(token, tokenOneClosureId));
	parserGrammar->addRule(new ParserGrammar::Rule(factorId, nodeList));
	nodeList.clear();
	
	// <FACTOR> rule 2: OPTIONAL
	nodeList.push_back(ParserGrammar::NodeType(token, tokenOptionalId));
	parserGrammar->addRule(new ParserGrammar::Rule(factorId, nodeList));
	nodeList.clear();
	
	// <FACTOR> rule 3: epsilon
	parserGrammar->addRule(new ParserGrammar::Rule(factorId, nodeList));
}

/* 
 * <SYMBOL_SET> ::= <SYMBOL_LIST>
 * 		| NOT <SYMBOL_LIST>
 * 		;
 */
void RegexProceduralGrammar::createRegexParserGrammarSymbolSet(Grammar *grammar) {
	ParserGrammar *parserGrammar = grammar->getParserGrammar();
	ParserGrammar::NodeTypeList nodeList;
	
	ParserGrammar::Type nonTerminal = ParserGrammar::NON_TERMINAL;
	ParserGrammar::Type token = ParserGrammar::TOKEN;
	
	// <SYMBOL_SET> rule 0: <SYMBOL_LIST>
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, symbolListId));
	parserGrammar->addRule(new ParserGrammar::Rule(symbolSetId, nodeList));
	nodeList.clear();
	
	// <SYMBOL_SET> rule 1: NOT <SYMBOL_LIST>
	nodeList.push_back(ParserGrammar::NodeType(token, tokenNotId));
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, symbolListId));
	parserGrammar->addRule(new ParserGrammar::Rule(symbolSetId, nodeList));
}

/* 
 * <SYMBOL_LIST> ::= SYMBOL <SYMBOL_LIST>
 *		| // epsilon
 *		;
 */
void RegexProceduralGrammar::createRegexParserGrammarSymbolList(Grammar *grammar) {
	ParserGrammar *parserGrammar = grammar->getParserGrammar();
	ParserGrammar::NodeTypeList nodeList;
	
	ParserGrammar::Type nonTerminal = ParserGrammar::NON_TERMINAL;
	ParserGrammar::Type token = ParserGrammar::TOKEN;
	
	// <SYMBOL_LIST> rule 0: SYMBOL <SYMBOL_LIST>
	nodeList.push_back(ParserGrammar::NodeType(token, tokenSymbolId));
	nodeList.push_back(ParserGrammar::NodeType(nonTerminal, symbolListId));
	parserGrammar->addRule(new ParserGrammar::Rule(symbolListId, nodeList));
	nodeList.clear();
	
	// <SYMBOL_LIST> rule 0: epsilon
	parserGrammar->addRule(new ParserGrammar::Rule(symbolListId, nodeList));
}

Grammar *RegexProceduralGrammar::createRegexGrammar() {
	Grammar *grammar = new Grammar();
	
	createRegexScannerGrammar(grammar);
	createRegexParserGrammar(grammar);
	
	return grammar;
}
