#include "parser/ParserCompileResult.h"

#include <cassert>

/*****************************************************************************
 * ParserCompileResult::Conflict
 *****************************************************************************/
ParserCompileResult::Conflict::Conflict(unsigned int r, unsigned int c) :
		row(r), col(c), defaultRule(NULL) {}

unsigned int ParserCompileResult::Conflict::getRow() const {
	return row;
}

unsigned int ParserCompileResult::Conflict::getCol() const {
	return col;
}

void ParserCompileResult::Conflict::addRule(Rule *rule) {
	// check if the rule is already added in this conflict
	for (RuleList::const_iterator it = conflictingRules.begin(); it != conflictingRules.end(); ++it) {
		if (*it == rule) return;
	}
	conflictingRules.push_back(rule);
	
	if (!defaultRule) defaultRule = rule;
}

const ParserCompileResult::RuleList & ParserCompileResult::Conflict::getConflictingRules() const {
	return conflictingRules;
}

ParserCompileResult::Rule *ParserCompileResult::Conflict::getDefaultRule() const {
	return defaultRule;
}

bool ParserCompileResult::Conflict::hasNullRule() const {
	for (RuleList::const_iterator it = conflictingRules.begin();
			it != conflictingRules.end(); ++it) {
		if (!*it) return true;
	}
	
	return false;
}

bool ParserCompileResult::Conflict::hasNonNullRule() const {
	for (RuleList::const_iterator it = conflictingRules.begin();
			it != conflictingRules.end(); ++it) {
		if (*it) return true;
	}
	
	return false;
}

void ParserCompileResult::Conflict::printConflict(std::ostream & stream, Grammar *grammar) const {
	if (!hasNullRule()) {
		ParserCompileResult::RuleList::const_iterator it;
		for (it = conflictingRules.begin(); it != conflictingRules.end(); ++it) {
			(*it)->printRule(stream, grammar);
			stream << "\n";
		}
		if (defaultRule) {
			stream << "Defaulting to: ";
			defaultRule->printRule(stream, grammar); 
			stream << "\n";
		}
	}
	else {
		if (hasNonNullRule()) {
			stream << "Shift/Reduce\n";
			stream << "Defaulting to Shift.\n";
		}
		else stream << "Shift/Shift\n";
	}
}

std::ostream & operator<<(std::ostream & stream, const ParserCompileResult::Conflict & conflict) {
	if (!conflict.hasNullRule()) {
		ParserCompileResult::RuleList::const_iterator it;
		for (it = conflict.conflictingRules.begin(); it != conflict.conflictingRules.end(); ++it) {
			stream << **it << "\n";
		}
		stream << "Defaulting to: " << *conflict.defaultRule << "\n";
	}
	else {
		if (conflict.hasNonNullRule()) {
			stream << "Shift/Reduce\n";
			stream << "Defaulting to Shift.\n";
		}
		else stream << "Shift/Shift\n";
	}
	
	return stream;
}

/*****************************************************************************
 * ParserCompileResult
 *****************************************************************************/
ParserCompileResult::ParserCompileResult() : grammar(NULL) {}

ParserCompileResult::~ParserCompileResult() {
	for (ConflictList::iterator it = conflicts.begin(); it != conflicts.end(); ++it) {
		delete(*it);
	}
}

void ParserCompileResult::setGrammar(Grammar *g) {
	grammar = g;
}

void ParserCompileResult::addConflictRule(unsigned int i, unsigned int j, Rule *rule) {
	Conflict *conflict = NULL;
	
	ConflictKey key = getConflictKey(i, j);
	ConflictMap::iterator it = conflictMap.find(key);
	if (it != conflictMap.end()) conflict = it->second;
	else {
		conflict = new Conflict(i, j);
		conflictMap[key] = conflict;
		conflicts.push_back(conflict);
	}
	
	assert(conflict);
	
	conflict->addRule(rule);
}

const ParserCompileResult::ConflictList & ParserCompileResult::getConflicts() const {
	return conflicts;
}

bool ParserCompileResult::hasConflicts() const {
	return !conflicts.empty();
}

unsigned int ParserCompileResult::getConflictsCount() const {
	return conflicts.size();
}

ParserCompileResult::ConflictKey ParserCompileResult::getConflictKey(unsigned int i, unsigned int j) const {
	assert(sizeof(ConflictKey) >= 2 * sizeof(unsigned int));
	return ((ConflictKey)i << (8 * sizeof(unsigned int))) | (ConflictKey)j;
}

std::ostream & operator<<(std::ostream & stream, const ParserCompileResult & result) {
	stream << result.getConflictsCount() << " conflicts.\n";
	
	unsigned int i = 1;
	for (ParserCompileResult::ConflictList::const_iterator it = result.conflicts.begin();
			it != result.conflicts.end(); ++it) {
		
		stream << "Conflict " << i++ << " at [" << (*it)->getRow()
				<< ", " << (*it)->getCol() << "]:\n";
		if (result.grammar) (*it)->printConflict(stream, result.grammar);
		else stream << **it << "\n";
		stream << "\n";
	}
	
	return stream;
}
