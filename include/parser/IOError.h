#ifndef PARSER_IOERROR_H
#define PARSER_IOERROR_H

#include <exception>
#include <string>

class IOError : public std::exception {
public:
	IOError() {}

	IOError(const std::string &msg) : message(msg) {}

	virtual const char *what() const throw() {
		return message.c_str();
	}

private:
	std::string message;
};

#endif
