#include "parser/GrammarLoader.h"

#include "parser/FileInput.h"
#include "parser/Grammar.h"
#include "parser/GrammarLoaderLoader.h"
#include "parser/Parser.h"
#include "parser/ParserGrammar.h"
#include "parser/Regex.h"
#include "parser/Scanner.h"
#include "parser/ScannerGrammar.h"

#include <cstring>

/*****************************************************************************
 * utility functions
 *****************************************************************************/
static void throwError(Scanner *scanner, const std::string & msg);
static std::string cleanToken(const std::string & token);

/*****************************************************************************
 * GrammarLoader
 *****************************************************************************/
GrammarLoader::GrammarLoader() {}

Grammar *GrammarLoader::loadGrammar(const std::string & scannerFile, const std::string & parserFile) {
	return loadGrammar(scannerFile.c_str(), parserFile.c_str());
}

Grammar *GrammarLoader::loadGrammar(const char *scannerFile, const char *parserFile) {
	return loadGrammar(new FileInput(scannerFile), new FileInput(parserFile));
}

Grammar *GrammarLoader::loadGrammar(FILE *scannerFile, FILE *parserFile) {
	return loadGrammar(new FileInput(scannerFile), new FileInput(parserFile));
}

Grammar *GrammarLoader::loadGrammar(Input *scannerInput, Input *parserInput) {
	Grammar *grammar = new Grammar();
	
	loadScanner(grammar, scannerInput);
	loadParser(grammar, parserInput);
	
	return grammar;
}

void GrammarLoader::loadScanner(Grammar *grammar, const std::string & scannerFile) {
	loadScanner(grammar, scannerFile.c_str());
}

void GrammarLoader::loadScanner(Grammar *grammar, const char *scannerFile) {
	loadScanner(grammar, new FileInput(scannerFile));
}

void GrammarLoader::loadScanner(Grammar *grammar, FILE *scannerFile) {
	loadScanner(grammar, new FileInput(scannerFile));
}

void GrammarLoader::loadScanner(Grammar *grammar, Input *scannerInput) {
	GrammarLoaderLoader *loader = GrammarLoaderLoader::getGrammarLoaderLoader();
	Scanner *scanner = new Scanner(loader->getScannerScannerAutomata(), scannerInput);
	
	ScannerGrammar *parsingGrammar = new ScannerGrammar();
	grammar->setScannerGrammar(parsingGrammar);
	
	ParsingTree::Token *token;
	while ((token = scanner->nextToken())) {
		bool ignored = false;
		if (token->getTokenTypeId() == loader->scannerIgnoreId) {
			ignored = true;
			delete(token);
			token = scanner->nextToken();
		}
		
		if (token->getTokenTypeId() != loader->scannerTokenIdId) {
			delete(token);
			throwError(scanner, "Token expected.");
		}
		
		std::string tokenName = cleanToken(token->getToken());
		// create the token id
		// allow the same token to be specified more than once
		// in this case, the resulting regex will be an 'or' of all
		// defined regexes
		grammar->getOrCreateTokenId(tokenName);
		delete(token);
		
		if (!(token = scanner->nextToken())) {
			delete(token);
			throwError(scanner, "Unexpected end of file.");
		}
		if (token->getTokenTypeId() != loader->scannerReduceId) {
			delete(token);
			throwError(scanner, "Expecting \"::=\".");
		}
		delete(token);
		
		if (!(token = scanner->nextToken())) throwError(scanner, "Unexpected end of file.");
		if (token->getTokenTypeId() != loader->scannerRegexId) {
			delete(token);
			throwError(scanner, "Expecting rule regex.");
		}
		std::string regex = cleanToken(token->getToken());
		delete(token);
		
		if (!(token = scanner->nextToken())) throwError(scanner, "Unexpected end of file.");
		if (token->getTokenTypeId() != loader->scannerRuleEndId) {
			delete(token);
			throwError(scanner, "Expecting \";\".");
		}
		
		TokenType *tokenType = NULL;
		try {
			tokenType = new TokenType(tokenName, regex);
		}
		catch (ParserError & error) {
			unsigned int regexSize = token->getToken().size();
			unsigned int errorPos = scannerInput->getCurrentLinePos()
					+ error.getLineColumn() + regexSize;
			
			ParserError err = ParserError(scannerInput, scannerInput->getInputLine(),
					errorPos, error.getRawMessage());
			delete(token);
			delete(scanner);
			throw err;
		}
		tokenType->setIgnored(ignored);
		parsingGrammar->addTokenType(tokenType);
		
		delete(token);
	}
	
	delete(scanner);
}

void GrammarLoader::loadParser(Grammar *grammar, const std::string & parserFile) {
	loadParser(grammar, parserFile.c_str());
}

void GrammarLoader::loadParser(Grammar *grammar, const char *parserFile) {
	loadParser(grammar, new FileInput(parserFile));
}

void GrammarLoader::loadParser(Grammar *grammar, FILE *parserFile) {
	loadParser(grammar, new FileInput(parserFile));
}

void GrammarLoader::loadParser(Grammar *grammar, Input *parserInput) {
	GrammarLoaderLoader *loader = GrammarLoaderLoader::getGrammarLoaderLoader();
	Scanner *scanner = new Scanner(loader->getParserScannerAutomata(), parserInput);
	
	loadParserNonTerminals(grammar, scanner, loader);
	scanner->getInput()->resetInput();
	
	ParserGrammar *parsingGrammar = new ParserGrammar();
	grammar->setParserGrammar(parsingGrammar);
	
	ParsingTree::Token *token;
	while ((token = scanner->nextToken())) {
		if (token->getTokenTypeId() != loader->parserNonTerminalId) {
			delete(token);
			throwError(scanner, "Non-Terminal expected.");
		}
		
		std::string ruleNonTerminal = cleanToken(token->getToken());
		
		// this value will be overriden, but lets initialize it with
		// a huge value, so if an unexpected behaviour occurs, it will be
		// easier to track
		ParsingTree::NonTerminalID ruleNonTerminalId = 0x7FFFFFFF;
		try {
			ruleNonTerminalId = grammar->getNonTerminalId(ruleNonTerminal);
		}
		catch (ParserError & error) {
			delete(token);
			throwError(scanner, error.getMessage());
		}
		delete(token);
		
		if (!(token = scanner->nextToken())) throwError(scanner, "Unexpected end of file.");
		if (token->getTokenTypeId() != loader->parserReduceId) {
			delete(token);
			throwError(scanner, "Expecting \"::=\".");
		}
		delete(token);
		
		bool ruleEnd = false;
		ParserGrammar::NodeTypeList nodeList;
		while (!ruleEnd) {
			if (!(token = scanner->nextToken())) throwError(scanner, "Unexpected end of file.");
			
			if (token->getTokenTypeId() == loader->parserNonTerminalId) {
				std::string nonTerminal = cleanToken(token->getToken());
				ParsingTree::NonTerminalID id = 0;
				try {
					id = grammar->getNonTerminalId(nonTerminal);
				}
				catch (ParserError & error) {
					delete(token);
					throwError(scanner, error.getMessage());
				}
				nodeList.push_back(ParserGrammar::NodeType(ParserGrammar::NON_TERMINAL, id));
			}
			else if (token->getTokenTypeId() == loader->parserTokenId) {
				if (!grammar->hasTokenName(token->getToken())) {
					std::string tok = token->getToken();
					delete(token);
					throwError(scanner, std::string("Use of undeclared token \"") + tok + "\".");
				}
				TokenTypeID id = grammar->getOrCreateTokenId(token->getToken());
				nodeList.push_back(ParserGrammar::NodeType(ParserGrammar::TOKEN, id));
			}
			else if (token->getTokenTypeId() == loader->parserOrId) {
				ParserGrammar::Rule *rule = new ParserGrammar::Rule(ruleNonTerminalId, nodeList);
				parsingGrammar->addRule(rule);
				nodeList.clear();
			}
			else if (token->getTokenTypeId() == loader->parserRuleEndId) ruleEnd = true;
			else {
				std::string tok = token->getToken();
				delete(token);
				throwError(scanner, std::string("Unexpected \"") + tok + "\".");
			}
			
			delete(token);
		}
		ParserGrammar::Rule *rule = new ParserGrammar::Rule(ruleNonTerminalId, nodeList);
		parsingGrammar->addRule(rule);
	}
	
	delete(scanner);
}

void GrammarLoader::loadParserNonTerminals(Grammar *grammar, Scanner *scanner,
		GrammarLoaderLoader *loader) {
	
	// read only the defining nonTerminals
	ParsingTree::Token *token;
	while ((token = ignoreTokens(scanner, loader->parserNonTerminalId))) {
		assert(token->getTokenTypeId() == loader->parserNonTerminalId);
		
		std::string ruleNonTerminal = cleanToken(token->getToken());
		if (!grammar->hasNonTerminalName(ruleNonTerminal)) {
			grammar->createNonTerminalId(ruleNonTerminal);
		}
		delete(token);
		
		// ignore every thing until the next rule end
		delete(ignoreTokens(scanner, loader->parserRuleEndId));
	}
}

ParsingTree::Token *GrammarLoader::ignoreTokens(Scanner *scanner, TokenTypeID tokId) {
	ParsingTree::Token *token;
	while ((token = scanner->nextToken())) {
		if (token->getTokenTypeId() == tokId) return token;
		delete(token);
	}
	
	return NULL;
}

/*****************************************************************************
 * utility functions
 *****************************************************************************/
static void throwError(Scanner *scanner, const std::string & msg) {
	ParserError error = ParserError(scanner->getInput(), msg);
	delete(scanner);
	throw error;
}

static std::string cleanToken(const std::string & token) {
	assert(token.size() >= 2);
	
	char buf[token.size()];
	strcpy(buf, token.c_str() + 1); // remove the first character
	buf[strlen(buf) - 1] = '\0'; // remove the last character
	return std::string(buf);
}
