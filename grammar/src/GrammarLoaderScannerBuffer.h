/*****************************************************************************
 * This file was generated automatically by Parser.
 * Do not edit this file manually.
 * 
 * This file was generated using Parser-1.0.0 compiled in May  9 2009 15:49:39
 * 
 * File created in 2009-05-16 13:29:38
 *****************************************************************************/

#ifndef GRAMMARLOADERSCANNERBUFFER_H
#define GRAMMARLOADERSCANNERBUFFER_H

enum GrammarLoaderScannerBuffer_tokens {
	GRAMMARLOADERSCANNERBUFFER_TOKEN_COMMENT = 0,
	GRAMMARLOADERSCANNERBUFFER_TOKEN_IGNORE = 1,
	GRAMMARLOADERSCANNERBUFFER_TOKEN_REDUCE = 2,
	GRAMMARLOADERSCANNERBUFFER_TOKEN_REGEX = 4,
	GRAMMARLOADERSCANNERBUFFER_TOKEN_RULE_END = 5,
	GRAMMARLOADERSCANNERBUFFER_TOKEN_TOKEN_ID = 3,
	GRAMMARLOADERSCANNERBUFFER_TOKEN_WHITESPACE = 6
};

extern unsigned char scanner_loader_buffer_scanner[];
extern unsigned int scanner_loader_buffer_scanner_size;

#endif