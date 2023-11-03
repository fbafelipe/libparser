#include "ArgumentOptions.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <getopt.h>
#include <unistd.h>

ArgumentOptions::ArgumentOptions(int argc, char * const argv[]) {
	dumpSlr1 = false;
	format = FORMAT_CPP;
	parserType = TYPE_SLR1;
	parser = NULL;
	output = NULL;
	scanner = NULL;
	variable = "parser";
	
	const char *shortOptions = "df:ho:p:s:t:v:";
	struct option longOptions[] = {
		{"dump", false, NULL, 'd'},
		{"format", true, NULL, 'f'},
		{"help", false, NULL, 'h'},
		{"output", true, NULL, 'o'},
		{"parser", true, NULL, 'p'},
		{"scanner", true, NULL, 's'},
		{"type", true, NULL, 't'},
		{"variable", true, NULL, 'v'}
	};
	int longIndex;
	
	int c;
	while ((c = getopt_long(argc, argv, shortOptions, longOptions, &longIndex)) != -1) {
		switch (c) {
			case 'd':
				dumpSlr1 = true;
				break;
			case 'f':
				setupFormat(optarg);
				break;
			case 'h':
				showUsage();
				exit(0);
				break;
			case 'p':
				parser = optarg;
				break;
			case 'o':
				output = optarg;
				break;
			case 's':
				scanner = optarg;
				break;
			case 't':
				setupParserType(optarg);
				break;
			case 'v':
				variable = optarg;
				break;
			case '?':
				// long opt already printed an error
				showUsage();
				exit(-1);
				break;
			default:
				abort();
		}
	}
}

bool ArgumentOptions::getDumpSlr1() const {
	return dumpSlr1;
}

ArgumentOptions::OutputFormat ArgumentOptions::getFormat() const {
	return format;
}

ArgumentOptions::ParserType ArgumentOptions::getPaserType() const {
	return parserType;
}

const char *ArgumentOptions::getParser() const {
	return parser;
}

const char *ArgumentOptions::getOutputFile() const {
	return output;
}

const char *ArgumentOptions::getScanner() const {
	return scanner;
}

const char *ArgumentOptions::getVariable() const {
	return variable;
}

void ArgumentOptions::setupFormat(const char *f) {
	unsigned int len = strlen(f);
	char upperCase[len + 1];
	for (unsigned int i = 0; i <= len; ++i) upperCase[i] = toupper(f[i]);
	
	if (!strcmp(upperCase, "BIN")) format = FORMAT_BIN;
	else if (!strcmp(upperCase, "CPP")) format = FORMAT_CPP;
	else if (!strcmp(upperCase, "C")) format = FORMAT_C;
	else {
		showUsage();
		exit(-1);
	}
}

void ArgumentOptions::setupParserType(const char *t) {
	unsigned int len = strlen(t);
	char upperCase[len + 1];
	for (unsigned int i = 0; i <= len; ++i) upperCase[i] = toupper(t[i]);
	
	if (!strcmp(upperCase, "LL1")) parserType = TYPE_LL1;
	else if (!strcmp(upperCase, "SLR1")) parserType = TYPE_SLR1;
	else {
		showUsage();
		exit(-1);
	}
}

void ArgumentOptions::showUsage() {
	std::cerr << "Usage: parsergen [OPTIONS]" << std::endl;
	
	std::cerr << "  -d, --dump\t\t Dump SLR1 states information." << std::endl;
	
	std::cerr << "  -f, --format FORMAT\t Specify the output format:" << std::endl;
	std::cerr << "\t\t BIN: Save the in a binary file." << std::endl;
	std::cerr << "\t\t CPP: Save in a C++ sorce code file (default)." << std::endl;
	std::cerr << "\t\t C: Save in a C source code file." << std::endl;
	std::cerr << std::endl;
	
	std::cerr << "  -h, --help\t\t Show this help and exit." << std::endl;
	std::cerr << "  -o, --output <file>\t Specify the output file." << std::endl;
	std::cerr << "  -p, --parser <file>\t Specify a file with the parser grammar." << std::endl;
	std::cerr << "  -s, --scanner <file>\t Specify a file with the scanner grammar." << std::endl;
	
	std::cerr << "  -t, --type TYPE\t Specify the parser format:" << std::endl;
	std::cerr << "\t\t LL1: LL1 parser type." << std::endl;
	std::cerr << "\t\t SLR1: SLR1 parser type (default)." << std::endl;
	std::cerr << std::endl;
	
	std::cerr << "  -v, --variable NAME\t Set the variable name in the C/C++ code." << std::endl;
}
