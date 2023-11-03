#ifndef PARSER_SLRONE_TABLE_CREATION_ALGORITHM_H
#define PARSER_SLRONE_TABLE_CREATION_ALGORITHM_H

#include "parser/ParserError.h"
#include "parser/ParserGrammar.h"
#include "parser/ParsingTree.h"

#include <map>
#include <ostream>
#include <queue>
#include <set>

class Grammar;
class ParserTableSLR1;

/**
 * @class SLR1TableCreationAlgorithm
 * 
 * Class with the SLR1 table creation algorithm.
 * 
 * This class is for internal use.
 * 
 * @author Felipe Borges Alves
 */
class SLR1TableCreationAlgorithm {
	public:
		typedef ParserGrammar::NodeType NodeType;
		typedef ParserGrammar::NodeTypeList NodeTypeList;
		typedef ParserGrammar::NonTerminalFirst NonTerminalFirst;
		typedef ParserGrammar::NonTerminalFollow NonTerminalFollow;
		typedef ParserGrammar::Rule Rule;
		typedef ParserGrammar::RuleList RuleList;
		typedef ParserGrammar::SymbolSet SymbolSet;
		typedef ParsingTree::NonTerminalID NonTerminalID;
		
		typedef std::map<const Rule *, unsigned int> RuleIndex;
		typedef std::set<NodeType> NodeTypeSet;
		
		SLR1TableCreationAlgorithm(const RuleList & rList, Grammar *g,
				NonTerminalFollow & follow, ParserCompileResult *compResult,
				std::ostream *verbose);
		~SLR1TableCreationAlgorithm();
		
		ParserTableSLR1 *getParserTable() const;
		
	private:
		typedef std::map<NonTerminalID, RuleList> NonTerminalRules;
		
		/*********************************************************************
		 * Item
		 *********************************************************************/
		class Item {
			public:
				Item(const Rule *r);
				Item(const Rule *r, unsigned int m);
				
				// return true if the mark is in front of a non terminal
				bool markingNonTerminal() const;
				
				// return true if marking the end of the rule
				bool markingEnd() const;
				
				NodeType getMarkedNode() const;
				
				const Rule *getRule() const;
				unsigned int getMark() const;
				
				Item advanceMark() const;
				
				// needed for maps/sets
				bool operator==(const Item & other) const;
				bool operator<(const Item & other) const;
				
				void printItem(std::ostream & stream, Grammar *grammar) const;
				
			private:
				const Rule *rule;
				unsigned int mark;
		};
		typedef std::set<Item> ItemSet;
		
		/*********************************************************************
		 * Kernel
		 *********************************************************************/
		typedef std::set<Item> Kernel;
		typedef std::set<Kernel> KernelSet;
		typedef std::queue<Kernel> KernelQueue;
		typedef std::map<NodeType, Kernel> NodeToKernel;
		typedef std::map<Kernel, unsigned int> KernelIndex;
		
		/*********************************************************************
		 * KernelClosure
		 *********************************************************************/
		class KernelClosure {
			public:
				KernelClosure(SLR1TableCreationAlgorithm *algo, const Kernel & k);
				
				const ItemSet & getClosure() const;
				
				void getSuccessors(NodeToKernel & successors) const;
				
			private:
				void getClosure(const Item & item);
				
				SLR1TableCreationAlgorithm *algorithm;
				
				Kernel kernel;
				
				ItemSet closure;
		};
		
		/*********************************************************************
		 * Auxiliar methods
		 *********************************************************************/
		void getNonTerminalRules();
		
		void createFakeRootRule();
		
		void createKernels();
		void createInitialKernel();
		
		void createKernelIndex();
		
		void createTable();
		void insertHaltAction(unsigned int state);
		void setupTableForItemSet(const ItemSet & itemSet,
				NodeToKernel & successors, unsigned int state, const Kernel & kernel);
		void setupActionsForEndingRule(unsigned int state, const Rule *rule);
		
		void setShiftAction(unsigned int state, unsigned int tok, unsigned int targetState);
		void setGotoAction(unsigned int state, unsigned int nonTerminal, unsigned int targetState);
		void setReduceAction(unsigned int state, unsigned int tok, unsigned int rule);
		
		void addGrammarRules();
		
		// the non terminal created by this algorithm to be the
		// initial symbol
		NonTerminalID getFakeRootId() const;
		
		unsigned int getParserEndToken() const;
		
		void printStateInfo(unsigned int state, const ItemSet & itemSet, const Kernel & kernel);
		
		/*********************************************************************
		 * Variables
		 *********************************************************************/
		const RuleList & ruleList;
		Grammar *grammar;
		NonTerminalFollow & follows;
		ParserCompileResult *compileResult;
		std::ostream *verboseOutput;
		
		ParserTableSLR1 *parserTable;
		
		// the non terminal created by this algorithm to be the
		// initial symbol
		Rule *fakeRule;
		
		KernelSet kernels;
		const Kernel *initialKernel;
		
		KernelIndex kernelIndex;
		RuleIndex ruleIndex;
		
		NonTerminalRules nonTerminalRules;
		
		bool validGrammar;
};

#endif
