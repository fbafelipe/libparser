#ifndef PARSER_GRAMMAR_LOADER_BUFFER_LOADER_H
#define PARSER_GRAMMAR_LOADER_BUFFER_LOADER_H

#include "parser/GrammarLoaderLoader.h"

#include <string>

class Grammar;

class GrammarLoaderBufferLoader : public GrammarLoaderLoader {
	public:
		GrammarLoaderBufferLoader();
		virtual ~GrammarLoaderBufferLoader();
		
		virtual const Pointer<ScannerAutomata> & getScannerScannerAutomata() const;
		virtual const Pointer<ScannerAutomata> & getParserScannerAutomata() const;
		
	private:
		void setupScannerTokens();
		void setupParserTokens();
				
		Pointer<ScannerAutomata> scannerScannerAutomata;
		Pointer<ScannerAutomata> parserScannerAutomata;
};

#endif
