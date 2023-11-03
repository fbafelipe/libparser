#ifndef PARSER_PARSER_ACTION_H
#define PARSER_PARSER_ACTION_H

#include "parser/ParsingTree.h"

/**
 * @class ParserAction
 * 
 * Abstract class for defining actions when a non-terminal symbol is recognized.
 * 
 * @author Felipe Borges Alves
 * 
 * @see Parser
 */
class ParserAction {
	public:
		virtual ~ParserAction() {}
		
		/**
		 * Called when a non-terminal symbol is recognized.
		 * 
		 * @param nonTerminal The recognized symbol.
		 */
		virtual void recognized(ParsingTree::NonTerminal *nonTerminal) = 0;
};

#endif
