#ifndef PARSER_MEMORY_INPUT_H
#define PARSER_MEMORY_INPUT_H

#include "parser/Input.h"

#include <string>

class MemoryInput : public Input {
	public:
		MemoryInput(const char *buf, unsigned int size);
		MemoryInput(const std::string & str);
		
		MemoryInput(const char *buf, unsigned int size, const std::string & inputName);
		MemoryInput(const std::string & str, const std::string & inputName);
		
		virtual ~MemoryInput();
		
		virtual void setInputPos(unsigned int pos);
		
		virtual void sanityCheck() const;
		
	protected:
		virtual char readChar();
		
		char *buffer;
		unsigned int bufSize;
};

#endif
