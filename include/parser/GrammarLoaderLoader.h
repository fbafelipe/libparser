#ifndef PARSER_GRAMMAR_LOADER_LOADER_H
#define PARSER_GRAMMAR_LOADER_LOADER_H

#include "parser/Pointer.h"
#include "parser/ScannerAutomata.h"
#include "parser/TokenType.h"

class GrammarLoaderLoader {
	public:
		static GrammarLoaderLoader *getGrammarLoaderLoader();
		
		GrammarLoaderLoader();
		virtual ~GrammarLoaderLoader();
		
		virtual const Pointer<ScannerAutomata> & getScannerScannerAutomata() const = 0;
		virtual const Pointer<ScannerAutomata> & getParserScannerAutomata() const = 0;
		
		/*****************************************************************************
		 * Scanner Tokens
		 *****************************************************************************/ 
		TokenTypeID scannerCommentId;
		TokenTypeID scannerIgnoreId;
		TokenTypeID scannerReduceId;
		TokenTypeID scannerTokenIdId;
		TokenTypeID scannerRegexId;
		TokenTypeID scannerRuleEndId;
		TokenTypeID scannerWhiteSpaceId;

		/*****************************************************************************
		 * Parser Tokens
		 *****************************************************************************/
		TokenTypeID parserCommentId;
		TokenTypeID parserNonTerminalId;
		TokenTypeID parserReduceId;
		TokenTypeID parserRuleEndId;
		TokenTypeID parserOrId;
		TokenTypeID parserTokenId;
		TokenTypeID parserWhitespaceId;
		
	private:
		static Pointer<GrammarLoaderLoader> grammarLoaderLoader;
};

#endif
