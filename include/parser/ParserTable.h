#ifndef PARSER_PARSER_TABLE_H
#define PARSER_PARSER_TABLE_H

#include "parser/ParserError.h"
#include "parser/ParsingTree.h"

#include <ostream>
#include <stack>
#include <string>
#include <vector>

class ParserAction;
class Scanner;

/**
 * @class ParserTable
 * 
 * This class holds the parsing table used during the parsing.
 * 
 * @author Felipe Borges Alves
 */
class ParserTable {
	public:
		typedef ParsingTree::Node Node;
		typedef ParsingTree::NonTerminalID NonTerminalID;
		typedef ParsingTree::NonTerminal NonTerminal;
		typedef ParsingTree::Token Token;
		
		typedef std::vector<ParserError> ParserErrorList;
		
		enum RuleNodeType {
			RULE_NODE_NONTERMINAL = 0,
			RULE_NODE_TOKEN
		};
		
		struct RuleNode {
			RuleNode() : type(RULE_NODE_TOKEN), id(0xFFFFFFFF) {}
			
			RuleNodeType type;
			unsigned int id;
		};
		
		typedef std::vector<RuleNode> RuleNodeList;
		typedef std::stack<Node *> NodeStack;
		
		class RuleBase {
			public:
				RuleBase(NonTerminalID id, unsigned int lIndex);
				
				NonTerminalID getNonTerminalId() const;
				
				unsigned int getLocalIndex() const;
				
				const RuleNodeList & getRuleNodes() const;
				void addRuleNode(const RuleNode & ruleNode);
				
				friend std::ostream & operator<<(std::ostream & stream, const RuleBase & rule);
				
			protected:
				// the non terminal that this rule applies
				NonTerminalID nonTerminalId;
				
				/*
				 * Maps the global rule index to the local rule index
				 * local index means the number of the rule of that non terminal
				 * 
				 * Example:
				 * <S> ::= a | <B>
				 * <B> ::= c | d
				 * 
				 * Global indices:
				 * 0: <S> ::= a
				 * 1: <S> ::= <B>
				 * 2: <B> ::= c
				 * 3: <B> ::= d
				 * 
				 * Local indices:
				 * 0: <S> ::= a
				 * 1: <S> ::= <B>
				 * 0: <B> ::= c
				 * 1: <B> ::= d
				 */
				unsigned int localIndex;
				
				RuleNodeList ruleNodes;
		};
		
		ParserTable();
		virtual ~ParserTable();
		
		NonTerminalID getRootNonTerminalId() const;
		void setRootNonTerminalId(NonTerminalID id);
		
		/**
		 * Do the parsing.
		 * Do not use this method directly, use Parser::parse() instead.
		 * 
		 * @param scanner The Scanner that will supply tokens.
		 * @param action The action to be called each non-terminal recognition.
		 * 
		 * @return The resulting parsing tree.
		 * 
		 * @throw ParserError If the input has an error.
		 */
		virtual Node *parse(Scanner *scanner, ParserAction *action) const = 0;
		
		/**
		 * Do the parsing.
		 * Do not use this method directly, use Parser::parseWithErrorRecovery() instead.
		 * 
		 * For each error found, a ParserError will be added to the errorList.
		 * If an error occur, the parsing will continue until the end of the input is
		 * reached, checking if there are other errors, but NULL will be returned.
		 * 
		 * @param scanner The Scanner that will supply tokens.
		 * @param action The action to be called each non-terminal recognition.
		 * @param errorList A list where the parse errors will be stored.
		 * 
		 * @return The resulting parsing tree or NULL if the input contains errors.
		 */
		virtual Node *parseWithErrorRecovery(Scanner *scanner, ParserAction *parserAction,
				ParserErrorList & errorList) const = 0;
		
		virtual bool operator==(const ParserTable & table) const = 0;
		bool operator!=(const ParserTable & table) const;
		
		friend std::ostream & operator<<(std::ostream & stream, const ParserTable & table);
		
		virtual void printTable(std::ostream & stream) const = 0;
		
	protected:
		NonTerminalID rootNonTerminalId;
};

#endif
