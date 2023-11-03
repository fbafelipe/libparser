#ifndef PARSER_GRAMMAR_LOADER_H
#define PARSER_GRAMMAR_LOADER_H

#include "parser/ParserError.h"
#include "parser/ParsingTree.h"

#include <cstdio>
#include <string>

class Grammar;
class GrammarLoaderLoader;
class Input;
class Scanner;

/**
 * @class GrammarLoader
 * 
 * This class loads grammars from BNF files.
 * 
 * @author Felipe Borges Alves
 */
class GrammarLoader {
	public:
		/**
		 * Load a grammar for the Scanner and a grammar for the parser.
		 * 
		 * @param scannerFile Path to the file with the scanner's grammar.
		 * @param parserFile Path to the file with the parser's grammar.
		 * 
		 * @return A Grammar containing both grammars.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 * @throw IOError If file not found.
		 */
		static Grammar *loadGrammar(const std::string & scannerFile,
				const std::string & parserFile);
		
		/**
		 * Load a grammar for the Scanner and a grammar for the parser.
		 * 
		 * @param scannerFile Path to the file with the scanner's grammar.
		 * @param parserFile Path to the file with the parser's grammar.
		 * 
		 * @return A Grammar containing both grammars.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 * @throw IOError If file not found.
		 */
		static Grammar *loadGrammar(const char *scannerFile,
				const char *parserFile);
		
		/**
		 * Load a grammar for the Scanner and a grammar for the parser.
		 * 
		 * @param scannerFile FILE with the scanner's grammar.
		 * @param parserFile FILE with the parser's grammar.
		 * 
		 * @return A Grammar containing both grammars.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 */
		static Grammar *loadGrammar(FILE *scannerFile, FILE *parserFile);
		
		/**
		 * Load a grammar for the Scanner and a grammar for the parser.
		 * 
		 * @param scannerInput Input with the scanner's grammar.
		 * @param parserInput Input with the parser's grammar.
		 * 
		 * @return A Grammar containing both grammars.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 */
		static Grammar *loadGrammar(Input *scannerInput, Input *parserInput);
		
		/**
		 * Load a grammar for the Scanner.
		 * 
		 * @param grammar The Grammar that will hold the ScannerGrammar
		 * @param scannerFile Path to the file with the scanner's grammar.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 * @throw IOError If file not found.
		 */
		static void loadScanner(Grammar *grammar, const std::string & scannerFile);
		
		/**
		 * Load a grammar for the Scanner.
		 * 
		 * @param grammar The Grammar that will hold the ScannerGrammar
		 * @param scannerFile Path to the file with the scanner's grammar.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 * @throw IOError If file not found.
		 */
		static void loadScanner(Grammar *grammar, const char *scannerFile);
		
		/**
		 * Load a grammar for the Scanner.
		 * 
		 * @param grammar The Grammar that will hold the ScannerGrammar
		 * @param scannerFile FILE with the scanner's grammar.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 */
		static void loadScanner(Grammar *grammar, FILE *scannerFile);
		
		/**
		 * Load a grammar for the Scanner.
		 * 
		 * @param grammar The Grammar that will hold the ScannerGrammar
		 * @param scannerInput Input with the scanner's grammar.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 */
		static void loadScanner(Grammar *grammar, Input *scannerInput);
		
		/**
		 * Load a grammar for the Parser.
		 * 
		 * @param grammar The Grammar that will hold the ParserGrammar
		 * @param parserFile Path to the file with the parser's grammar.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 * @throw IOError If file not found.
		 */
		static void loadParser(Grammar *grammar, const std::string & parserFile);
		
		/**
		 * Load a grammar for the Parser.
		 * 
		 * @param grammar The Grammar that will hold the ParserGrammar
		 * @param parserFile Path to the file with the parser's grammar.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 * @throw IOError If file not found.
		 */
		static void loadParser(Grammar *grammar, const char *parserFile);
		
		/**
		 * Load a grammar for the Parser.
		 * 
		 * @param grammar The Grammar that will hold the ParserGrammar
		 * @param parserFile FILE with the parser's grammar.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 */
		static void loadParser(Grammar *grammar, FILE *parserFile);
		
		/**
		 * Load a grammar for the Parser.
		 * 
		 * @param grammar The Grammar that will hold the ParserGrammar
		 * @param parserFile Input with the parser's grammar.
		 * 
		 * @throw ParserError If the BNF file contains a syntax error.
		 */
		static void loadParser(Grammar *grammar, Input *parserInput);
		
	private:
		GrammarLoader();
		
		static void loadParserNonTerminals(Grammar *grammar, Scanner *scanner,
				GrammarLoaderLoader *loader);
		
		// ignore everything different from tokId
		static ParsingTree::Token *ignoreTokens(Scanner *scanner, TokenTypeID tokId);
};

#endif
