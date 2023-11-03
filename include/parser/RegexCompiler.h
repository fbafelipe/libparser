#ifndef PARSER_REGEX_COMPILER_H
#define PARSER_REGEX_COMPILER_H

#include "parser/ParsingTree.h"
#include "parser/Pointer.h"
#include "parser/RegexCompiler.h"

#include <map>
#include <vector>

class Automata;
class DynamicAutomata;
class ParserTable;
class RegexGrammar;
class ScannerAutomata;

class RegexCompiler {
	public:
		RegexCompiler();
		virtual ~RegexCompiler();
		
		static RegexCompiler *getInstance();
		
		Automata *compile(const char *pattern) const;
		
	private:
		static Pointer<RegexCompiler> instance;
		
		typedef ParsingTree::Node Node;
		typedef ParsingTree::NonTerminal NonTerminal;
		typedef ParsingTree::Token Token;
		
		typedef std::vector<char> SymbolList;
		typedef std::map<char, SymbolList> PredefinedSet;
		
		enum FactorType {
			FACTOR_CLOSURE,
			FACTOR_ONE_CLOSURE,
			FACTOR_OPTIONAL,
			
			FACTOR_NONE
		};
		
		DynamicAutomata *getNodeAutomata(ParsingTree::Node *node) const;
		
		DynamicAutomata *getRegexAutomata(NonTerminal *nonTerminal) const;
		DynamicAutomata *getRegexAutomata(const DynamicAutomata *regex,
				NonTerminal *nonTerminal) const;
		DynamicAutomata *getRegexOrAutomata(const DynamicAutomata *exp,
				NonTerminal *nonTerminal) const;
		DynamicAutomata *getExpressionAutomata(NonTerminal *nonTerminal) const;
		FactorType getFactorType(NonTerminal *nonTerminal) const;
		DynamicAutomata *getSymbolSetAutomata(NonTerminal *nonTerminal) const;
		
		DynamicAutomata *getSymbolAutomata(Token *token) const;
		
		void getSymbolList(ParsingTree::NonTerminal *nonTerminal, SymbolList & symbolList) const;
		DynamicAutomata *getSymbolListAcceptAutomata(const SymbolList & symbolList) const;
		DynamicAutomata *getSymbolListRejectAutomata(const SymbolList & symbolList) const;
		
		/*****************************************************************************
		 * Regex predefined sets functions
		 *****************************************************************************/
		PredefinedSet createPredefinedSets() const;
		void createPredefinedEscape(PredefinedSet & predefined, char escape, char c) const;
		void createPredefinedSetDigit(PredefinedSet & predefined) const;
		void createPredefinedSetNonDigit(PredefinedSet & predefined) const;
		void createPredefinedSetWord(PredefinedSet & predefined) const;
		void createPredefinedSetNonWord(PredefinedSet & predefined) const;
		void createPredefinedSetWhite(PredefinedSet & predefined) const;
		void createPredefinedSetNonWhite(PredefinedSet & predefined) const;
		void createPredefinedSetHexaLower(PredefinedSet & predefined) const;
		void createPredefinedSetHexaUpper(PredefinedSet & predefined) const;
		
		RegexGrammar *regexGrammar;
		
		Pointer<ScannerAutomata> scannerAutomata;
		Pointer<ParserTable> parserTable;
		
		PredefinedSet predefinedSets;
};

#endif
