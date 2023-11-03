#include "parser/ParserTableLL1.h"

#include "parser/ParserAction.h"
#include "parser/Scanner.h"

#include <cassert>
#include <list>

typedef std::stack<ParserTableLL1::Node *> NodeStack;

/*****************************************************************************
 * ParserTableLL1::Rule
 *****************************************************************************/
ParserTableLL1::Rule::Rule(NonTerminalID id, unsigned int lIndex) : RuleBase(id, lIndex) {}

void ParserTableLL1::Rule::pushRule(NodeStack & nodeStack) const {
	assert(nodeStack.top()->getNodeType() == ParsingTree::NODE_NON_TERMINAL);
	NonTerminal *nonTerminal = (NonTerminal *)nodeStack.top();
	assert(nonTerminal->getNonTerminalId() == nonTerminalId);
	nodeStack.pop();
	
	nonTerminal->setNonTerminalRule(localIndex);
	
	// the nodes need to be added in the reverse order of the stack
	// since it is expensive to insert in the front of a vector,
	// let's add them in the end, in the correct order
	std::list<Node *> nodeList;
	
	for (RuleNodeList::const_reverse_iterator it = ruleNodes.rbegin(); it != ruleNodes.rend(); ++it) {
		Node *node;
		
		if (it->type == ParserTableLL1::RULE_NODE_NONTERMINAL) {
			node = new NonTerminal(it->id);
		}
		else {
			assert(it->type == ParserTableLL1::RULE_NODE_TOKEN);
			
			node = new Token(it->id);
		}
		
		nodeStack.push(node);
		nodeList.push_front(node);
	}
	
	// add the nodes in nonTerminal
	for (std::list<Node *>::const_iterator it = nodeList.begin(); it != nodeList.end(); ++it) {
		nonTerminal->addNode(*it);
	}
}

bool ParserTableLL1::Rule::operator==(const Rule & rule) const {
	if (nonTerminalId != rule.nonTerminalId) return false;
	if (localIndex != rule.localIndex) return false;
	if (ruleNodes.size() != rule.ruleNodes.size()) return false;
	
	RuleNodeList::const_iterator it2 = rule.ruleNodes.begin();
	for (RuleNodeList::const_iterator it1 = ruleNodes.begin(); it1 < ruleNodes.end(); ++it1, ++it2) {
		if (it1->type != it2->type || it1->id != it2->id) return false;
	}
	
	return true;
}

bool ParserTableLL1::Rule::operator!=(const Rule & rule) const {
	return !(*this == rule);
}

/*****************************************************************************
 * ParserTableLL1::ParsingContext
 *****************************************************************************/
struct ParserTableLL1::ParsingContext {
	ParsingContext(const ParserTableLL1 *table, Scanner *scan);
	~ParsingContext();
	
	// called when an error occurs
	void cleanUp();
	
	// If errorList is not NULL than the parsing is with error recovery,
	// errors will be added to the list and no exception will be thrown
	// If errorList is NULL than the parsing is without error recovery,
	// an exception will be thrown in the first error
	void errorOccurredEOF(ParserErrorList *errorList);
	void errorOccurredExpectingToken(ParserErrorList *errorList, TokenTypeID expecting);
	void errorOccurredExpectingRule(ParserErrorList *errorList, NonTerminal *topNt);
	
	const ParserTableLL1 *parserTable;
	
	Scanner *scanner;
	
	Token *nextToken;
	
	NonTerminal *root;
	NodeStack nodeStack;
	
	bool hasError;
};

ParserTableLL1::ParsingContext::ParsingContext(const ParserTableLL1 *table,
		Scanner *scan) : parserTable(table), scanner(scan), hasError(false) {
	
	root = new NonTerminal(parserTable->getRootNonTerminalId());
	nodeStack.push(root);
	
	nextToken = scanner->nextToken();
}

ParserTableLL1::ParsingContext::~ParsingContext() {
	assert(!nextToken);
}

void ParserTableLL1::ParsingContext::cleanUp() {
	// do not delete the scanner, it will be deleted in the Parser destructor
	
	while (!nodeStack.empty()) nodeStack.pop();
	
	delete(root);
	root = NULL;
	
	delete(nextToken);
	nextToken = NULL;
}

void ParserTableLL1::ParsingContext::errorOccurredEOF(ParserErrorList *errorList) {
	hasError = true;
	
	ParserError error(scanner->getInput(), "Unexpected end of file.");
	
	if (errorList) errorList->push_back(error);
	else {
		cleanUp();
		throw error;
	}
}

void ParserTableLL1::ParsingContext::errorOccurredExpectingToken(ParserErrorList *errorList,
		TokenTypeID expecting) {
	
	hasError = true;
	
	ParserError error(scanner->getInput(), "Unexpected token: " + nextToken->getToken());
	
	if (errorList) {
		errorList->push_back(error);
		
		// error recovery
		do {
			delete(nextToken);
			nextToken = scanner->nextToken();
		} while (nextToken && nextToken->getTokenTypeId() != expecting);
	}
	else {
		cleanUp();
		throw error;
	}
}

void ParserTableLL1::ParsingContext::errorOccurredExpectingRule(ParserErrorList *errorList, NonTerminal *topNt) {
	
	hasError = true;
	
	ParserError error(scanner->getInput(), "Unexpected token: " + nextToken->getToken());
	
	if (errorList) {
		errorList->push_back(error);
		
		// error recovery
		do {
			delete(nextToken);
			nextToken = scanner->nextToken();
		} while (nextToken && !parserTable->getRule(topNt, nextToken));
	}
	else {
		cleanUp();
		throw error;
	}
}

/*****************************************************************************
 * ParserTableLL1
 *****************************************************************************/
ParserTableLL1::ParserTableLL1(unsigned int tableRows, unsigned int tableCols) {
	parserTableRows = tableRows;
	parserTableCols = tableCols;
	
	parserTable = new int *[parserTableRows];
	for (unsigned int i = 0; i < parserTableRows; ++i) {
		parserTable[i] = new int[parserTableCols];
		
		for (unsigned int j = 0; j < parserTableCols; ++j) {
			parserTable[i][j] = -1;
		}
	}
}

ParserTableLL1::~ParserTableLL1() {
	for (unsigned int i = 0; i < parserTableRows; ++i) delete[](parserTable[i]);
	delete[](parserTable);
	
	for (RuleList::iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
		delete(*it);
	}
}

unsigned int ParserTableLL1::getParserTableRows() const {
	return parserTableRows;
}

unsigned int ParserTableLL1::getParserTableCols() const {
	return parserTableCols;
}

int **ParserTableLL1::getParserTable() const {
	return parserTable;
}

void ParserTableLL1::setRule(unsigned int i, unsigned int j, int rule) {
	assert(i < parserTableRows);
	assert(j < parserTableCols);
	
	parserTable[i][j] = rule;
}

int ParserTableLL1::getRule(unsigned int i, unsigned int j) const {
	assert(i < parserTableRows);
	assert(j < parserTableCols);
	
	return parserTable[i][j];
}

void ParserTableLL1::addRule(Rule *rule) {
	ruleList.push_back(rule);
}

const ParserTableLL1::RuleList & ParserTableLL1::getRuleList() const {
	return ruleList;
}

ParserTableLL1::Node *ParserTableLL1::parse(Scanner *scanner, ParserAction *action) const {
	
	return parse(scanner, action, NULL);
}

ParserTableLL1::Node *ParserTableLL1::parseWithErrorRecovery(Scanner *scanner, ParserAction *action,
				ParserErrorList & errorList) const {
	
	return parse(scanner, action, &errorList);
}

ParserTableLL1::Node *ParserTableLL1::parse(Scanner *scanner, ParserAction *action, ParserErrorList *errorList) const {
	
	ParsingContext context(this, scanner);
	
	while (!context.nodeStack.empty()) {
		Node *stackTop = context.nodeStack.top();
		
		if (stackTop->getNodeType() == ParsingTree::NODE_TOKEN) {
			Token *tokenTop = (Token *)stackTop;
			
			if (!context.nextToken) {
				context.errorOccurredEOF(errorList);
				break;
			}
			
			if (tokenTop->getTokenTypeId() == context.nextToken->getTokenTypeId()) {
				*tokenTop = *context.nextToken;
				
				context.nodeStack.pop();
				
				delete(context.nextToken);
				context.nextToken = scanner->nextToken();
			}
			else {
				context.errorOccurredExpectingToken(errorList, tokenTop->getTokenTypeId());
				continue;
			}
		}
		else {
			NonTerminal *nonTerminalTop = (NonTerminal *)stackTop;
			
			Rule *rule = getRule(nonTerminalTop, context.nextToken);
			if (rule) rule->pushRule(context.nodeStack);
			else {
				if (context.nextToken) {
					context.errorOccurredExpectingRule(errorList, nonTerminalTop);
					continue;
				}
				else {
					context.errorOccurredEOF(errorList);
					break;
				}
			}
		}
	}
	
	if (context.nextToken) {
		ParserError error(scanner->getInput(), std::string("Junk at the end of file: ") + context.nextToken->getToken());
		if (errorList) errorList->push_back(error);
		else {
			context.cleanUp();
			throw error;
		}
		
		context.hasError = true;
	}
	
	if (context.hasError) {
		context.cleanUp();
		
		return NULL;
	}
	
	assert(context.root->getNodeType() == ParsingTree::NODE_NON_TERMINAL);
	if (action) executeActions(action, context.root);
	
	return context.root;
}

ParserTableLL1::Rule *ParserTableLL1::getRule(NonTerminal *stackTop, Token *nextToken) const {
	// if nextToken is null, then we reached the end of the input
	// parserTableCols - 1
	unsigned int j = nextToken ? nextToken->getTokenTypeId() : getParserEndToken();
	
	int r = parserTable[stackTop->getNonTerminalId()][j];
	if (r == -1) return NULL;
	return ruleList[r];
}

unsigned int ParserTableLL1::getParserEndToken() const {
	return parserTableCols - 1;
}

void ParserTableLL1::executeActions(ParserAction *action, NonTerminal *nonTerminal) const {
	const ParsingTree::NodeList & nodes = nonTerminal->getNodeList();
	
	for (ParsingTree::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ((*it)->getNodeType() == ParsingTree::NODE_NON_TERMINAL) {
			executeActions(action, (NonTerminal *)*it);
		}
	}
	
	action->recognized(nonTerminal);
}

bool ParserTableLL1::operator==(const ParserTable & table) const {
	if (!dynamic_cast<const ParserTableLL1 *>(&table)) return false;
	return *this == (const ParserTableLL1 &)table;
}

bool ParserTableLL1::operator==(const ParserTableLL1 & table) const {
	if (parserTableRows != table.parserTableRows) return false;
	if (parserTableCols != table.parserTableCols) return false;
	if (ruleList.size() != table.ruleList.size()) return false;
	
	for (unsigned int i = 0; i < parserTableRows; ++i) {
		for (unsigned int j = 0; j < parserTableCols; ++j) {
			if (parserTable[i][j] != table.parserTable[i][j]) return false;
		}
	}
	
	RuleList::const_iterator it2 = table.ruleList.begin();
	for (RuleList::const_iterator it1 = ruleList.begin(); it1 < ruleList.end(); ++it1, ++it2) {
		if (**it1 != **it2) return false;
	}
	
	return true;
}

bool ParserTableLL1::operator!=(const ParserTableLL1 & table) const {
	return !(*this == table); 
}

void ParserTableLL1::printTable(std::ostream & stream) const {
	stream << "ParserTableLL1:\n";
	
	for (unsigned int i = 0; i < parserTableRows; ++i) {
		for (unsigned int j = 0; j < parserTableCols; ++j) {
			if (j) stream << "\t";
			stream << parserTable[i][j];
		}
		stream << "\n";
	}
	
	stream << "\nRuleList:\n";
	unsigned int ruleIndex = 0;
	for (RuleList::const_iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
		stream << ruleIndex++ << ": " << **it << "\n";
	}
}
