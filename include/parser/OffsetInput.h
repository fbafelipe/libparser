#ifndef PARSER_OFFSET_INPUT_H
#define PARSER_OFFSET_INPUT_H

#include "parser/Input.h"

/**
 * @class OffsetInput
 * 
 * Add an offset (positive or negative) to the line number count of an input.
 * 
 * @Author Felipe Borges Alves
 */
class OffsetInput : public Input {
	public:
		OffsetInput(Input *in, int off);
		OffsetInput(Input *in, int off, const std::string & newName);
		virtual ~OffsetInput();
		
		void setRenameInput(bool rename);
		
		virtual void resetInput();
		
		virtual InputLocation getCurrentLocation() const;
		
		virtual const std::string & getInputName() const;
		virtual unsigned int getInputLine() const;
		
		virtual unsigned int getInputPos() const;
		
		virtual std::string getCurrentLine();
		virtual unsigned int getCurrentLinePos() const;
		
		virtual void markPosition();
		virtual void rollback(unsigned int usedChars);
		
		virtual void setInputPos(unsigned int pos);
		
	protected:
		virtual char readChar();
		
	private:
		Input *input;
		int offset;
		bool renameInput;
};

#endif
