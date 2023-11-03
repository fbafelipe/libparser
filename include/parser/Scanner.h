#ifndef PARSER_SCANNER_H
#define PARSER_SCANNER_H

#include "parser/ParserError.h"
#include "parser/ParsingTree.h"
#include "parser/Pointer.h"
#include "parser/ScannerAutomata.h"

class Input;

/**
 * @class Scanner
 * 
 * This class reads characters form an Input to supply tokens to the Parser.
 * If you are parsing a regular language, this class can be used directly,
 * without a Parser.
 * 
 * @author Felipe Borges Alves
 * 
 * @see Input
 */
class Scanner {
	public:
		/**
		 * Construct a Scanner with the argument ScannerAutomata.
		 * 
		 * This constructor will leave the input as NULL.
		 * You must set the input before reading tokens.
		 * 
		 * @param a The ScannerAutomata of this Scanner.
		 * The same ScannerAutomata can be used in multiples Scanners.
		 * 
		 * @see ScannerAutomata
		 */
		Scanner(const Pointer<ScannerAutomata> & a);
		
		/**
		 * Construct a Scanner with the argument ScannerAutomata and
		 * Input.
		 * 
		 * @param a The ScannerAutomata of this Scanner.
		 * The same ScannerAutomata can be used in multiples Scanners.
		 * @param in The Input that will supply characters to this Scanner.
		 */
		Scanner(const Pointer<ScannerAutomata> & a, Input *in);
		
		virtual ~Scanner();
		
		/**
		 * Read the next token.
		 * 
		 * @return The next token or NULL if the end of the input was reached.
		 */
		virtual ParsingTree::Token *nextToken();
		
		/**
		 * @return The Input of this Scanner.
		 */
		Input *getInput() const;
		
		/**
		 * Change the Input of this Scanner.
		 * 
		 * @param in The Input that will supply characters to this Scanner.
		 */
		void setInput(Input *in);
		
		/**
		 * @return The ScannerAutomata of this Scanner.
		 */
		const Pointer<ScannerAutomata> & getScannerAutomata() const;
		
	private:
		Pointer<ScannerAutomata> automata;
		Input *input;
};

#endif
