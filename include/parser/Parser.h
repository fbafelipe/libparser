#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include "parser/ParserTable.h"
#include "parser/ParsingTree.h"
#include "parser/Pointer.h"

#include <vector>

class ParserAction;
class Scanner;

/**
 * @class Parser
 * 
 * Class that joint a ParserTable with a Scanner.
 * 
 * @author Felipe Borges Alves
 * @see ParserTable
 * @see Scanner
 */
class Parser {
	public:
		typedef std::vector<ParserError> ParserErrorList;
		
		/**
		 * Construct a Parser with the argument ParserTable
		 * using <code>scan</code> for reading tokens.
		 * 
		 * @param table The ParserTable that will do the parsing.
		 * @param scan The Scanner that will supply tokens.
		 */
		Parser(const Pointer<ParserTable> & table, Scanner *scan);
		virtual ~Parser();
		
		/**
		 * Set a ParserAction that will be executed each Non-terminal
		 * recognition.
		 * 
		 * @param action The action which will be executed each recognition.
		 * 
		 * @see ParserAction
		 */
		void setParserAction(ParserAction *action);
		
		/**
		 * @return The previously set ParserAction or NULL if no action was set.
		 */
		ParserAction *getParserAction() const;
		
		/**
		 * Do the parsing.
		 * 
		 * @return The resulting parsing tree.
		 * 
		 * @throw ParserError If the input has an error.
		 * 
		 * @see ParsingTree
		 * @see ParserError
		 */
		virtual ParsingTree::Node *parse();
		
		/**
		 * Do the parsing.
		 * 
		 * For each error found, a ParserError will be added to the errorList.
		 * If an error occur, the parsing will continue until the end of the input is
		 * reached, checking if there are other errors, but NULL will be returned.
		 * 
		 * @return The resulting parsing tree or NULL if the input contains errors.
		 * 
		 * @see ParsingTree
		 * @see ParserError
		 */
		virtual ParsingTree::Node *parseWithErrorRecovery(ParserErrorList & errorList);
		
		/**
		 * @return The Scanner that supply tokens to this Parser.
		 */
		Scanner *getScanner() const;
		
		/**
		 * @return The ParserTable of this Parser.
		 */
		const Pointer<ParserTable> & getParserTable() const;
		
	protected:
		Pointer<ParserTable> parserTable;
		Scanner *scanner;
		
		ParserAction *parserAction;
};

#endif
