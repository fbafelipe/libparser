#ifndef PARSER_PARSER_GRAMMAR_H
#define PARSER_PARSER_GRAMMAR_H

#include "parser/ParserDefs.h"
#include "parser/ParserError.h"
#include "parser/ParsingTree.h"

#include <map>
#include <ostream>
#include <set>
#include <vector>

class Grammar;
class ParserCompileResult;
class ParserTable;
class ParserTableLL1;
class SLR1Verbose;

class ParserGrammar {
	public:
		typedef ParsingTree::NonTerminalID NonTerminalID;
		
		// must be an unsigned int, epsilon may be in the list
		typedef std::vector<unsigned int> SymbolList;
		typedef std::set<unsigned int> SymbolSet;
		typedef std::set<NonTerminalID> NonTerminalSet;
		typedef std::map<NonTerminalID, SymbolSet> NonTerminalFirst;
		typedef std::map<NonTerminalID, SymbolSet> NonTerminalFollow;
		
		enum Type {
			NON_TERMINAL = 0,
			TOKEN = 1
		};
		
		class NodeType {
			public:
				NodeType(Type t, unsigned int i);
				
				Type getType() const;
				unsigned int getId() const;
				
				// used for maps/sets
				bool operator==(const NodeType & other) const;
				bool operator<(const NodeType & other) const;
				
			private:
				Type type;
				unsigned int id;
		};
		typedef std::vector<NodeType> NodeTypeList;
		
		class Rule {
			public:
				Rule(NonTerminalID l, NodeTypeList r);
				
				NonTerminalID getLeft() const;
				const NodeTypeList & getRight() const;
				
				void printRule(std::ostream & stream, Grammar *grammar) const;
				
				friend std::ostream & operator<<(std::ostream & stream, const Rule & rule);
				
			private:
				NonTerminalID left;
				NodeTypeList right;
		};
		typedef std::vector<Rule *> RuleList;
		
		ParserGrammar();
		~ParserGrammar();
		
		void addRule(Rule *rule);
		
		// get the nonterminals defined in this grammar
		void getNonTerminals(NonTerminalSet & nonTerminals) const;
		
		// get the nonterminals that reduce epsilon, directly or indirectly
		void getEpsilonReducers(NonTerminalSet & nonTerminals) const;
		
		void getNonTerminalFirst(NonTerminalFirst & firsts) const;
		void getNonTerminalFollow(NonTerminalFollow & follows) const;
		void getNonTerminalFollow(NonTerminalFollow & follows,
				const NonTerminalFirst & firsts) const;
		
		void getNonTerminalFirstAndFollow(NonTerminalFirst & firsts,
				NonTerminalFollow & follows) const;
		
		ParserTable *compile(Grammar *g) const;
		ParserTable *compile(Grammar *g, ParserCompileResult *compileResult) const;
		
		ParserTable *compileSLR1(Grammar *g) const;
		ParserTable *compileSLR1(Grammar *g,
				ParserCompileResult *compileResult) const;
		ParserTable *compileSLR1(Grammar *g, ParserCompileResult *compileResult,
				std::ostream & verboseOutput) const;
		ParserTable *compileSLR1(Grammar *g, ParserCompileResult *compileResult,
				std::ostream *verboseOutput) const;
		
		ParserTable *compileLL1(Grammar *g) const;
		ParserTable *compileLL1(Grammar *g, ParserCompileResult *compileResult) const;
		
		void printGrammar(std::ostream & stream, Grammar *g) const;
		
		// check if this grammar is in a consistent state
		void sanityCheck() const;
		
	private:
		// return true if any change occurs
		// this function is used while computing the firsts
		bool getFirsts(NonTerminalFirst & firsts, Rule *rule,
				const NonTerminalSet & epsilonReducers) const;
		
		// this function is used to get the firsts of the left side of a rule,
		// while building the parser table
		void getRuleFirsts(SymbolSet & ruleFirsts, const NonTerminalFirst & firsts,
				const Rule *rule) const;
		
		void setLL1Rule(ParserTableLL1 *table, ParserCompileResult *compileResult,
				unsigned int i, unsigned int j, unsigned int rule) const;
		
		// add the rules of this grammar in the parser table
		void addGrammarRules(Grammar *g, ParserTableLL1 *table) const;
		
		RuleList grammar;
};

#endif
