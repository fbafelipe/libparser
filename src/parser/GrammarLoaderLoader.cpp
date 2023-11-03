#include "parser/GrammarLoaderLoader.h"

#include "parser/GrammarLoaderBufferLoader.h"
#include "parser/GrammarLoaderProceduralLoader.h"
#include "parser/ParserDefs.h"

GrammarLoaderLoader *GrammarLoaderLoader::getGrammarLoaderLoader() {
	if (!grammarLoaderLoader) {
#ifdef USE_PROCEDURAL_GRAMMAR
		grammarLoaderLoader = new GrammarLoaderProceduralLoader();
#else
		grammarLoaderLoader = new GrammarLoaderBufferLoader();
#endif
	}
	return grammarLoaderLoader;
}

GrammarLoaderLoader::GrammarLoaderLoader() {
	scannerCommentId = 0;
	scannerIgnoreId = 0;
	scannerReduceId = 0;
	scannerTokenIdId = 0;
	scannerRegexId = 0;
	scannerRuleEndId = 0;
	scannerWhiteSpaceId = 0;
	
	parserCommentId = 0;
	parserNonTerminalId = 0;
	parserReduceId = 0;
	parserRuleEndId = 0;
	parserOrId = 0;
	parserTokenId = 0;
	parserWhitespaceId = 0;
}

GrammarLoaderLoader::~GrammarLoaderLoader() {}

Pointer<GrammarLoaderLoader> GrammarLoaderLoader::grammarLoaderLoader = NULL;
