#ifndef ARGUMENT_OPTIONS_H
#define ARGUMENT_OPTIONS_H

class ArgumentOptions {
	public:
		enum OutputFormat {
			FORMAT_BIN,
			FORMAT_CPP,
			FORMAT_C
		};
		
		enum ParserType {
			TYPE_LL1,
			TYPE_SLR1
		};
		
		ArgumentOptions(int argc, char * const argv[]);
		
		bool getDumpSlr1() const;
		OutputFormat getFormat() const;
		ParserType getPaserType() const;
		const char *getParser() const;
		const char *getOutputFile() const;
		const char *getScanner() const;
		const char *getVariable() const;
		
		static void showUsage();
		
	private:
		void setupFormat(const char *f);
		void setupParserType(const char *t);
		
		bool dumpSlr1;
		OutputFormat format;
		ParserType parserType;
		const char *parser;
		const char *output;
		const char *scanner;
		const char *variable;
};

#endif
