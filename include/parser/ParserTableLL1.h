#ifndef PARSER_PARSER_TABLE_LLONE_H
#define PARSER_PARSER_TABLE_LLONE_H

#include "parser/ParserError.h"
#include "parser/ParserTable.h"
#include "parser/ParsingTree.h"

#include <string>

class Scanner;

/**
 * @class ParserTableLL1
 * 
 * This class holds a LL1 parsing table used during the parsing.
 * 
 * @author Felipe Borges Alves
 */
class ParserTableLL1 : public ParserTable {
	public:
		class Rule : public RuleBase {
			public:
				Rule(NonTerminalID id, unsigned int lIndex);
				
				// push the nodes of this rule in the stack
				void pushRule(NodeStack & nodeStack) const;
				
				bool operator==(const Rule & rule) const;
				bool operator!=(const Rule & rule) const;
		};
		typedef std::vector<Rule *> RuleList;
		typedef std::vector<unsigned int> RuleIndex;
		
		struct ParsingContext;
		
		ParserTableLL1(unsigned int tableRows, unsigned int tableCols);
		virtual ~ParserTableLL1();
		
		unsigned int getParserTableRows() const;
		unsigned int getParserTableCols() const;
		
		int **getParserTable() const;
		
		void setRule(unsigned int i, unsigned int j, int rule);
		int getRule(unsigned int i, unsigned int j) const;
		void addRule(Rule *rule);
		const RuleList & getRuleList() const;
		
		virtual Node *parse(Scanner *scanner, ParserAction *action) const;
		
		virtual Node *parseWithErrorRecovery(Scanner *scanner, ParserAction *parserAction,
				ParserErrorList & errorList) const;
		
		virtual bool operator==(const ParserTable & table) const;
		bool operator==(const ParserTableLL1 & table) const;
		bool operator!=(const ParserTableLL1 & table) const;
		
		virtual void printTable(std::ostream & stream) const;
		
	private:
		// If errorList is not NULL than the parsing is with error recovery,
		// errors will be added to the list and no exception will be thrown
		// If errorList is NULL than the parsing is without error recovery,
		// an exception will be thrown in the first error
		Node *parse(Scanner *scanner, ParserAction *action,
				ParserErrorList *errorList) const;
		
		Rule *getRule(NonTerminal *stackTop, Token *nextToken) const;
		
		unsigned int getParserEndToken() const;
		
		void executeActions(ParserAction *action, NonTerminal *nonTerminal) const;
		
		unsigned int parserTableRows;
		unsigned int parserTableCols;
		int **parserTable;
		
		RuleList ruleList;
};

#endif
