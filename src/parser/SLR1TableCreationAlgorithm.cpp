#include "parser/SLR1TableCreationAlgorithm.h"

#include "parser/Grammar.h"
#include "parser/ParserCompileResult.h"
#include "parser/ParserTableSLR1.h"

#include <cassert>
#include <cstring>

/*****************************************************************************
 * SLR1TableCreationAlgorithm::Item
 *****************************************************************************/
SLR1TableCreationAlgorithm::Item::Item(const Rule *r) : rule(r), mark(0) {}

SLR1TableCreationAlgorithm::Item::Item(const Rule *r, unsigned int m) :
		rule(r), mark(m) {
	
	assert(mark <= rule->getRight().size());
}

SLR1TableCreationAlgorithm::Item SLR1TableCreationAlgorithm::Item::advanceMark() const {
	assert(!markingEnd());
	return Item(rule, mark + 1);
}

bool SLR1TableCreationAlgorithm::Item::markingNonTerminal() const {
	if (markingEnd()) return false;
	
	NodeType node = rule->getRight()[mark];
	return node.getType() == ParserGrammar::NON_TERMINAL;
}

bool SLR1TableCreationAlgorithm::Item::markingEnd() const {
	const NodeTypeList & right = rule->getRight();
	assert(mark <= right.size());
	return mark == right.size();
}

SLR1TableCreationAlgorithm::NodeType SLR1TableCreationAlgorithm::Item::getMarkedNode() const {
	const NodeTypeList & right = rule->getRight();
	assert(mark < right.size());
	
	return right[mark];
}

const SLR1TableCreationAlgorithm::Rule *SLR1TableCreationAlgorithm::Item::getRule() const {
	return rule;
}

unsigned int SLR1TableCreationAlgorithm::Item::getMark() const {
	return mark;
}

bool SLR1TableCreationAlgorithm::Item::operator==(const Item & other) const {
	return rule == other.rule && mark == other.mark;
}

bool SLR1TableCreationAlgorithm::Item::operator<(const Item & other) const {
	return rule < other.rule || (rule == other.rule && mark < other.mark);
}

void SLR1TableCreationAlgorithm::Item::printItem(std::ostream & stream, Grammar *grammar) const {
	stream << "<" << grammar->getNonTerminalName(rule->getLeft()) << "> ::= ";
	
	const NodeTypeList & nodeList = rule->getRight();
	unsigned int pos = 0;
	for (NodeTypeList::const_iterator it = nodeList.begin(); it != nodeList.end(); ++it) {
		if (pos++ == mark) stream << ". ";
		
		if (it->getType() == ParserGrammar::NON_TERMINAL) {
			stream << "<" << grammar->getNonTerminalName(it->getId()) << ">";
		}
		else stream << grammar->getTokenName(it->getId());
		
		stream << " ";
	}
	
	if (pos == mark) stream << ". ";
	stream << ";";
}

/*****************************************************************************
 * SLR1TableCreationAlgorithm::KernelClosure
 *****************************************************************************/
SLR1TableCreationAlgorithm::KernelClosure::KernelClosure(
		SLR1TableCreationAlgorithm *algo, const Kernel & k) : algorithm(algo),
		kernel(k) {
	
	for (Kernel::const_iterator it = kernel.begin(); it != kernel.end(); ++it) {
		getClosure(*it);
	}
}

const SLR1TableCreationAlgorithm::ItemSet & SLR1TableCreationAlgorithm::KernelClosure::getClosure() const {
	return closure;
}

void SLR1TableCreationAlgorithm::KernelClosure::getSuccessors(NodeToKernel & successors) const {
	for (ItemSet::const_iterator it = closure.begin(); it != closure.end(); ++it) {
		if (!it->markingEnd()) {
			successors[it->getMarkedNode()].insert(it->advanceMark());
		}
	}
}

void SLR1TableCreationAlgorithm::KernelClosure::getClosure(const Item & item) {
	if (closure.find(item) != closure.end()) return;
	
	closure.insert(item);
	
	if (item.markingNonTerminal()) {
		NonTerminalID id = item.getMarkedNode().getId();
		
		const RuleList & ruleList = algorithm->nonTerminalRules[id];
		for (RuleList::const_iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
			getClosure(Item(*it));
		}
	}
}

/*****************************************************************************
 * SLR1TableCreationAlgorithm
 *****************************************************************************/
SLR1TableCreationAlgorithm::SLR1TableCreationAlgorithm(const RuleList & rList,
		Grammar *g, NonTerminalFollow & follow, ParserCompileResult *compResult,
		std::ostream *verbose) : ruleList(rList), grammar(g),
		follows(follow), compileResult(compResult), verboseOutput(verbose),
		parserTable(NULL), initialKernel(NULL), validGrammar(true) {
	
	createFakeRootRule();
	
	getNonTerminalRules();
	
	createKernels();
	createKernelIndex();
	createTable();
	
	assert(parserTable);
	
	if (!validGrammar) {
		delete(parserTable);
		throw ParserError("Invalid grammar.");
	}
}

SLR1TableCreationAlgorithm::~SLR1TableCreationAlgorithm() {
	delete(fakeRule);
}

ParserTableSLR1 *SLR1TableCreationAlgorithm::getParserTable() const {
	return parserTable;
}

SLR1TableCreationAlgorithm::NonTerminalID SLR1TableCreationAlgorithm::getFakeRootId() const {
	return grammar->getNonTerminalTypeCount();
}

unsigned int SLR1TableCreationAlgorithm::getParserEndToken() const {
	return grammar->getTokenTypeCount();
}

void SLR1TableCreationAlgorithm::getNonTerminalRules() {
	for (RuleList::const_iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
		nonTerminalRules[(*it)->getLeft()].push_back(*it);
	}
}

void SLR1TableCreationAlgorithm::createFakeRootRule() {
	NodeTypeList fakeRuleNodes;
	fakeRuleNodes.push_back(NodeType(ParserGrammar::NON_TERMINAL, 0));
	fakeRuleNodes.push_back(NodeType(ParserGrammar::TOKEN, getParserEndToken()));
	fakeRule = new Rule(getFakeRootId(), fakeRuleNodes);
}

void SLR1TableCreationAlgorithm::createKernels() {
	createInitialKernel();
	
	KernelQueue toExpand;
	toExpand.push(*initialKernel);
	
	while (!toExpand.empty()) {
		Kernel kernel = toExpand.front();
		toExpand.pop();
		
		KernelClosure kernelClosure(this, kernel);
		
		NodeToKernel successors;
		kernelClosure.getSuccessors(successors);
		
		for (NodeToKernel::const_iterator it = successors.begin();
				it != successors.end(); ++it) {
			
			if (kernels.find(it->second) == kernels.end()) {
				kernels.insert(it->second);
				toExpand.push(it->second);
			}
		}
	}
}

void SLR1TableCreationAlgorithm::createInitialKernel() {
	Kernel k;
	k.insert(Item(fakeRule, 0));
	kernels.insert(k);
	
	assert(kernels.size() == 1);
	initialKernel = &*kernels.begin();
}

void SLR1TableCreationAlgorithm::createKernelIndex() {
	assert(kernelIndex.empty());
	
	// initialKernel must be 0
	assert(initialKernel);
	kernelIndex[*initialKernel] = 0;
	
	unsigned int index = 1;
	for (KernelSet::const_iterator it = kernels.begin(); it != kernels.end(); ++it) {
		if (&*it != initialKernel) kernelIndex[*it] = index++;
	}
}

void SLR1TableCreationAlgorithm::createTable() {
	assert(!parserTable);
	assert(kernelIndex.size() == kernels.size());
	
	parserTable = new ParserTableSLR1(kernels.size(), grammar->getNonTerminalTypeCount() + 1,
			grammar->getTokenTypeCount() + 1);
	
	addGrammarRules();
	
	for (KernelSet::const_iterator it = kernels.begin(); it != kernels.end(); ++it) {
		assert(kernelIndex.find(*it) != kernelIndex.end());
		
		KernelClosure kernelClosure(this, *it);
		NodeToKernel successors;
		kernelClosure.getSuccessors(successors);
		
		setupTableForItemSet(kernelClosure.getClosure(), successors, kernelIndex[*it], *it);
	}
}

void SLR1TableCreationAlgorithm::insertHaltAction(unsigned int state) {
	ParserTableSLR1::Action action(ParserTableSLR1::HALT);
	parserTable->setAction(action, state, getParserEndToken());
	
	if (verboseOutput) *verboseOutput << "\tOn EOF, Halt\n";
}

void SLR1TableCreationAlgorithm::setupTableForItemSet(const ItemSet & itemSet,
		NodeToKernel & successors, unsigned int state, const Kernel & kernel) {
	
	if (verboseOutput) printStateInfo(state, itemSet, kernel);
	
	for (ItemSet::const_iterator it = itemSet.begin(); it != itemSet.end(); ++it) {
		if (it->markingEnd()) {
			setupActionsForEndingRule(state, it->getRule());
		}
		else {
			NodeType node = it->getMarkedNode();
			unsigned int targetState = kernelIndex[successors[node]];
			
			if (node.getType() == ParserGrammar::TOKEN) {
				setShiftAction(state, node.getId(), targetState);
			}
			else setGotoAction(state, node.getId(), targetState);
		}
	}
	
	if (verboseOutput) *verboseOutput << "\n\n";
}

void SLR1TableCreationAlgorithm::setupActionsForEndingRule(unsigned int state, const Rule *rule) {
	unsigned int rIndex = ruleIndex[rule];
	
	const SymbolSet & ruleFollows = follows[rule->getLeft()];
	for (SymbolSet::const_iterator sIt = ruleFollows.begin();
			sIt != ruleFollows.end(); ++sIt) {
		
		setReduceAction(state, *sIt, rIndex);
	}
}

void SLR1TableCreationAlgorithm::printStateInfo(unsigned int state,
		const ItemSet & itemSet, const Kernel & kernel) {
	
	*verboseOutput << "State " << state << "\n";
	
	for (Kernel::const_iterator it = kernel.begin(); it != kernel.end(); ++it) {
		*verboseOutput << "\t";
		it->printItem(*verboseOutput, grammar);
		*verboseOutput << "\n";
	}
	
	*verboseOutput << "\n";
}

void SLR1TableCreationAlgorithm::setShiftAction(unsigned int state,
		unsigned int tok, unsigned int targetState) {
	
	if (tok != PARSER_END && tok != grammar->getTokenTypeCount()) {
		assert(tok < grammar->getTokenTypeCount());
		
		ParserTableSLR1::Action action(ParserTableSLR1::SHIFT, targetState);
		const ParserTableSLR1::Action & oldAct = parserTable->getAction(state, tok);
		if (action == oldAct) return;
		
		if (!oldAct.isErrorAction()) {
			if (oldAct.getType() == ParserTableSLR1::SHIFT) {
				// we don't known the rule of this shift, so leave it as NULL
				validGrammar = false;
				compileResult->addConflictRule(state, tok, NULL);
				
				// shift/shift conflict
				// since we don't known a rule to put in the conflict
				// let's add nothing
			}
			else {
				assert(oldAct.getType() == ParserTableSLR1::REDUCE);
				
				// shift/reduce conflict
				// we will keep the shift
				assert(oldAct.getNum() < ruleList.size());
				compileResult->addConflictRule(state, tok, ruleList[oldAct.getNum()]);
				compileResult->addConflictRule(state, tok, NULL);
			}
		}
		parserTable->setAction(action, state, tok);
		
		if (verboseOutput) {
			*verboseOutput << "\tOn " << grammar->getTokenName(tok)
					<< ", shift and goto " << targetState << "\n";
		}
	}
	else insertHaltAction(state);
}

void SLR1TableCreationAlgorithm::setGotoAction(unsigned int state,
		unsigned int nonTerminal, unsigned int targetState) {
	
	assert(parserTable->getGoto(state, nonTerminal) == -1
			|| parserTable->getGoto(state, nonTerminal) == (int)targetState);
	
	parserTable->setGoto(targetState, state, nonTerminal);
	
	if (verboseOutput) {
		*verboseOutput << "\tOn <" << grammar->getNonTerminalName(nonTerminal)
				<< ">, goto " << targetState << "\n";
	}
}

void SLR1TableCreationAlgorithm::setReduceAction(unsigned int state,
		unsigned int tok, unsigned int rule) {
	
	if (tok == PARSER_END) tok = getParserEndToken();
	
	bool updateTable = true;
	
	ParserTableSLR1::Action action(ParserTableSLR1::REDUCE, rule);
	const ParserTableSLR1::Action & oldAct = parserTable->getAction(state, tok);
	if (action == oldAct) return;
	
	if (!oldAct.isErrorAction()) {
		if (oldAct.getType() == ParserTableSLR1::SHIFT) {
			// shift/reduce conflict
			compileResult->addConflictRule(state, tok, NULL);
			compileResult->addConflictRule(state, tok, ruleList[rule]);
			
			// we will keep the shift
			updateTable = false;
		}
		else if (oldAct.getType() == ParserTableSLR1::REDUCE) {
			// reduce/reduce conflict
			assert(oldAct.getNum() < ruleList.size());
			compileResult->addConflictRule(state, tok, ruleList[oldAct.getNum()]);
			compileResult->addConflictRule(state, tok, ruleList[rule]);
			
			// we will keep with the rule that comes first
			updateTable = rule < oldAct.getNum();
		}
		else {
			assert(oldAct.getType() == ParserTableSLR1::HALT);
			
			validGrammar = false;
			
			// halt/reduce conflict
			compileResult->addConflictRule(state, tok, ruleList[oldAct.getNum()]);
			compileResult->addConflictRule(state, tok, NULL);
			
			// we will keep the halt
			updateTable = false;
		}
	}
	
	if (updateTable) parserTable->setAction(action, state, tok);
	
	if (verboseOutput) {
		if (tok == getParserEndToken()) {
			*verboseOutput << "\tOn parser end, reduce with rule " << rule << "\n";
		}
		else {
			*verboseOutput << "\tOn " << grammar->getTokenName(tok)
					<< ", reduce with rule " << rule << "\n";
		}
	}
}

void SLR1TableCreationAlgorithm::addGrammarRules() {
	unsigned int localIndices[grammar->getNonTerminalTypeCount()];
	memset(localIndices, 0, grammar->getNonTerminalTypeCount() * sizeof(unsigned int));
	
	unsigned int index = 0;
	
	for (RuleList::const_iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
		ruleIndex[*it] = index++;
		
		NonTerminalID id = (*it)->getLeft();
		assert(id < grammar->getNonTerminalTypeCount());
		
		unsigned int lIndex = localIndices[id]++;
		
		ParserTableSLR1::Rule *rule = new ParserTableSLR1::Rule(id, lIndex);
		
		const NodeTypeList nodeTypeList = (*it)->getRight();
		for (NodeTypeList::const_iterator it2 = nodeTypeList.begin(); it2 != nodeTypeList.end(); ++it2) {
			ParserTableSLR1::RuleNode ruleNode;
			
			if (it2->getType() == ParserGrammar::NON_TERMINAL) {
				ruleNode.type = ParserTableSLR1::RULE_NODE_NONTERMINAL;
			}
			else ruleNode.type = ParserTableSLR1::RULE_NODE_TOKEN;
			
			ruleNode.id = it2->getId();
			rule->addRuleNode(ruleNode);
		}
		
		parserTable->addRule(rule);
	}
}
