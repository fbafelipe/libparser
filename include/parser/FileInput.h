#ifndef PARSER_STDIO_INPUT_H
#define PARSER_STDIO_INPUT_H

#include "parser/Input.h"

#include <cstdio>

/**
 * @class FileInput
 * 
 * Input implementation which reads from a file.
 * 
 * @author Felipe Borges Alves
 */
class FileInput : public Input {
	public:
		/**
		 * Construct a FileInput that reads from the file with the path
		 * <code>file</code>.
		 * The path will also be used as the input name.
		 * 
		 * @param file The path of the file.
		 * 
		 * @throw IOError If an error occur while opening the file.
		 */
		FileInput(const std::string & file);
		
		/**
		 * Construct a FileInput that reads from the file with the path
		 * <code>file</code>.
		 * The path will also be used as the input name.
		 * 
		 * @param file The path of the file.
		 * 
		 * @throw IOError If an error occur while opening the file.
		 */
		FileInput(const char *file);
		
		/**
		 * Construct a FileInput that reads from the specified file pointer.
		 * The file pointer will be closed in the destructor of this class.
		 * The default input name will be used.
		 * 
		 * @param fp The file pointer used to read the input.
		 */
		FileInput(FILE *fp);
		virtual ~FileInput();
		
		virtual void sanityCheck() const;
		
		virtual void setInputPos(unsigned int pos);
		
	protected:
		virtual char readChar();
		
	private:
		FILE *filePointer;
		
		char *buffer;
		unsigned int bufPos;
		unsigned int bufSize;
};

#endif
