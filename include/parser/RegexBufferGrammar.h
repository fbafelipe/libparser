#ifndef PARSER_REGEX_BUFFER_GRAMMAR_H
#define PARSER_REGEX_BUFFER_GRAMMAR_H

#include "parser/RegexGrammar.h"

class RegexBufferGrammar : public RegexGrammar {
	public:
		RegexBufferGrammar();
		virtual ~RegexBufferGrammar();
		
	private:
		void setupTokens();
		void setupNonTerminals();
};

#endif
