#ifndef PARSER_PARSER_COMPILE_RESULT_H
#define PARSER_PARSER_COMPILE_RESULT_H

#include "parser/ParserGrammar.h"

#include <ostream>

class Grammar;

class ParserCompileResult {
	public:
		typedef ParserGrammar::Rule Rule;
		typedef ParserGrammar::RuleList RuleList;
		
		class Conflict {
			public:
				Conflict(unsigned int r, unsigned int c);
				
				unsigned int getRow() const;
				unsigned int getCol() const;
				
				void addRule(Rule *rule);
				const RuleList & getConflictingRules() const;
				
				Rule *getDefaultRule() const;
				
				bool hasNullRule() const;
				bool hasNonNullRule() const;
				
				void printConflict(std::ostream & stream, Grammar *grammar) const;
				
				friend std::ostream & operator<<(std::ostream & stream, const Conflict & conflict);
				
			private:
				unsigned int row;
				unsigned int col;
				
				RuleList conflictingRules;
				Rule *defaultRule;
		};
		typedef std::vector<Conflict *> ConflictList;
		
		ParserCompileResult();
		~ParserCompileResult();
		
		void setGrammar(Grammar *g);
		
		void addConflictRule(unsigned int i, unsigned int j, Rule *rule);
		const ConflictList & getConflicts() const;
		
		bool hasConflicts() const;
		unsigned int getConflictsCount() const;
		
		friend std::ostream & operator<<(std::ostream & stream, const ParserCompileResult & result);
		
	private:
		typedef unsigned long long ConflictKey;
		typedef std::map<ConflictKey, Conflict *> ConflictMap;
		
		Grammar *grammar;
		
		ConflictKey getConflictKey(unsigned int i, unsigned int j) const;
		
		ConflictList conflicts;
		ConflictMap conflictMap;
};

#endif
