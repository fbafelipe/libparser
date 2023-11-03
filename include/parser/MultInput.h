#ifndef PARSER_MULT_INPUT_H
#define PARSER_MULT_INPUT_H

#include "parser/Input.h"

class MultInput : public Input {
	public:
		MultInput();
		virtual ~MultInput();
		
		virtual Input *currentInput() const = 0;
		
		virtual InputLocation getCurrentLocation() const;
		
		virtual const std::string & getInputName() const;
		virtual unsigned int getInputLine() const;
		
		virtual unsigned int getInputPos() const;
		
		virtual std::string getCurrentLine();
		virtual unsigned int getCurrentLinePos() const;
		
		virtual void markPosition();
		virtual void rollback(unsigned int usedChars);
		
		void setInputPos(unsigned int pos);
		
	protected:
		virtual char readChar();
		
		// return true if there is another input
		virtual bool inputFinished() = 0;
		
		unsigned int inputOffset;
};

#endif
