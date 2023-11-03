#include "parser/Regex.h"

#include "parser/Automata.h"
#include "parser/RegexCompiler.h"

Regex::Regex(const std::string & pattern) {
	compile(pattern.c_str());
}

Regex::Regex(const char *pattern) {
	compile(pattern);
}

Regex::Regex(Automata *autom) : automata(autom) {}

Regex::Regex(char c) {
	automata = new Automata(2);
	automata->setTransition(0, c, 1);
	automata->setFinalState(1);
}

Regex::~Regex() {
	delete(automata);
}

bool Regex::matches(const std::string & sentence) const {
	return automata->accept(sentence.c_str());
}

void Regex::compile(const char *pattern) {
	automata = RegexCompiler::getInstance()->compile(pattern);
}

const Automata *Regex::getAutomata() const {
	return automata;
}
