#include "parser/ParserGrammar.h"

#include "parser/Grammar.h"
#include "parser/ParserCompileResult.h"
#include "parser/ParserTable.h"
#include "parser/ParserTableLL1.h"
#include "parser/ParserTableSLR1.h"
#include "parser/SLR1TableCreationAlgorithm.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

/*****************************************************************************
 * ParserGrammar::NodeType
 *****************************************************************************/
ParserGrammar::NodeType::NodeType(Type t, unsigned int i) : type(t), id(i) {}

ParserGrammar::Type ParserGrammar::NodeType::getType() const {
	return type;
}

unsigned int ParserGrammar::NodeType::getId() const {
	return id;
}

bool ParserGrammar::NodeType::operator==(const NodeType & other) const {
	return type == other.type && id == other.id;
}

bool ParserGrammar::NodeType::operator<(const NodeType & other) const {
	return type < other.type || (type == other.type && id < other.id);
}

/*****************************************************************************
 * ParserGrammar::Rule
 *****************************************************************************/
ParserGrammar::Rule::Rule(NonTerminalID l, NodeTypeList r) : left(l), right(r) {}

ParserGrammar::NonTerminalID ParserGrammar::Rule::getLeft() const {
	return left;
}

const ParserGrammar::NodeTypeList & ParserGrammar::Rule::getRight() const {
	return right;
}

void ParserGrammar::Rule::printRule(std::ostream & stream, Grammar *grammar) const {
	stream << "<" << grammar->getNonTerminalName(left) << "> ::=";
	
	if (!right.empty()) {
		for (ParserGrammar::NodeTypeList::const_iterator it = right.begin();
				it != right.end(); ++it) {
			if (it->getType() == ParserGrammar::NON_TERMINAL) {
				stream << " <" <<  grammar->getNonTerminalName(it->getId()) << ">";
			}
			else {
				assert(it->getType() == ParserGrammar::TOKEN);
				
				if (it->getId() != grammar->getTokenTypeCount()) {
					stream << " " << grammar->getTokenName(it->getId());
				}
				else stream << " $";
			}
		}
	}
	else stream << " /* empty */";
}

std::ostream & operator<<(std::ostream & stream, const ParserGrammar::Rule & rule) {
	stream << "Nt[" << rule.getLeft() << "] ->";
	
	if (!rule.getRight().empty()) {
		for (ParserGrammar::NodeTypeList::const_iterator it = rule.getRight().begin();
				it != rule.getRight().end(); ++it) {
			if (it->getType() == ParserGrammar::NON_TERMINAL) {
				stream << " Nt[" << it->getId() << "]";
			}
			else {
				assert(it->getType() == ParserGrammar::TOKEN);
				stream << " " << it->getId();
			}
		}
	}
	else stream << " /* empty */";
	
	return stream;
}

/*****************************************************************************
 * ParserGrammar
 *****************************************************************************/
ParserGrammar::ParserGrammar() {}

ParserGrammar::~ParserGrammar() {
	for (RuleList::iterator it = grammar.begin(); it != grammar.end(); ++it) delete(*it);
}

void ParserGrammar::addRule(Rule *rule) {
	grammar.push_back(rule);
}

void ParserGrammar::getNonTerminals(NonTerminalSet & nonTerminals) const {
	for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
		nonTerminals.insert((*it)->getLeft());
	}
}

void ParserGrammar::getEpsilonReducers(NonTerminalSet & nonTerminals) const {
	for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
		if ((*it)->getRight().empty()) nonTerminals.insert((*it)->getLeft());
	}
	
	bool changed = true;
	while (changed) {
		changed = false;
		
		for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
			if (nonTerminals.find((*it)->getLeft()) == nonTerminals.end()) {
				bool reduceEpsilon = true;
				
				const NodeTypeList right = (*it)->getRight();
				for (NodeTypeList::const_iterator it2 = right.begin(); it2 != right.end(); ++it2) {
					if (it2->getType() == TOKEN) {
						reduceEpsilon = false;
						break;
					}
					else {
						assert(it2->getType() == NON_TERMINAL);
						if (nonTerminals.find(it2->getId()) == nonTerminals.end()) {
							reduceEpsilon = false;
							break;
						}
					}
				}
				
				if (reduceEpsilon) {
					nonTerminals.insert((*it)->getLeft());
					changed = true;
				}
			}
		}
	}
}

void ParserGrammar::getNonTerminalFirst(NonTerminalFirst & firsts) const {
	NonTerminalSet epsilonReducers;
	getEpsilonReducers(epsilonReducers);
	
	bool changed = true;
	while (changed) {
		changed = false;
		
		for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
			changed |= getFirsts(firsts, *it, epsilonReducers);
		}
	}
}

void ParserGrammar::getNonTerminalFollow(NonTerminalFollow & follows) const {
	NonTerminalFirst firsts;
	getNonTerminalFirst(firsts);
	getNonTerminalFollow(follows, firsts);
}

void ParserGrammar::getNonTerminalFollow(NonTerminalFollow & follows,
		const NonTerminalFirst & firsts) const {
	
	assert(!grammar.empty());
	
	// Rule 1: if A is the grammar's initial symbol, then $ e follow(A)
	follows[grammar[0]->getLeft()].insert(PARSER_END);
	
	// Rule 2: if A -> aBb and b != EPSILON, then first(b) is contained in follow(B)
	for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
		Rule *rule = *it;
		
		const NodeTypeList & right = rule->getRight();
		for (NodeTypeList::const_iterator it2 = right.begin(); it2 != right.end(); ++it2) {
			if (it2->getType() == NON_TERMINAL) {
				SymbolSet & it2Follows = follows[it2->getId()];
				
				NodeTypeList::const_iterator it3 = it2;
				
				for (++it3; it3 != right.end(); ++it3) {
					if (it3->getType() == TOKEN) break;
					
					NonTerminalFirst::const_iterator firstIt = firsts.find(it3->getId());
					assert(firstIt != firsts.end());
					const SymbolSet & it3Firsts = firstIt->second;
					for (SymbolSet::const_iterator sIt = it3Firsts.begin();
							sIt != it3Firsts.end(); ++sIt) {
						it2Follows.insert(*sIt);
					}
				}
				
				if (it3 != right.end()) {
					assert(it3->getType() == TOKEN);
					it2Follows.insert(it3->getId());
				}
			}
		}
	}
	
	// Rule 3: if A -> aB (or A -> aBb, where EPSILON e first(b))
	// then follow(A) is contained in follow(B)
	bool changed = true;
	while (changed) {
		changed = false;
		
		for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
			Rule *rule = *it;
			const SymbolSet & ruleFollows = follows[rule->getLeft()];
			
			const NodeTypeList & right = rule->getRight();
			for (NodeTypeList::const_iterator it2 = right.begin(); it2 != right.end(); ++it2) {
				if (it2->getType() == NON_TERMINAL) {
					NodeTypeList::const_iterator it3 = it2;
					
					for (++it3; it3 != right.end(); ++it3) {
						if (it3->getType() == TOKEN) break;
						NonTerminalFirst::const_iterator firstIt = firsts.find(it3->getId());
						assert(firstIt != firsts.end());
						if (firstIt->second.find(EPSILON) == firstIt->second.end()) break;
					}
					
					if (it3 == right.end()) {
						// copy the follows from rule->getLeft() to follows from it2->getId()
						SymbolSet & it2Follows = follows[it2->getId()];
						for (SymbolSet::const_iterator sIt = ruleFollows.begin();
								sIt != ruleFollows.end(); ++sIt) {
							if (it2Follows.find(*sIt) == it2Follows.end()) {
								it2Follows.insert(*sIt);
								changed = true;
							}
						}
					}
				}
			}
		}
	}
	
	// remove the EPSILON that may have been added in some follows
	for (NonTerminalFollow::iterator it = follows.begin(); it != follows.end(); ++it) {
		SymbolSet::iterator eIt = it->second.find(EPSILON);
		if (eIt != it->second.end()) it->second.erase(eIt);
	}
}

void ParserGrammar::getNonTerminalFirstAndFollow(NonTerminalFirst & firsts,
				NonTerminalFollow & follows) const {
	getNonTerminalFirst(firsts);
	getNonTerminalFollow(follows, firsts);
}

ParserTable *ParserGrammar::compile(Grammar *g) const {
	ParserCompileResult compileResult;
	return compile(g, &compileResult);
}

ParserTable *ParserGrammar::compile(Grammar *g,
		ParserCompileResult *compileResult) const {
	
	return compileLL1(g, compileResult);
}

ParserTable *ParserGrammar::compileSLR1(Grammar *g) const {
	ParserCompileResult *compileResult = new ParserCompileResult();
	ParserTable *table = compileSLR1(g, compileResult);
	delete(compileResult);
	return table;
}

ParserTable *ParserGrammar::compileSLR1(Grammar *g,
		ParserCompileResult *compileResult) const {
	
	return compileSLR1(g, compileResult, NULL);
}

ParserTable *ParserGrammar::compileSLR1(Grammar *g, ParserCompileResult *compileResult,
		std::ostream & verboseOutput) const {
	
	return compileSLR1(g, compileResult, &verboseOutput);
}

ParserTable *ParserGrammar::compileSLR1(Grammar *g, ParserCompileResult *compileResult,
		std::ostream *verboseOutput) const {
	
	compileResult->setGrammar(g);
	
	NonTerminalFirst firsts;
	NonTerminalFollow follows;
	getNonTerminalFirstAndFollow(firsts, follows);
	
	SLR1TableCreationAlgorithm algorithm(grammar, g, follows, compileResult, verboseOutput);
	return algorithm.getParserTable();
}

ParserTable *ParserGrammar::compileLL1(Grammar *g) const {
	ParserCompileResult *compileResult = new ParserCompileResult();
	ParserTable *table = compileLL1(g, compileResult);
	delete(compileResult);
	return table;
}

ParserTable *ParserGrammar::compileLL1(Grammar *g, ParserCompileResult *compileResult) const {
	compileResult->setGrammar(g);
	
	ParserTableLL1 *table = new ParserTableLL1(g->getNonTerminalTypeCount(),
			g->getTokenTypeCount() + 1);
	
	// internally PARSER_END has a fixed value, convert to the correct value
	TokenTypeID parserEndId = g->getTokenTypeCount();
	
	addGrammarRules(g, table);
	
	NonTerminalFirst firsts;
	NonTerminalFollow follows;
	getNonTerminalFirstAndFollow(firsts, follows);
	
	unsigned int r = 0;
	for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it, ++r) {
		SymbolSet ruleFirsts;
		getRuleFirsts(ruleFirsts, firsts, *it);
		
		for (SymbolSet::const_iterator sIt = ruleFirsts.begin(); sIt != ruleFirsts.end(); ++sIt) {
			if (*sIt == EPSILON) continue;
			if (*sIt == PARSER_END) {
				setLL1Rule(table, compileResult, (*it)->getLeft(), parserEndId, r);
			}
			else setLL1Rule(table, compileResult, (*it)->getLeft(), *sIt, r);
		}
		
		if (ruleFirsts.find(EPSILON) != ruleFirsts.end()) {
			const SymbolSet & ruleFollows = follows[(*it)->getLeft()];
			for (SymbolSet::const_iterator sIt = ruleFollows.begin(); sIt != ruleFollows.end(); ++sIt) {
				assert(*sIt != EPSILON);
				if (*sIt == PARSER_END) {
					setLL1Rule(table, compileResult, (*it)->getLeft(), g->getTokenTypeCount(), r);
				}
				else setLL1Rule(table, compileResult, (*it)->getLeft(), *sIt, r);
			}
		}
	}
	
	return table;
}

void ParserGrammar::setLL1Rule(ParserTableLL1 *table, ParserCompileResult *compileResult,
		unsigned int i, unsigned int j, unsigned int rule) const {
	
	int oldRule = table->getRule(i, j);
	
	if (oldRule == -1 || oldRule == (int)rule) table->setRule(i, j, rule);
	else {
		if ((int)rule < oldRule) {
			// add first the rule that will be used
			compileResult->addConflictRule(i, j, grammar[rule]);
			compileResult->addConflictRule(i, j, grammar[oldRule]);
			table->setRule(i, j, rule);
		}
		else {
			// add first the rule that will be used
			compileResult->addConflictRule(i, j, grammar[oldRule]);
			compileResult->addConflictRule(i, j, grammar[rule]);
		}
	}
}

void ParserGrammar::addGrammarRules(Grammar *g, ParserTableLL1 *table) const {
	unsigned int localIndices[g->getNonTerminalTypeCount()];
	memset(localIndices, 0, g->getNonTerminalTypeCount() * sizeof(unsigned int));
	
	for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
		NonTerminalID id = (*it)->getLeft();
		assert(id < g->getNonTerminalTypeCount());
		
		unsigned int lIndex = localIndices[id]++;
		
		ParserTableLL1::Rule *rule = new ParserTableLL1::Rule(id, lIndex);
		
		const NodeTypeList nodeTypeList = (*it)->getRight();
		for (NodeTypeList::const_iterator it2 = nodeTypeList.begin(); it2 != nodeTypeList.end(); ++it2) {
			ParserTableLL1::RuleNode ruleNode;
			
			if (it2->getType() == NON_TERMINAL) {
				ruleNode.type = ParserTableLL1::RULE_NODE_NONTERMINAL;
			}
			else ruleNode.type = ParserTableLL1::RULE_NODE_TOKEN;
			
			ruleNode.id = it2->getId();
			rule->addRuleNode(ruleNode);
		}
		
		table->addRule(rule);
	}
}

/*****************************************************************************
 * First & Follow utility functions
 *****************************************************************************/
bool ParserGrammar::getFirsts(NonTerminalFirst & firsts, Rule *rule,
		const NonTerminalSet & epsilonReducers) const {
	
	const NodeTypeList & right = rule->getRight();
	
	NonTerminalID id = rule->getLeft();
	SymbolSet & idSymbols = firsts[id];
	
	if (right.empty()) {
		if (idSymbols.find(EPSILON) != idSymbols.end()) return false;
		idSymbols.insert(EPSILON);
		return true;
	}
	
	bool changed = false;
	bool epsilon = true;
	for (NodeTypeList::const_iterator it = right.begin(); it != right.end() && epsilon; ++it) {
		if (it->getType() == TOKEN) {
			if (idSymbols.find(it->getId()) == idSymbols.end()) {
				idSymbols.insert(it->getId());
				changed = true;
			}
			epsilon = false;
		}
		else {
			// copy the symbols of it (except EPSILON) to id
			const SymbolSet & symbols = firsts[it->getId()];
			for (SymbolSet::const_iterator sIt = symbols.begin(); sIt != symbols.end(); ++sIt) {
				if (idSymbols.find(*sIt) == idSymbols.end() && *sIt != EPSILON) {
					idSymbols.insert(*sIt);
					changed = true;
				}
			}
			
			if (epsilonReducers.find(it->getId()) == epsilonReducers.end()) epsilon = false;
		}
	}
	
	if (epsilon) {
		if (idSymbols.find(EPSILON) != idSymbols.end()) return changed;
		idSymbols.insert(EPSILON);
		return true;
	}
	
	return changed;
}

void ParserGrammar::getRuleFirsts(SymbolSet & ruleFirsts, const NonTerminalFirst & firsts,
		const Rule *rule) const {
	
	const NodeTypeList & right = rule->getRight();
	bool epsilon = true;
	for (NodeTypeList::const_iterator it = right.begin(); it != right.end(); ++it) {
		if (it->getType() == TOKEN) {
			ruleFirsts.insert(it->getId());
			epsilon = false;
			break;
		}
		else {
			assert(it->getType() == NON_TERMINAL);
			
			NonTerminalFirst::const_iterator firstIt = firsts.find(it->getId());
			assert(firstIt != firsts.end());
			const SymbolSet & symbols = firstIt->second;
			for (SymbolSet::const_iterator sIt = symbols.begin(); sIt != symbols.end(); ++sIt) {
				if (*sIt != EPSILON) ruleFirsts.insert(*sIt);
			}
			
			if (symbols.find(EPSILON) == symbols.end()) {
				epsilon = false;
				break;
			}
		}
	}
	
	if (epsilon) ruleFirsts.insert(EPSILON);
}

void ParserGrammar::printGrammar(std::ostream & stream, Grammar *g) const {
	for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
		stream << "<" << g->getNonTerminalName((*it)->getLeft()) << "> ::= ";
		
		const NodeTypeList & right = (*it)->getRight();
		for (NodeTypeList::const_iterator it2 = right.begin(); it2 != right.end(); ++it2) {
			if (it2 != right.begin()) stream << " ";
			
			if (it2->getType() == TOKEN) stream << g->getTokenName(it2->getId());
			else stream << "<" << g->getNonTerminalName(it2->getId()) << ">";
		}
		
		stream << ";\n";
	}
}

void ParserGrammar::sanityCheck() const {
	NonTerminalSet nonTerminals;
	getNonTerminals(nonTerminals);
	
	for (RuleList::const_iterator it = grammar.begin(); it != grammar.end(); ++it) {
		const NodeTypeList & right = (*it)->getRight();
		for (NodeTypeList::const_iterator it2 = right.begin(); it2 != right.end(); ++it2) {
			if (it2->getType() == NON_TERMINAL) {
				assert(nonTerminals.find(it2->getId()) != nonTerminals.end());
			}
			else assert(it2->getType() == TOKEN);
		}
	}
}
