#ifndef PARSER_REGEX_GRAMMAR_H
#define PARSER_REGEX_GRAMMAR_H

#include "parser/ParserTable.h"
#include "parser/ParsingTree.h"
#include "parser/Pointer.h"
#include "parser/ScannerAutomata.h"

class RegexGrammar {
	public:
		typedef ParsingTree::NonTerminalID NonTerminalID;
		
		RegexGrammar();
		virtual ~RegexGrammar();
		
		const Pointer<ScannerAutomata> & getScannerAutomata() const;
		const Pointer<ParserTable> & getParserTable() const;
		
		Pointer<ScannerAutomata> scannerAutomata;
		Pointer<ParserTable> parserTable;
		
		// tokens
		TokenTypeID tokenClosureId;
		TokenTypeID tokenOneClosureId;
		TokenTypeID tokenOptionalId;
		TokenTypeID tokenComposedStartId;
		TokenTypeID tokenComposedEndId;
		TokenTypeID tokenOrId;
		TokenTypeID tokenSymbolListStartId;
		TokenTypeID tokenSymbolListEndId;
		TokenTypeID tokenNotId;
		TokenTypeID tokenSymbolId;
		
		// non terminals
		NonTerminalID regexId;
		NonTerminalID regexOrId;
		NonTerminalID expressionId;
		NonTerminalID factorId;
		NonTerminalID symbolSetId;
		NonTerminalID symbolListId;
};

#endif
