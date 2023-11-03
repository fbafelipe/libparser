#ifndef PARSER_TOKEN_TYPE_H
#define PARSER_TOKEN_TYPE_H

#include <string>
#include <vector>

typedef unsigned int TokenTypeID;

class Regex;

class TokenType {
	public:
		TokenType(const std::string & name, const std::string & rexp);
		TokenType(const std::string & name, Regex *rexp);
		~TokenType();
		
		bool isIgnored() const;
		void setIgnored(bool ig);
		
		const std::string & getTypeName() const;
		const Regex *getRegex() const;
		
	private:
		std::string typeName;
		Regex *regex;
		
		// true if the scanner should ignore tokens of this type
		bool ignored;
};

typedef std::vector<TokenType *> TokenTypeList;

#endif
