#ifndef PARSER_REGEX_PROCEDURAL_GRAMMAR_H
#define PARSER_REGEX_PROCEDURAL_GRAMMAR_H

#include "parser/RegexGrammar.h"

#include <string>

class Grammar;

class RegexProceduralGrammar : public RegexGrammar {
	public:
		RegexProceduralGrammar();
		virtual ~RegexProceduralGrammar();
		
	protected:
		void scannerGrammarAddTokenType(Grammar *grammar,
				const std::string & tokenName, TokenTypeID *id, char c);
		void createRegexScannerGrammar(Grammar *grammar);
		
		void createRegexParserGrammar(Grammar *grammar);
		
		void createRegexParserGrammarRegex(Grammar *grammar);
		void createRegexParserGrammarRegexOr(Grammar *grammar);
		void createRegexParserGrammarExpression(Grammar *grammar);
		void createRegexParserGrammarFactor(Grammar *grammar);
		void createRegexParserGrammarSymbolSet(Grammar *grammar);
		void createRegexParserGrammarSymbolList(Grammar *grammar);
		
		Grammar *createRegexGrammar();
};

#endif
