#ifndef PARSER_LEXER_GRAMMAR_H
#define PARSER_LEXER_GRAMMAR_H

#include "parser/DynamicAutomata.h"
#include "parser/TokenType.h"

#include <string>
#include <vector>

class Grammar;
class ScannerAutomata;

class ScannerGrammar {
	public:
		ScannerGrammar();
		~ScannerGrammar();
		
		void addTokenType(TokenType *tokenType);
		
		// compile this grammar into one automata
		ScannerAutomata *compile(Grammar *g) const;
		
	protected: // not private becouse some tests need to access those methods 
		typedef std::vector<DynamicAutomata *> DynamicAutomataVector;
		typedef std::map<DynamicAutomata::State *, TokenTypeID> StateToToken;
		typedef DynamicAutomata::StateToState StateToState;
		typedef DynamicAutomata::State State;
		typedef DynamicAutomata::StateList StateList;
		typedef DynamicAutomata::StateSet StateSet;
		typedef DynamicAutomata::StateSetToState StateSetToState;
		
		static ScannerAutomata *convertAutomata(DynamicAutomata *dAutomata, const StateToToken & stateToken);
		static void minimizeAutomata(DynamicAutomata *automata, StateToToken & stateToken);

		static DynamicAutomata *mergeAllAutomatas(const DynamicAutomataVector & dAutomataVector,
				StateToToken & stateToken);

		static DynamicAutomata *mergeAutomatas(DynamicAutomata *automataA,
				DynamicAutomata *automataB, StateToToken & stateToken);
		
		static void deleteAutomataAndClearStates(DynamicAutomata *automata,
				StateToToken & stateToken);
		
		static void updateStateTokens(StateToToken & stateToken, const StateToState & oldToNew);
		static void updateStateTokens(StateToToken & stateToken, const StateSetToState & oldToNew);

		static void updateStateToken(StateToToken & stateToken,
				State *oldState, State *newState);
		
	private:
		TokenTypeList grammar;
};

#endif
