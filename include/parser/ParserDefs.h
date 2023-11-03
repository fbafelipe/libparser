#ifndef PARSER_PARSER_DEFS_H
#define PARSER_PARSER_DEFS_H

const char * const VERSION = "1.0.0";

// the alphabet is always the 128 ASCii characters (including NIL)
const unsigned int ALPHABET_SIZE = 128;

// epsilon must be a unsigned int value that does not fit in the ASCii (including nil) charset
const unsigned int EPSILON = 0x7FFFFFFF;

// the symbol $, that represent the end of the input
// this is valid only for the parser
const unsigned int PARSER_END = 0x7FFFFFFE;

//#define USE_PROCEDURAL_GRAMMAR

#endif
