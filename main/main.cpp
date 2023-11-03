#include "ArgumentOptions.h"

#include "parser/Grammar.h"
#include "parser/GrammarLoader.h"
#include "parser/ParserCompileResult.h"
#include "parser/ParserDefs.h"
#include "parser/ParserError.h"
#include "parser/ParserGrammar.h"
#include "parser/ParserLoader.h"
#include "parser/ParserTable.h"
#include "parser/ScannerAutomata.h"
#include "parser/ScannerGrammar.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>

static void saveBin(Grammar *grammar, ScannerAutomata *automata,
		ParserTable *table, const char *outputFile);

template<class T>
static void safeWrite(FILE *fp, const T *buffer, unsigned int size);

static void saveCpp(Grammar *grammar, ScannerAutomata *automata,
		ParserTable *table, const char *outputFile, const char *variable);
static void saveC(Grammar *grammar, ScannerAutomata *automata,
		ParserTable *table, const char *outputFile, const char *variable);

static void saveSourcesFiles(std::fstream & header, std::fstream & source, const char *outputFile,
		Grammar *grammar, ScannerAutomata *automata, ParserTable *table, const char *variable);

static void saveHeader(std::fstream & output, const char *outputFile,
		Grammar *grammar, ScannerAutomata *automata, ParserTable *table,
		const std::string & varScanner, const std::string & varParser);

static void saveSource(std::fstream & output, const char *outputFile,
		ScannerAutomata *automata, ParserTable *table,
		const std::string & varScanner, const std::string & varParser);

static void dumpComment(std::fstream & output);

static void dumpVariable(std::fstream & output, const std::string & varName,
		unsigned char *buffer, unsigned int bufferSize);

inline static void getHexaCode(unsigned char byte, char *hexa);

static void dumpEnum(std::fstream & output, const std::string & enumName,
		const std::string & varPrefix, const std::map<std::string, unsigned int> & valueMap);

static std::string toUpperCase(const std::string & str);

int main(int argc, char *argv[]) {
	ArgumentOptions options(argc, argv);
	
	if (!options.getScanner() && !options.getParser()) {
		std::cerr << "You must specity at least the scanner or the parser grammar." << std::endl;
		ArgumentOptions::showUsage();
		return -1;
	}
	
	ScannerAutomata *automata = NULL;
	ParserTable *table = NULL;
	
	Grammar *grammar = new Grammar();
	
	try {
		if (options.getScanner()) {
			try {
				GrammarLoader::loadScanner(grammar, options.getScanner());
				automata = grammar->getScannerGrammar()->compile(grammar);
			}
			catch (ParserError & error) {
				std::cerr << error.getMessage() << std::endl;
				exit(-1);
			}
			catch (std::string & error) {
				std::cerr << "Error: " << error << std::endl;
				exit(-1);
			}
		}
		if (options.getParser()) {
			try {
				GrammarLoader::loadParser(grammar, options.getParser());
			}
			catch (ParserError & error) {
				std::cerr << error.getMessage() << std::endl;
				exit(-1);
			}
			catch (std::string & error) {
				std::cerr << "Error: " << error << std::endl;
				exit(-1);
			}
			
			ParserCompileResult compileResult;
			
			switch (options.getPaserType()) {
				case ArgumentOptions::TYPE_LL1:
					table = grammar->getParserGrammar()->compileLL1(grammar, &compileResult);
					break;
				case ArgumentOptions::TYPE_SLR1:
					if (options.getDumpSlr1()) {
						table = grammar->getParserGrammar()->compileSLR1(grammar,
								&compileResult, std::cout);
					}
					else {
						table = grammar->getParserGrammar()->compileSLR1(grammar, &compileResult);
					}
					break;
				default:
					abort();
			}
			
			if (compileResult.getConflictsCount() > 0) {
				std::cout << "Warning: " << compileResult << "\n";
			}
		}
	}
	catch (ParserError & error) {
		std::cerr << error.getMessage() << std::endl;
		exit(-1);
	}
	
	const char *outputFile = "parser";
	if (options.getOutputFile()) outputFile = options.getOutputFile();
	
	const char *variable = options.getVariable();
	
	switch (options.getFormat()) {
		case ArgumentOptions::FORMAT_BIN:
			saveBin(grammar, automata, table, outputFile);
			break;
		case ArgumentOptions::FORMAT_CPP:
			saveCpp(grammar, automata, table, outputFile, variable);
			break;
		case ArgumentOptions::FORMAT_C:
			saveC(grammar, automata, table, outputFile, variable);
			break;
		default:
			abort();
	}
	
	delete(automata);
	delete(table);
	delete(grammar->getScannerGrammar());
	delete(grammar->getParserGrammar());
	delete(grammar);
	
	return 0;
}

static void saveBin(Grammar *grammar, ScannerAutomata *automata,
		ParserTable *table, const char *outputFile) {
	
	FILE *fp = fopen(outputFile, "wb");
	
	unsigned int size;
	unsigned char *buffer = ParserLoader::saveBuffer(grammar, automata, table, &size);
	
	safeWrite(fp, buffer, size);
	
	fclose(fp);
	
	delete[](buffer);
}

template<class T>
static void safeWrite(FILE *fp, const T *buffer, unsigned int size) {
	unsigned int pos = 0;
	
	while (pos < size) {
		unsigned int w = fwrite(buffer + pos, sizeof(T), size - pos, fp);
		if (w) pos += size;
		else {
			std::cerr << "Error writing to file." << std::endl;
			exit(-1);
		}
	}
}

static void saveCpp(Grammar *grammar, ScannerAutomata *automata,
		ParserTable *table, const char *outputFile, const char *variable) {
	
	std::string headerFile = std::string(outputFile) + ".h";
	std::string sourceFile = std::string(outputFile) + ".cpp";
	
	std::fstream header(headerFile.c_str(), std::ios::out);
	std::fstream source(sourceFile.c_str(), std::ios::out);
	saveSourcesFiles(header, source, outputFile, grammar, automata, table, variable);
}

static void saveC(Grammar *grammar, ScannerAutomata *automata,
		ParserTable *table, const char *outputFile, const char *variable) {
	
	std::string headerFile = std::string(outputFile) + ".h";
	std::string sourceFile = std::string(outputFile) + ".c";
	
	std::fstream header(headerFile.c_str(), std::ios::out);
	std::fstream source(sourceFile.c_str(), std::ios::out);
	saveSourcesFiles(header, source, outputFile, grammar, automata, table, variable);
}

static void saveSourcesFiles(std::fstream & header, std::fstream & source, const char *outputFile,
		Grammar *grammar, ScannerAutomata *automata, ParserTable *table, const char *variable) {
	
	std::string variableScanner = std::string(variable) + "_scanner";
	std::string variableParser = std::string(variable) + "_parser";
	
	// the outputFile may contain a path
	// get only the file name
	const char *p;
	while ((p = strstr(outputFile, "/"))) outputFile = p + 1;
	if (!*outputFile) {
		std::cerr << "Error: invalid output file." << std::endl;
		exit(-1);
	}
	
	saveHeader(header, outputFile, grammar, automata, table, variableScanner, variableParser);
	saveSource(source, outputFile, automata, table, variableScanner, variableParser);
	
	header.close();
	source.close();
}

static void saveHeader(std::fstream & output, const char *outputFile,
		Grammar *grammar, ScannerAutomata *automata, ParserTable *table,
		const std::string & varScanner, const std::string & varParser) {
	
	std::string upperOutput = toUpperCase(outputFile);
	
	dumpComment(output);
	output << "#ifndef " << upperOutput << "_H\n";
	output << "#define " << upperOutput << "_H\n\n";
	
	// create the enum with the tokens
	if (automata) {
		dumpEnum(output, std::string(outputFile) + "_tokens", upperOutput + "_TOKEN_",
				grammar->getTokenIds());
	}
	
	// create the enum with the non terminals
	if (table) {
		dumpEnum(output, std::string(outputFile) + "_nonTerminals",
				upperOutput + "_NONTERMINAL_", grammar->getNonTermonalIds());
	}
	
	// create the variables
	if (automata) {
		output << "extern unsigned char " << varScanner << "[];\n";
		output << "extern unsigned int " << varScanner << "_size;\n\n";
	}
	if (table) {
		output << "extern unsigned char " << varParser << "[];\n";
		output << "extern unsigned int " << varParser << "_size;\n\n";
	}
	
	output << "#endif\n";
}

static void saveSource(std::fstream & output, const char *outputFile,
		ScannerAutomata *automata, ParserTable *table,
		const std::string & varScanner, const std::string & varParser) {
	
	dumpComment(output);
	output << "#include \"" << outputFile << ".h\"\n\n";
	
	if (automata) {
		unsigned int scannerSize;
		unsigned char *scannerBuffer = ParserLoader::automataToBuffer(automata, &scannerSize);
		
		dumpVariable(output, varScanner, scannerBuffer, scannerSize);
	}
	
	output << "\n";
	
	if (table) {
		unsigned int parserSize;
		unsigned char *parserBuffer = ParserLoader::tableToBuffer(table, &parserSize);
		
		dumpVariable(output, varParser, parserBuffer, parserSize);
	}
}

static void dumpComment(std::fstream & output) {
	time_t secs = time(NULL);
	struct tm *tm = localtime(&secs);
	char lTime[128];
	
	sprintf(lTime, "%04d-%02d-%02d %02d:%02d:%02d",
			1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);
	
	output << "/*****************************************************************************\n";
	output << " * This file was generated automatically by Parser.\n";
	output << " * Do not edit this file manually.\n";
	output << " * \n";
	output << " * This file was generated using Parser-" << VERSION << " compiled in "
			<< __DATE__ << " " << __TIME__ << "\n";
	output << " * \n";
	output << " * File created in " << lTime << "\n";
	output << " *****************************************************************************/\n\n";
}

static void dumpVariable(std::fstream & output, const std::string & varName,
		unsigned char *buffer, unsigned int bufferSize) {
	
	const unsigned int bytesPerLine = 12;
	
	output << "unsigned int " << varName << "_size = " << bufferSize << ";\n";
	output << "unsigned char " << varName << "[] = {\n\t";
	
	unsigned int byteInLine = 0;
	for (unsigned int i = 0; i < bufferSize; ++i) {
		char hexa[4];
		getHexaCode(buffer[i], hexa);
		
		output << "0x" << hexa;
		if (i + 1 < bufferSize) {
			output << ",";
			if (++byteInLine >= bytesPerLine) {
				output << "\n\t";
				byteInLine = 0;
			}
			else output << " ";
		}
	}
	
	output << "\n};\n";
}

inline static void getHexaCode(unsigned char byte, char *hexa) {
	unsigned int h[2];
	
	h[0] = 0xF0;
	h[0] &= byte;
	h[0] >>= 4;
	
	h[1] = 0x0F;
	h[1] &= byte;
	 
	assert(h[0] >= 0 && h[0] < 16);
	assert(h[1] >= 0 && h[1] < 16);
	
	if (h[0] < 10) hexa[0] = h[0] + '0';
	else hexa[0] = h[0] + 'A' - 10;
	
	if (h[1] < 10) hexa[1] = h[1] + '0';
	else hexa[1] = h[1] + 'A' - 10;
	
	hexa[2] = '\0';
}

static void dumpEnum(std::fstream & output, const std::string & enumName,
		const std::string & varPrefix, const std::map<std::string, unsigned int> & valueMap) {
	output << "enum " << enumName << " {\n";
	
	bool first = true;
	for (std::map<std::string, unsigned int>::const_iterator it = valueMap.begin();
			it != valueMap.end(); ++it) {
		
		if (first) first = false;
		else if (!first) output << ",\n";
		output << "\t" << varPrefix << it->first << " = " << it->second;
	}
	
	output << "\n};\n\n";
}

static std::string toUpperCase(const std::string & str) {
	std::string upper;
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		upper.push_back(toupper(*it));
	}
	return upper;
}
