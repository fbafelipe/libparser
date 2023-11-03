#include "parser/ParsingTree.h"

#include <queue>

/*****************************************************************************
 * ParsingTree::Node
 *****************************************************************************/
ParsingTree::Node::Node(NodeType type) : nodeType(type) {}

ParsingTree::Node::~Node() {}

ParsingTree::NodeType ParsingTree::Node::getNodeType() const {
	return nodeType;
}

/*****************************************************************************
 * ParsingTree::NonTerminal
 *****************************************************************************/

ParsingTree::NonTerminal::NonTerminal(NonTerminalID id) : Node(NODE_NON_TERMINAL),
		nonTerminalId(id), nonTerminalRule(0) {}

ParsingTree::NonTerminal::NonTerminal(NonTerminalID id, unsigned int rule) : Node(NODE_NON_TERMINAL),
		nonTerminalId(id), nonTerminalRule(rule) {}

ParsingTree::NonTerminal::~NonTerminal() {
	deleteSubnodes();
}

ParsingTree::NonTerminalID ParsingTree::NonTerminal::getNonTerminalId() const {
	return nonTerminalId;
}

const ParsingTree::NodeList & ParsingTree::NonTerminal::getNodeList() const {
	return nodes;
}

void ParsingTree::NonTerminal::addNode(Node *node) {
	nodes.push_back(node);
}

unsigned int ParsingTree::NonTerminal::getNonTerminalRule() const {
	return nonTerminalRule;
}

void ParsingTree::NonTerminal::setNonTerminalRule(unsigned int rule) {
	nonTerminalRule = rule;
}

void ParsingTree::NonTerminal::clearNodesList() {
	nodes.clear();
}

void ParsingTree::NonTerminal::deleteSubnodes() {
	// avoid recursion
	std::queue<NonTerminal *> ntQueue;
	
	// do not add this to the queue since it won't be deleted
	
	for (NodeList::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ((*it)->getNodeType() == NODE_NON_TERMINAL) ntQueue.push((NonTerminal *)*it);
		else delete(*it);
	}
	nodes.clear();
	
	while (!ntQueue.empty()) {
		NonTerminal *nt = ntQueue.front();
		ntQueue.pop();
		
		for (NodeList::iterator it = nt->nodes.begin(); it != nt->nodes.end(); ++it) {
			if ((*it)->getNodeType() == NODE_NON_TERMINAL) ntQueue.push((NonTerminal *)*it);
			else delete(*it);
		}
		
		// we need to clear the list or a double free will occur when deleting the nt
		nt->nodes.clear();
		
		delete(nt);
	}
}

bool ParsingTree::NonTerminal::hasInputLocation() const {
	for (NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ((*it)->hasInputLocation()) return true;
	}
	
	return false;
}

InputLocation ParsingTree::NonTerminal::getInputLocation() const {
	for (NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ((*it)->hasInputLocation()) return (*it)->getInputLocation();
	}
	
	return InputLocation();
}

/*****************************************************************************
 * ParsingTree::Token
 *****************************************************************************/
ParsingTree::Token::Token(TokenTypeID id) : Node(NODE_TOKEN), tokenTypeId(id) {}

ParsingTree::Token::Token(TokenTypeID id, const std::string & tok,
		const InputLocation & location) : Node(NODE_TOKEN),
		tokenTypeId(id), token(tok), inputLocation(location) {}

TokenTypeID ParsingTree::Token::getTokenTypeId() const {
	return tokenTypeId;
}

const std::string & ParsingTree::Token::getToken() const {
	return token;
}

void ParsingTree::Token::setToken(const std::string & tok) {
	token = tok;
}

bool ParsingTree::Token::hasInputLocation() const {
	return true;
}

InputLocation ParsingTree::Token::getInputLocation() const {
	return inputLocation;
}

ParsingTree::Token & ParsingTree::Token::operator=(const Token & other) {
	tokenTypeId = other.tokenTypeId;
	token = other.token;
	inputLocation = other.inputLocation;
	
	return *this;
}

/*****************************************************************************
 * ParsingTree
 *****************************************************************************/
ParsingTree::ParsingTree() {}
