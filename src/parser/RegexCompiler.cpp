#include "parser/RegexCompiler.h"

#include "parser/Automata.h"
#include "parser/AutomataOperations.h"
#include "parser/DynamicAutomata.h"
#include "parser/Grammar.h"
#include "parser/MemoryInput.h"
#include "parser/Parser.h"
#include "parser/ParserDefs.h"
#include "parser/RegexBufferGrammar.h"
#include "parser/RegexProceduralGrammar.h"
#include "parser/Scanner.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>

typedef ParsingTree::Node Node;
typedef ParsingTree::NonTerminal NonTerminal;
typedef ParsingTree::Token Token;

RegexCompiler *RegexCompiler::getInstance() {
	if (!instance)
		instance = new RegexCompiler();
	return instance;
}

RegexCompiler::RegexCompiler() {
	predefinedSets = createPredefinedSets();
	
#ifdef USE_PROCEDURAL_GRAMMAR
	regexGrammar = new RegexProceduralGrammar();
#else
	regexGrammar = new RegexBufferGrammar();
#endif
	
	scannerAutomata = regexGrammar->getScannerAutomata();
	parserTable = regexGrammar->getParserTable();
}

RegexCompiler::~RegexCompiler() {
	delete(regexGrammar);
}

Automata *RegexCompiler::compile(const char *pattern) const {
	Input *input = new MemoryInput(pattern, strlen(pattern));
	
	Scanner *scanner = new Scanner(scannerAutomata, input);
	Parser *parser = new Parser(parserTable, scanner);
	
	ParsingTree::Node *node = parser->parse();
	
	DynamicAutomata *dynamicAutomata = getNodeAutomata(node);
	dynamicAutomata->determineAndMinimize();
	Automata *automata = dynamicAutomata->toAutomata();
	
	delete(node);
	delete(dynamicAutomata);
	delete(parser);
	
	return automata;
}

DynamicAutomata *RegexCompiler::getNodeAutomata(Node *node) const {
	DynamicAutomata *automata = NULL;
	
	if (!node) {
		// accepts just the empty sentence
		automata = new DynamicAutomata();
		automata->getInitialState()->setFinalState(true);
	}
	else {
		assert(node->getNodeType() == ParsingTree::NODE_NON_TERMINAL);
		automata = getRegexAutomata((NonTerminal *)node);
	}
	
	return automata;
}

DynamicAutomata *RegexCompiler::getRegexAutomata(NonTerminal *nonTerminal) const {
	DynamicAutomata *regex = new DynamicAutomata();
	regex->getInitialState()->setFinalState(true);
	
	DynamicAutomata *automata = getRegexAutomata(regex, nonTerminal);
	delete(regex);
	
	return automata;
}

/*
 * <REGEX> ::= <EXPRESSION> <REGEX_OR>
 * 		| // epsilon
 * 		;
 */
DynamicAutomata *RegexCompiler::getRegexAutomata(const DynamicAutomata *regex,
		NonTerminal *nonTerminal) const {
	
	assert(nonTerminal->getNonTerminalId() == regexGrammar->regexId);
	
	DynamicAutomata *automata = NULL;
	
	switch (nonTerminal->getNonTerminalRule()) {
		case 0: // <EXPRESSION> <REGEX_OR>
		{
			DynamicAutomata *expression = getExpressionAutomata(nonTerminal->getNonTerminalAt(0));
			DynamicAutomata *exp = AutomataOperations::automataConcatenation(regex, expression);
			delete(expression);
			automata = getRegexOrAutomata(exp, nonTerminal->getNonTerminalAt(1));
			delete(exp);
			break;
		}
		case 1: // epsilon
			automata = regex->clone();
			break;
		default:
			abort();
	}
	
	return automata;
}

/* 
 * <REGEX_OR> ::= OR <REGEX>
 * 		| <REGEX>
 * 		;
 */
DynamicAutomata *RegexCompiler::getRegexOrAutomata(const DynamicAutomata *exp,
		NonTerminal *nonTerminal) const {
	
	assert(nonTerminal->getNonTerminalId() == regexGrammar->regexOrId);
	
	DynamicAutomata *automata = NULL;
	
	switch (nonTerminal->getNonTerminalRule()) {
		case 0: // OR <REGEX>
		{
			DynamicAutomata *regex = getRegexAutomata(nonTerminal->getNonTerminalAt(1));
			automata = AutomataOperations::automataUnion(exp, regex);
			delete(regex);
			break;
		}
		case 1: // <REGEX>
			automata = getRegexAutomata(exp, nonTerminal->getNonTerminalAt(0));
			break;
		default:
			abort();
	}
	
	return automata;
}

/* 
 * <EXPRESSION> ::= SYMBOL <FACTOR>
 * 		| COMPOSED_START <REGEX> COMPOSED_END <FACTOR>
 * 		| SYMBOL_LIST_START <SYMBOL_SET> SYMBOL_LIST_END <FACTOR>
 * 		;
 */
DynamicAutomata *RegexCompiler::getExpressionAutomata(NonTerminal *nonTerminal) const {
	assert(nonTerminal->getNonTerminalId() == regexGrammar->expressionId);
	
	DynamicAutomata *automata = NULL;
	FactorType factor;
	
	switch (nonTerminal->getNonTerminalRule()) {
		case 0: // SYMBOL <FACTOR>
			automata = getSymbolAutomata(nonTerminal->getTokenAt(0));
			factor = getFactorType(nonTerminal->getNonTerminalAt(1));
			break;
		case 1: // COMPOSED_START <REGEX> COMPOSED_END <FACTOR>
			automata = getRegexAutomata(nonTerminal->getNonTerminalAt(1));
			factor = getFactorType(nonTerminal->getNonTerminalAt(3));
			break;
		case 2:
			automata = getSymbolSetAutomata(nonTerminal->getNonTerminalAt(1));
			factor = getFactorType(nonTerminal->getNonTerminalAt(3));
			break;
		default:
			abort();
	}
	
	DynamicAutomata *expression = NULL;
	
	switch (factor) {
		case FACTOR_CLOSURE:
			expression = AutomataOperations::automataClosure(automata);
			delete(automata);
			break;
		case FACTOR_ONE_CLOSURE:
		{
			DynamicAutomata *closure = AutomataOperations::automataClosure(automata);
			expression = AutomataOperations::automataConcatenation(automata, closure);
			
			delete(closure);
			delete(automata);
			
			break;
		}
		case FACTOR_OPTIONAL:
			expression = automata;
			expression->getInitialState()->setFinalState(true);
			break;
		case FACTOR_NONE:
			expression = automata;
			break;
		default:
			abort();
			
	}
	
	return expression;
}

/* 
 * <FACTOR> ::= CLOSURE
 * 		| ONE_CLOSURE
 * 		| OPTIONAL
 * 		| // epsilon
 * 		;
 */
RegexCompiler::FactorType RegexCompiler::getFactorType(NonTerminal *nonTerminal) const {
	assert(nonTerminal->getNonTerminalId() == regexGrammar->factorId);
	
	FactorType type;
	
	switch (nonTerminal->getNonTerminalRule()) {
		case 0: // CLOSURE
			type = FACTOR_CLOSURE;
			break;
		case 1: // ONE_CLOSURE
			type = FACTOR_ONE_CLOSURE;
			break;
		case 2: // OPTIONAL
			type = FACTOR_OPTIONAL;
			break;
		case 3: // epsilon
			type = FACTOR_NONE;
			break;
		default:
			abort();
	}
	
	return type;
}

/* 
 * <SYMBOL_SET> ::= <SYMBOL_LIST>
 * 		| NOT <SYMBOL_LIST>
 * 		;
 */
DynamicAutomata *RegexCompiler::getSymbolSetAutomata(NonTerminal *nonTerminal) const {
	assert(nonTerminal->getNonTerminalId() == regexGrammar->symbolSetId);
	
	DynamicAutomata *automata = NULL;
	SymbolList symbolList;
	
	switch (nonTerminal->getNonTerminalRule()) {
		case 0: // <SYMBOL_LIST>
			getSymbolList(nonTerminal->getNonTerminalAt(0), symbolList);
			automata = getSymbolListAcceptAutomata(symbolList);
			break;
		case 1:
			getSymbolList(nonTerminal->getNonTerminalAt(1), symbolList);
				automata = getSymbolListRejectAutomata(symbolList);
			break;
		default:
			abort();
	}
	
	return automata;
}

DynamicAutomata *RegexCompiler::getSymbolAutomata(Token *token) const {
	char c;
	const std::string & tok = token->getToken();
	assert(!tok.empty());
	
	// check if it is has a escape character
	if (tok[0] == '\\') {
		assert(tok.size() == 2);
		c = tok[1];
		
		PredefinedSet::const_iterator it = predefinedSets.find(c);
		if (it != predefinedSets.end()) {
			return getSymbolListAcceptAutomata(it->second);
		}
	}
	else {
		assert(tok.size() == 1);
		c = tok[0];
		if (c == '.') {
			SymbolList wildcard;
			for (unsigned int i = 1; i < ALPHABET_SIZE; ++i) wildcard.push_back(i);
			return getSymbolListAcceptAutomata(wildcard);
		}
	}
	
	DynamicAutomata *automata = new DynamicAutomata();
	DynamicAutomata::State *initialState = automata->getInitialState();
	DynamicAutomata::State *finalState = automata->createState();
	finalState->setFinalState(true);
	
	initialState->addTransition(c, finalState);
	
	return automata;
}

/*
 * <SYMBOL_LIST> ::= SYMBOL <SYMBOL_LIST>
 *				| epsilon
 *				;
 */
void RegexCompiler::getSymbolList(ParsingTree::NonTerminal *nonTerminal, SymbolList & symbolList) const {
	assert(nonTerminal->getNonTerminalId() == regexGrammar->symbolListId);
	
	while (nonTerminal) {
		ParsingTree::Token *token = NULL;
		
		if (nonTerminal->getNonTerminalRule() == 0) {
			token = (ParsingTree::Token *)nonTerminal->getNodeList()[0];
			nonTerminal = (ParsingTree::NonTerminal *)nonTerminal->getNodeList()[1];
		}
		else {
			assert(nonTerminal->getNonTerminalRule() == 1);
			break;
		}
		
		const std::string & tok = token->getToken();
		assert(!tok.empty());
		
		// check if it is has a escape character
		if (tok[0] == '\\') {
			assert(tok.size() == 2);
			
			PredefinedSet::const_iterator it = predefinedSets.find(tok[1]);
			if (it != predefinedSets.end()) {
				// include all symbols
				for (SymbolList::const_iterator it2 = it->second.begin();
						it2 != it->second.end(); ++it2) {
					symbolList.push_back(*it2);
				}
			}
			else symbolList.push_back(tok[1]);
		}
		else {
			assert(tok.size() == 1);
			if (tok[0] == '.') {
				for (unsigned int i = 1; i < ALPHABET_SIZE; ++i) symbolList.push_back(i);
			}
			else symbolList.push_back(tok[0]);
		}
	}
}

DynamicAutomata *RegexCompiler::getSymbolListAcceptAutomata(const SymbolList & symbolList) const {
	std::set<char> charSet;
	for (SymbolList::const_iterator it = symbolList.begin(); it != symbolList.end(); ++it) {
		charSet.insert(*it);
	}
	
	DynamicAutomata *automata = new DynamicAutomata();
	DynamicAutomata::State *initialState = automata->getInitialState();
	DynamicAutomata::State *finalState = automata->createState();
	finalState->setFinalState(true);
	
	for (std::set<char>::const_iterator it = charSet.begin(); it != charSet.end(); ++it) {
		initialState->addTransition(*it, finalState);
	}
	
	return automata;
}

DynamicAutomata *RegexCompiler::getSymbolListRejectAutomata(const SymbolList & symbolList) const {
	std::set<char> charSet;
	for (SymbolList::const_iterator it = symbolList.begin(); it != symbolList.end(); ++it) {
		charSet.insert(*it);
	}
	
	DynamicAutomata *automata = new DynamicAutomata();
	DynamicAutomata::State *initialState = automata->getInitialState();
	DynamicAutomata::State *finalState = automata->createState();
	finalState->setFinalState(true);
	
	for (unsigned int input = 1; input < ALPHABET_SIZE; ++input) {
		if (charSet.find(input) == charSet.end()) {
			initialState->addTransition(input, finalState);
		}
	}
	
	return automata;
}

/*****************************************************************************
 * Regex predefined sets functions
 *****************************************************************************/
RegexCompiler::PredefinedSet RegexCompiler::createPredefinedSets() const {
	PredefinedSet predefined;
	
	createPredefinedEscape(predefined, 't', '\t');
	createPredefinedEscape(predefined, 'n', '\n');
	createPredefinedEscape(predefined, 'f', '\f');
	createPredefinedEscape(predefined, 'r', '\r');
	
	createPredefinedSetDigit(predefined);
	createPredefinedSetNonDigit(predefined);
	createPredefinedSetWord(predefined);
	createPredefinedSetNonWord(predefined);
	createPredefinedSetWhite(predefined);
	createPredefinedSetNonWhite(predefined);
	createPredefinedSetHexaLower(predefined);
	createPredefinedSetHexaUpper(predefined);
	
	return predefined;
}

void RegexCompiler::createPredefinedEscape(PredefinedSet & predefined, char escape, char c) const {
	SymbolList symbolList;
	
	symbolList.push_back(c);
	
	predefined[escape] = symbolList;
}

void RegexCompiler::createPredefinedSetDigit(PredefinedSet & predefined) const {
	SymbolList symbolList;
	
	for (unsigned int i = '0'; i <= '9'; ++i) symbolList.push_back(i);
	
	predefined['d'] = symbolList;
}

void RegexCompiler::createPredefinedSetNonDigit(PredefinedSet & predefined) const {
	SymbolList symbolList;
	
	for (unsigned int i = 1; i < '0'; ++i) symbolList.push_back(i);
	for (unsigned int i = '9' + 1; i < ALPHABET_SIZE; ++i) symbolList.push_back(i);
	
	predefined['D'] = symbolList;
}

void RegexCompiler::createPredefinedSetWord(PredefinedSet & predefined) const {
	SymbolList symbolList;
	
	for (unsigned int i = 1; i < ALPHABET_SIZE; ++i) {
		if (isalpha(i) || i == '_') symbolList.push_back(i);
	}
	
	predefined['w'] = symbolList;
}

void RegexCompiler::createPredefinedSetNonWord(PredefinedSet & predefined) const {
	SymbolList symbolList;
	
	for (unsigned int i = 1; i < ALPHABET_SIZE; ++i) {
		if (!isalpha(i) && i != '_') symbolList.push_back(i);
	}
	
	predefined['W'] = symbolList;
}

void RegexCompiler::createPredefinedSetWhite(PredefinedSet & predefined) const {
	SymbolList symbolList;
	
	// [ \t\n\f\r]
	symbolList.push_back(' ');
	symbolList.push_back('\t');
	symbolList.push_back('\n');
	symbolList.push_back('\f');
	symbolList.push_back('\r');
	
	predefined['s'] = symbolList;
}

void RegexCompiler::createPredefinedSetNonWhite(PredefinedSet & predefined) const {
	SymbolList symbolList;
	
	// [^ \t\n\f\r]
	for (unsigned int i = 1; i < ALPHABET_SIZE; ++i) {
		if (i != ' ' && i != '\t' && i != '\n' && i != '\f' && i != '\r') {
			symbolList.push_back(i);
		}
	}
	
	predefined['S'] = symbolList;
}

void RegexCompiler::createPredefinedSetHexaLower(PredefinedSet & predefined) const {
	SymbolList symbolList;
	
	for (unsigned int i = '0'; i <= '9'; ++i) symbolList.push_back(i);
	for (unsigned int i = 'a'; i <= 'f'; ++i) symbolList.push_back(i);
	
	predefined['h'] = symbolList;
}

void RegexCompiler::createPredefinedSetHexaUpper(PredefinedSet & predefined) const {
	SymbolList symbolList;
	
	for (unsigned int i = '0'; i <= '9'; ++i) symbolList.push_back(i);
	for (unsigned int i = 'A'; i <= 'F'; ++i) symbolList.push_back(i);
	
	predefined['H'] = symbolList;
}

Pointer<RegexCompiler> RegexCompiler::instance = NULL;
