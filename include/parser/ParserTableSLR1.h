#ifndef PARSER_PARSER_TABLE_SLRONE_H
#define PARSER_PARSER_TABLE_SLRONE_H

#include "parser/ParserError.h"
#include "parser/ParserTable.h"
#include "parser/ParsingTree.h"
#include "parser/TokenType.h"

#include <stack>
#include <string>

class Scanner;

/**
 * @class ParserTableSLR1
 * 
 * This class holds a SLR1 parsing table used during the parsing.
 * 
 * @author Felipe Borges Alves
 */
class ParserTableSLR1 : public ParserTable {
	public:
		typedef unsigned int State;
		typedef std::stack<State> StateStack;
		
		typedef std::stack<ParsingTree::Node *> NodeStack;
		typedef ParsingTree::NonTerminal NonTerminal;
		
		class Rule : public RuleBase {
			public:
				Rule(NonTerminalID id, unsigned int lIndex);
				
				bool operator==(const Rule & rule) const;
				bool operator!=(const Rule & rule) const;
		};
		typedef std::vector<Rule *> RuleList;
		typedef std::vector<unsigned int> RuleIndex;
		
		struct ParsingContext;
		
		enum ActionType {
			SHIFT = 0,
			REDUCE = 2,
			HALT = 3,
			ERROR = 4
		};
		
		class Action {
			public:
				Action();
				Action(ActionType t);
				Action(ActionType t, unsigned int n);
				
				ActionType getType() const;
				unsigned int getNum() const;
				
				bool isHaltAction() const;
				bool isErrorAction() const;
				
				void shift(ParsingContext & context) const;
				void reduce(ParsingContext & context) const;
				
				Action & operator=(const Action & action);
				bool operator==(const Action & action) const;
				bool operator!=(const Action & action) const;
				
				friend std::ostream & operator<<(std::ostream & stream, const Action & action);
				
			private:
				ActionType type;
				unsigned int num;
		};
		
		ParserTableSLR1(unsigned int statesSize, unsigned int nonTerminalsSize,
				unsigned int tokensSize);
		virtual ~ParserTableSLR1();
		
		unsigned int getNumStates() const;
		unsigned int getNumNonTerminals() const;
		unsigned int getNumTokens() const;
		
		void addRule(Rule *rule);
		const RuleList & getRuleList() const;
		
		const Action & getAction(State state, TokenTypeID tok) const;
		int getGoto(State state, NonTerminalID nonTerminal) const;
		
		void setAction(const Action & act, State state, TokenTypeID tok);
		void setGoto(int target, State state, NonTerminalID nonTerminal);
		
		int **getGotoTable() const;
		Action **getActionTable() const;
		
		virtual Node *parse(Scanner *scanner,
				ParserAction *parserAction) const;
		
		virtual Node *parseWithErrorRecovery(Scanner *scanner, ParserAction *parserAction,
				ParserErrorList & errorList) const;
		
		virtual void printTable(std::ostream & stream) const;
		
		virtual bool operator==(const ParserTable & table) const;
		bool operator==(const ParserTableSLR1 & table) const;
		bool operator!=(const ParserTableSLR1 & table) const;
		
	private:
		// If errorList is not NULL than the parsing is with error recovery,
		// errors will be added to the list and no exception will be thrown
		// If errorList is NULL than the parsing is without error recovery,
		// an exception will be thrown in the first error
		Node *parse(Scanner *scanner, ParserAction *parserAction,
				ParserErrorList *errorList) const;
		
		const Action & getAction(State state, Token *tok) const;
		State getState(State current, NonTerminalID nonTerminal) const;
		
		unsigned int getParserEndToken() const;
		
		unsigned int numStates;
		unsigned int numNonTerminals;
		
		// constant, but an unsigned int won't hurt
		unsigned int numTokens;
		
		int **gotoTable;
		Action **actionTable;
		
		RuleList ruleList;
};

#endif
