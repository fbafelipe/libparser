#include "parser/ParserTable.h"

/*****************************************************************************
 * ParserTable::RuleBase
 *****************************************************************************/
ParserTable::RuleBase::RuleBase(NonTerminalID id, unsigned int lIndex) : nonTerminalId(id),
		localIndex(lIndex) {}

ParserTable::NonTerminalID ParserTable::RuleBase::getNonTerminalId() const {
	return nonTerminalId;
}

unsigned int ParserTable::RuleBase::getLocalIndex() const {
	return localIndex;
}

const ParserTable::RuleNodeList & ParserTable::RuleBase::getRuleNodes() const {
	return ruleNodes;
}

void ParserTable::RuleBase::addRuleNode(const RuleNode & ruleNode) {
	assert(ruleNode.type == RULE_NODE_NONTERMINAL || ruleNode.type == RULE_NODE_TOKEN);
	
	ruleNodes.push_back(ruleNode);
}

std::ostream & operator<<(std::ostream & stream, const ParserTable::RuleBase & rule) {
	stream << "<" << rule.nonTerminalId << "> ::= ";
	
	for (ParserTable::RuleNodeList::const_iterator it = rule.ruleNodes.begin();
			it != rule.ruleNodes.end(); ++it) {
		if (it != rule.ruleNodes.begin()) stream << " ";
		
		const ParserTable::RuleNode & node = *it;
		if (node.type == ParserTable::RULE_NODE_TOKEN) stream << node.id;
		else stream << "<" << node.id << ">";
	}
	
	stream << "; (" << rule.localIndex << ")";
	
	return stream;
}

/*****************************************************************************
 * ParserTable
 *****************************************************************************/
ParserTable::ParserTable() : rootNonTerminalId(0) {}

ParserTable::~ParserTable() {}

ParserTable::NonTerminalID ParserTable::getRootNonTerminalId() const {
	return rootNonTerminalId;
}

void ParserTable::setRootNonTerminalId(NonTerminalID id) {
	rootNonTerminalId = id;
}

bool ParserTable::operator!=(const ParserTable & table) const {
	return !(*this == table);
}

std::ostream & operator<<(std::ostream & stream, const ParserTable & table) {
	table.printTable(stream);
	return stream;
}
