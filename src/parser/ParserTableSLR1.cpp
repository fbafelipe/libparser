#include "parser/ParserTableSLR1.h"

#include "parser/ParserAction.h"
#include "parser/Scanner.h"

#include <cassert>
#include <cstdlib>

/*****************************************************************************
 * ParserTableSLR1::Rule
 *****************************************************************************/
ParserTableSLR1::Rule::Rule(NonTerminalID id, unsigned int lIndex) : RuleBase(id, lIndex) {}

bool ParserTableSLR1::Rule::operator==(const Rule & rule) const {
	if (nonTerminalId != rule.nonTerminalId) return false;
	if (localIndex != rule.localIndex) return false;
	if (ruleNodes.size() != rule.ruleNodes.size()) return false;
	
	RuleNodeList::const_iterator it2 = rule.ruleNodes.begin();
	for (RuleNodeList::const_iterator it1 = ruleNodes.begin(); it1 < ruleNodes.end(); ++it1, ++it2) {
		if (it1->type != it2->type || it1->id != it2->id) return false;
	}
	
	return true;
}

bool ParserTableSLR1::Rule::operator!=(const Rule & rule) const {
	return !(*this == rule);
}

/*****************************************************************************
 * ParserTableSLR1::ParsingContext
 *****************************************************************************/
struct ParserTableSLR1::ParsingContext {
	ParsingContext(const ParserTableSLR1 *table, Scanner *scan, ParserAction *act);
	~ParsingContext();
	
	void acceptToken();
	
	void popStates(unsigned int numStates);
	
	ParsingTree::Node *getParsingTreeRoot() const;
	
	// called when an error occurs
	void cleanUp();
	
	// If errorList is not NULL than the parsing is with error recovery,
	// errors will be added to the list and no exception will be thrown
	// If errorList is NULL than the parsing is without error recovery,
	// an exception will be thrown in the first error
	void errorOccurred(ParserErrorList *errorList);
	
	const ParserTableSLR1 *parserTable;
	
	Scanner *scanner;
	ParserAction *parserAction;
	
	StateStack stack;
	Token *nextToken;
	
	// used to build the parsing tree
	NodeStack nodeStack;
};

ParserTableSLR1::ParsingContext::ParsingContext(const ParserTableSLR1 *table,
		Scanner *scan, ParserAction *act) : parserTable(table), scanner(scan),
		parserAction(act) {
	
	nextToken = scanner->nextToken();
	stack.push(0);
}

ParserTableSLR1::ParsingContext::~ParsingContext() {
	assert(!nextToken);
}

void ParserTableSLR1::ParsingContext::acceptToken() {
	assert(nextToken);
	nodeStack.push(nextToken);
	nextToken = scanner->nextToken();
}

void ParserTableSLR1::ParsingContext::popStates(unsigned int numStates) {
	// after poping the states the stack cannot be empty
	assert(numStates < stack.size());
	for (unsigned int i = 0; i < numStates; ++i) stack.pop();
}

ParsingTree::Node *ParserTableSLR1::ParsingContext::getParsingTreeRoot() const {
	assert(nodeStack.size() == 1);
	return nodeStack.top();
}

void ParserTableSLR1::ParsingContext::cleanUp() {
	// do not delete the scanner, it will be deleted in the Parser destructor
	
	while (!nodeStack.empty()) {
		delete(nodeStack.top());
		nodeStack.pop();
	}
	
	delete(nextToken);
	nextToken = NULL;
}

void ParserTableSLR1::ParsingContext::errorOccurred(ParserErrorList *errorList) {
	ParserError error;
	
	if (nextToken) {
		std::string tok = nextToken->getToken();
		error = ParserError(scanner->getInput(), std::string("Unexpected token \"") + tok + "\".");
	}
	else error = ParserError(scanner->getInput(), "Unexpected end of file.");
	
	if (errorList) {
		errorList->push_back(error);
		
		// error recovery
		do {
			delete(nextToken);
			nextToken = scanner->nextToken();
		} while (nextToken && parserTable->getAction(stack.top(), nextToken).getType() == ERROR);
	}
	else {
		cleanUp();
		throw error;
	}
}

/*****************************************************************************
 * ParserTableSLR1::Action
 *****************************************************************************/
ParserTableSLR1::Action::Action() : type(ERROR), num(0x7FFFFFFF) {}

ParserTableSLR1::Action::Action(ActionType t) : type(t), num(0x7FFFFFFF) {}

ParserTableSLR1::Action::Action(ActionType t, unsigned int n) : type(t), num(n) {}

ParserTableSLR1::ActionType ParserTableSLR1::Action::getType() const {
	return type;
}

unsigned int ParserTableSLR1::Action::getNum() const {
	return num;
}

bool ParserTableSLR1::Action::isHaltAction() const {
	return type == HALT;
}

bool ParserTableSLR1::Action::isErrorAction() const {
	return type == ERROR;
}

void ParserTableSLR1::Action::shift(ParsingContext & context) const {
	context.acceptToken();
	context.stack.push(num);
}

void ParserTableSLR1::Action::reduce(ParsingContext & context) const {
	assert(num < context.parserTable->ruleList.size());
	
	Rule *rule = context.parserTable->ruleList[num];
	const RuleNodeList & nodeList = rule->getRuleNodes();
	assert(nodeList.size() < context.stack.size());
	
	NonTerminal *nonTerminal = new NonTerminal(rule->getNonTerminalId(), rule->getLocalIndex());
	
	// the nodes must be added in the inverse order
	NodeStack nodes;
	
	for (RuleNodeList::const_iterator it = nodeList.begin(); it != nodeList.end(); ++it) {
		context.stack.pop();
		
		nodes.push(context.nodeStack.top());
		context.nodeStack.pop();
	}
	
	// add the nodes
	while (!nodes.empty()) {
		nonTerminal->addNode(nodes.top());
		nodes.pop();
	}
	
	context.nodeStack.push(nonTerminal);
	
	State current = context.stack.top();
	current = context.parserTable->getState(current, rule->getNonTerminalId());
	context.stack.push(current);
	
	if (context.parserAction) context.parserAction->recognized(nonTerminal);
}

ParserTableSLR1::Action & ParserTableSLR1::Action::operator=(const Action & action) {
	type = action.type;
	num = action.num;
	return *this;
}

bool ParserTableSLR1::Action::operator==(const Action & action) const {
	return type == action.type && num == action.num;
}

bool ParserTableSLR1::Action::operator!=(const Action & action) const {
	return !(*this == action);
}

std::ostream & operator<<(std::ostream & stream,
		const ParserTableSLR1::Action & action) {
	
	switch (action.type) {
		case ParserTableSLR1::SHIFT:
			stream << "S" << action.num;
			break;
		case ParserTableSLR1::REDUCE:
			stream << "R" << action.num;
			break;
		case ParserTableSLR1::HALT:
			stream << "H";
			break;
		case ParserTableSLR1::ERROR:
			stream << "E";
			break;
	}
	
	return stream;
}

/*****************************************************************************
 * ParserTableSLR1
 *****************************************************************************/
ParserTableSLR1::ParserTableSLR1(unsigned int statesSize,
		unsigned int nonTerminalsSize, unsigned int tokensSize) {
	
	numStates = statesSize;
	numNonTerminals = nonTerminalsSize;
	numTokens = tokensSize;
	
	assert(numStates > 0);
	assert(numNonTerminals > 0);
	assert(numTokens > 0);
	
	gotoTable = new int *[numStates];
	for (unsigned int i = 0; i < numStates; ++i) {
		gotoTable[i] = new int[numNonTerminals];
		
		for (unsigned int j = 0; j < numNonTerminals; ++j) gotoTable[i][j] = -1;
	}
	
	actionTable = new Action *[numStates];
	for (unsigned int i = 0; i < numStates; ++i) actionTable[i] = new Action[numTokens];
}

ParserTableSLR1::~ParserTableSLR1() {
	for (unsigned int i = 0; i < numStates; ++i) {
		delete[](gotoTable[i]);
		delete[](actionTable[i]);
	}
	delete[](gotoTable);
	delete[](actionTable);
	
	for (RuleList::iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
		delete(*it);
	}
}

unsigned int ParserTableSLR1::getNumStates() const {
	return numStates;
}

unsigned int ParserTableSLR1::getNumNonTerminals() const {
	return numNonTerminals;
}

unsigned int ParserTableSLR1::getNumTokens() const {
	return numTokens;
}

void ParserTableSLR1::addRule(Rule *rule) {
	ruleList.push_back(rule);
}

const ParserTableSLR1::RuleList & ParserTableSLR1::getRuleList() const {
	return ruleList;
}

const ParserTableSLR1::Action & ParserTableSLR1::getAction(State state, TokenTypeID tok) const {
	assert(state < numStates);
	assert(tok < numTokens);
	
	return actionTable[state][tok];
}

int ParserTableSLR1::getGoto(State state, NonTerminalID nonTerminal) const {
	assert(state < numStates);
	assert(nonTerminal < numNonTerminals);
	
	return gotoTable[state][nonTerminal];
}

void ParserTableSLR1::setAction(const Action & act, State state, TokenTypeID tok) {
	assert(state < numStates);
	assert(tok < numTokens);
	
	actionTable[state][tok] = act;
}

void ParserTableSLR1::setGoto(int target, State state, NonTerminalID nonTerminal) {
	assert(state < numStates);
	assert(nonTerminal < numNonTerminals);
	
	assert(target >= -1 && target < (int)numStates);
	
	gotoTable[state][nonTerminal] = target;
}

int **ParserTableSLR1::getGotoTable() const {
	return gotoTable;
}

ParserTableSLR1::Action **ParserTableSLR1::getActionTable() const {
	return actionTable;
}

ParserTableSLR1::Node *ParserTableSLR1::parse(Scanner *scanner,
		ParserAction *parserAction) const {
	
	return parse(scanner, parserAction, NULL);
}

ParserTableSLR1::Node *ParserTableSLR1::parseWithErrorRecovery(Scanner *scanner, ParserAction *parserAction,
				ParserErrorList & errorList) const {
	
	return parse(scanner, parserAction, &errorList);
}

ParserTableSLR1::Node *ParserTableSLR1::parse(Scanner *scanner, ParserAction *parserAction,
		ParserErrorList *errorList) const {
	
	ParsingContext context(this, scanner, parserAction);
	
	bool hasError = false;
	
	bool halt = false;
	while (!halt) {
		const Action & action = getAction(context.stack.top(), context.nextToken);
		
		switch (action.getType()) {
			case SHIFT:
				action.shift(context);
				break;
			case REDUCE:
				action.reduce(context);
				break;
			case HALT:
				halt = true;
				break;
			case ERROR:
				context.errorOccurred(errorList);
				hasError = true;
				halt = context.nextToken == NULL;
				break;
		}
	}
	
	assert(!context.nextToken);
	
	if (hasError) {
		context.cleanUp();
		return NULL;
	}
	
	return context.getParsingTreeRoot();
}

const ParserTableSLR1::Action & ParserTableSLR1::getAction(State state, Token *tok) const {
	unsigned int id;
	if (tok) id = tok->getTokenTypeId();
	else id = getParserEndToken();
	
	assert(state < numStates);
	assert(id < numTokens);
	
	return actionTable[state][id];
}

ParserTableSLR1::State ParserTableSLR1::getState(State current, NonTerminalID nonTerminal) const {
	assert(current < numStates);
	assert(nonTerminal < numNonTerminals);
	
	return gotoTable[current][nonTerminal];
}

unsigned int ParserTableSLR1::getParserEndToken() const {
	return numTokens - 1;
}

void ParserTableSLR1::printTable(std::ostream & stream) const {
	stream << "ParserTableSLR1:\n";
	stream << "Num States: " << numStates << "\n";
	stream << "Num Tokens: " << numTokens << "\n";
	stream << "Num Non Terminals: " << numNonTerminals << "\n";
	
	stream << "ActionTable:\n";
	for (unsigned int i = 0; i < numStates; ++i) {
		stream << i << ": ";
		for (unsigned int j = 0; j < numTokens; ++j) {
			if (j) stream << "\t";
			stream << actionTable[i][j];
		}
		stream << "\n";
	}
	
	stream << "GotoTable:\n";
	for (unsigned int i = 0; i < numStates; ++i) {
		stream << i << ": ";
		for (unsigned int j = 0; j < numNonTerminals; ++j) {
			if (j) stream << "\t";
			stream << gotoTable[i][j];
		}
		stream << "\n";
	}
	
	stream << "\nRuleList:\n";
	unsigned int ruleIndex = 0;
	for (RuleList::const_iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
		stream << ruleIndex++ << ": " << **it << "\n";
	}
}

bool ParserTableSLR1::operator==(const ParserTable & table) const {
	if (!dynamic_cast<const ParserTableSLR1 *>(&table)) return false;
	return *this == (const ParserTableSLR1 &)table;
}

bool ParserTableSLR1::operator==(const ParserTableSLR1 & table) const {
	if (numStates != table.numStates) return false;
	if (numNonTerminals != table.numNonTerminals) return false;
	if (numTokens != table.numTokens) return false;
	
	for (unsigned int i = 0; i < numStates; ++i) {
		for (unsigned int j = 0; j < numNonTerminals; ++j) {
			if (gotoTable[i][j] != table.gotoTable[i][j]) return false;
		}
	}
	
	for (unsigned int i = 0; i < numStates; ++i) {
		for (unsigned int j = 0; j < numTokens; ++j) {
			if (actionTable[i][j] != table.actionTable[i][j]) return false;
		}
	}
	
	RuleList::const_iterator it2 = table.ruleList.begin();
	for (RuleList::const_iterator it1 = ruleList.begin(); it1 < ruleList.end(); ++it1, ++it2) {
		if (**it1 != **it2) return false;
	}
	
	return true;
}

bool ParserTableSLR1::operator!=(const ParserTableSLR1 & table) const {
	return !(*this == table);
}
