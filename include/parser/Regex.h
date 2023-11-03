#ifndef PARSER_REGEX_H
#define PARSER_REGEX_H

#include <map>
#include <string>

class Automata;

class Regex {
	public:
		Regex(const std::string & pattern);
		Regex(const char *pattern);
		Regex(Automata *autom);
		~Regex();
		
		// create a regex that reconize only the strig of size one with the specified char
		Regex(char c);
		
		bool matches(const std::string & sentence) const;
		
		const Automata *getAutomata() const;
		
	private:
		void compile(const char *pattern);
		
		Automata *automata;
};

#endif
