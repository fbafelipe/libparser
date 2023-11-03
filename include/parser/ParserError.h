#ifndef PARSER_PARSER_ERROR_H
#define PARSER_PARSER_ERROR_H

#include <exception>
#include <string>

class Input;
class InputLocation;

class ParserError : public std::exception {
	public:
		ParserError();
		ParserError(const std::string & msg);
		ParserError(Input *input, const std::string & msg);
		ParserError(Input *input, unsigned int line, unsigned int col,
				const std::string & msg);
		ParserError(const InputLocation & location, const std::string & msg);
		virtual ~ParserError() throw();
		
		const std::string & getInputName() const;
		
		// the error message, without the input name and line
		const std::string & getRawMessage() const;
		
		const std::string & getMessage() const;
		
		virtual const char *what() const throw();
		
		unsigned int getLineNumber() const;
		unsigned int getLineColumn() const;
		
	private:
		void init();
		
		std::string inputName;
		std::string rawMessage;
		std::string message;
		
		std::string currentLine;
		
		unsigned int lineNum;
		unsigned int lineCol;
};

#endif
