#ifndef PARSER_SCANNER_AUTOMATA_H
#define PARSER_SCANNER_AUTOMATA_H

#include "parser/ParserError.h"
#include "parser/ParsingTree.h"
#include "parser/TokenType.h"

#include <ostream>
#include <set>

class Automata;
class Input;

/**
 * @class ScannerAutomata
 * 
 * This class holds the Automata used by the Scanner.
 * 
 * @author Felipe Borges Alves
 * 
 * @see Automata
 * @see Scanner
 * @see Input
 */
class ScannerAutomata {
	public:
		typedef std::set<TokenTypeID> TokenTypeIDSet;
		
		/**
		 * Construct a ScannerAutomata with the argument Automata.
		 * 
		 * @param a The automata of this ScannerAutomata.
		 */
		ScannerAutomata(Automata *a);
		~ScannerAutomata();
		
		/**
		 * @return The Automata of this ScannerAutomata.
		 */
		Automata *getAutomata() const;
		
		/**
		 * Register <code>id</code> as an ignored token.
		 * When an ignored token is read, instead of being returned,
		 * it is discarded and another token is read. 
		 */
		void addIgnoredToken(TokenTypeID id);
		
		/**
		 * Get the ignored tokens.
		 * 
		 * When an ignored token is read, instead of being returned,
		 * it is discarded and another token is read.
		 * 
		 * @return A TokenTypeIDSet with the ignored tokens.
		 */
		const TokenTypeIDSet & getIgnoredTokens() const;
		
		/**
		 * Return the token recognized in a state.
		 * 
		 * @param state The state which the token is recognized.
		 * 
		 * @return The ID of the token recognized in the state <code>state</code> or -1
		 * if the state does not recognized any token. 
		 */
		int getStateTokenTypeId(unsigned int state) const;
		
		/**
		 * Set a token recognition state.
		 * 
		 * @param state The state that will recognize the token.
		 * @param tokenId The ID of the recognized token.
		 */
		void setStateTokenTypeId(unsigned int state, TokenTypeID tokenId);
		
		/**
		 * Read the next token.
		 * Do not use this method directly, use Scanner::nextToken() instead.
		 * 
		 * @param input The Input that will supply characters to this ScannerAutomata.
		 * 
		 * @return The next token or NULL if the end of the input was reached.
		 */
		ParsingTree::Token *nextToken(Input *input) const;
		
		// check if this ScannerAutomata is in a consistent state
		void sanityCheck() const;
		
		// this methods are for debug, they may be very inefficient
		bool operator==(const ScannerAutomata & automata) const;
		bool operator!=(const ScannerAutomata & automata) const;
		
		friend std::ostream & operator<<(std::ostream & stream, const ScannerAutomata & automata);
		
	private:
		Automata *automata;
		
		// the tokens recognized in each state
		int *stateTokenTypeId;
		
		TokenTypeIDSet ignoredTokens;
};

#endif
