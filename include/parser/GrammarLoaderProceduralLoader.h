#ifndef PARSER_GRAMMAR_LOADER_PROCEDURAL_LOADER_H
#define PARSER_GRAMMAR_LOADER_PROCEDURAL_LOADER_H

#include "parser/GrammarLoaderLoader.h"

#include <string>

class Grammar;

class GrammarLoaderProceduralLoader : public GrammarLoaderLoader {
	public:
		GrammarLoaderProceduralLoader();
		virtual ~GrammarLoaderProceduralLoader();
		
		virtual const Pointer<ScannerAutomata> & getScannerScannerAutomata() const;
		virtual const Pointer<ScannerAutomata> & getParserScannerAutomata() const;
		
	private:
		void createToken(Grammar *grammar, const std::string & name,
				TokenTypeID *id, const char *pattern, bool ignored = false);
		
		Grammar *createScannerGrammarGrammar();
		Grammar *createParserGrammarGrammar();
				
		Pointer<ScannerAutomata> scannerScannerAutomata;
		Pointer<ScannerAutomata> parserScannerAutomata;
};

#endif
