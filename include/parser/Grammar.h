#ifndef PARSER_GRAMMAR_H
#define PARSER_GRAMMAR_H

#include "parser/TokenType.h"
#include "parser/ParserError.h"
#include "parser/ParsingTree.h"

#include <map>
#include <string>

class ScannerGrammar;
class ParserGrammar;

/**
 * @class Grammar
 * 
 * Class that holds a ScannerGrammar and a ParserGrammar.
 * This class also map Tokens and NonTerminals IDs to it's names.
 * 
 * @author Felipe Borges Alves
 */
class Grammar {
	public:
		typedef std::map<std::string, TokenTypeID> TokenMap;
		typedef std::map<std::string, ParsingTree::NonTerminalID> NonTerminalMap;
		
		/**
		 * Default constructor.
		 */
		Grammar();
		
		~Grammar();
		
		/**
		 * @return The ScannerGrammar.
		 */
		ScannerGrammar *getScannerGrammar() const;
		
		/**
		 * Set the ScannerGrammar.
		 * This class doesn't take ownership of the ScannerGrammar,
		 * it will not be deleted by Grammar.
		 * 
		 * @param grammar The ScannerGrammar.
		 */
		void setScannerGrammar(ScannerGrammar *grammar);
		
		/**
		 * @return The ParserGrammar.
		 */
		ParserGrammar *getParserGrammar() const;
		
		/**
		 * Set the ParserGrammar.
		 * This class doesn't take ownership of the ParserGrammar,
		 * it will not be deleted by Grammar.
		 * 
		 * @param grammar The ParserGrammar.
		 */
		void setParserGrammar(ParserGrammar *grammar);
		
		/**
		 * Get the TokenTypeID for the token type named <code>tokenName</code>.
		 * If there is no ID for such token type, then a new ID will be created.
		 * 
		 * @param tokenName The name of the token type.
		 * 
		 * @return The TokenTypeID for <code>tokenName</code>.
		 */
		TokenTypeID getOrCreateTokenId(const std::string & tokenName);
		
		/**
		 * @return The TokenTypeID for <code>tokenName</code> or -1 if
		 * there is no ID for such token type.
		 */
		int getTokenId(const std::string & tokenName);
		
		/**
		 * @return A map of token type name to TokenTypeID.
		 */
		const TokenMap & getTokenIds() const;
		
		/**
		 * @return True if this grammar contains a token type named
		 * <code>tokenName</code>.
		 */
		bool hasTokenName(const std::string & tokenName) const;
		
		/**
		 * @return The number of token types.
		 */
		unsigned int getTokenTypeCount() const;
		
		/**
		 * @return The NonTerminalID of the specified non-terminal name.
		 * 
		 * @throw ParserError If there is no NonTerminal with the specified name.
		 */
		ParsingTree::NonTerminalID getNonTerminalId(
				const std::string & nonTerminalName);
		
		/**
		 * Create a NonTerminalID for the specified NonTerminal name.
		 * 
		 * @param nonTerminalName The name of the NonTerminal.
		 * 
		 * @throw ParserError If <code>nonTerminalName</code> already exists.
		 */
		ParsingTree::NonTerminalID createNonTerminalId(
				const std::string & nonTerminalName);
		
		/**
		 * @return A map of non-terminal name to NonTerminalID.
		 */
		const NonTerminalMap & getNonTermonalIds() const;
		
		/**
		 * @return True if this grammar contains a non-terminal named
		 * <code>nonTerminalName</code>.
		 */
		bool hasNonTerminalName(const std::string & nonTerminalName) const;
		
		/**
		 * @return The number of non-terminals.
		 */
		unsigned int getNonTerminalTypeCount() const;
		
		/**
		 * Get the name of a token type by it's ID. 
		 * This method is slow, it is meant for debugging.
		 * 
		 * @param id The TokenTypeID.
		 * 
		 * @return The name of the token type or the string "?" if there is
		 * no such token type.
		 */
		std::string getTokenName(TokenTypeID id) const;
		
		/**
		 * Get the name of a non-terminal by it's ID. 
		 * This method is slow, it is meant for debugging.
		 * 
		 * @param id The NonTerminalID.
		 * 
		 * @return The name of the non-terminal or the string "?" if there is
		 * no such non-terminal.
		 */
		std::string getNonTerminalName(ParsingTree::NonTerminalID id) const;
		
	private:
		ScannerGrammar *scannerGrammar;
		ParserGrammar *parserGrammar;
		
		TokenMap tokenMap;
		TokenTypeID nextTokenId;
		
		NonTerminalMap nonTerminalMap;
		ParsingTree::NonTerminalID nextNonTerminalId;
};

#endif
