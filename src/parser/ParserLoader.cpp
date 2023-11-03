#include "parser/ParserLoader.h"

#include "parser/Automata.h"
#include "parser/Grammar.h"
#include "parser/ParserDefs.h"
#include "parser/ParserTable.h"
#include "parser/ParserTableLL1.h"
#include "parser/ParserTableSLR1.h"
#include "parser/ScannerAutomata.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdint.h>

enum TableType {
	TABLE_LL1 = 0,
	TABLE_SLR1 = 1
};

ParserLoader::ParserLoader() {}

/*****************************************************************************
 * Parser load functions
 *****************************************************************************/
static ParserTable *loadTableLL1(const unsigned char *buffer);
static ParserTable *loadTableSLR1(const unsigned char *buffer);

/*****************************************************************************
 * Parser save functions
 *****************************************************************************/
static unsigned char *saveTableLL1(const ParserTableLL1 *table, unsigned int *bufferSize);
static unsigned char *saveTableSLR1(const ParserTableSLR1 *table, unsigned int *bufferSize);

/*****************************************************************************
 * Buffer save functions
 *****************************************************************************/
static void getGrammarTokNtNames(Grammar *grammar, std::vector<const char *> & tokenNames,
		std::vector<const char *> & ntNames, unsigned int *bufferSize);

static void writeGrammarTokenNtNames(unsigned char *buffer, unsigned int & pos,
		const std::vector<const char *> & tokenNames, const std::vector<const char *> & ntNames);

/*****************************************************************************
 * Auxiliar functions
 *****************************************************************************/
template<class T>
inline static T readFromBuffer(const void *buffer, unsigned int & pos);

template<>
inline std::string readFromBuffer(const void *buffer, unsigned int & pos);

template<class T>
inline static void writeToBuffer(void *buffer, unsigned int & pos, T value);

template<>
inline void writeToBuffer(void *buffer, unsigned int & pos, const char *value);

// buf value can be null
static void writeToBuffer(void *buffer, unsigned int & pos, const void *bufValue, unsigned int size);

/*****************************************************************************
 * Load functions
 *****************************************************************************/
// See ParserLoader::automataToBuffer for buffer format
ScannerAutomata *ParserLoader::bufferToAutomata(const unsigned char *buffer) {
	unsigned int pos = 0;
	unsigned int numStates = readFromBuffer<uint32_t>(buffer, pos);
	unsigned int finalStatesSize = numStates / 32 + (numStates % 32 == 0 ? 0 : 1);
	
	// read the transitions
	int **transitions;
	transitions = new int *[ALPHABET_SIZE];
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) {
		transitions[i] = new int[numStates];
		
		for (unsigned int j = 0; j < numStates; ++j) {
			transitions[i][j] = readFromBuffer<int32_t>(buffer, pos);
		}
	}
	Automata *automata = new Automata(transitions, numStates);
	
	// the automata will copy the transitions, so we can delete it
	for (unsigned int i = 0; i < ALPHABET_SIZE; ++i) delete[](transitions[i]);
	delete[](transitions);
	
	// read the final states
	unsigned int state = 0;
	for (unsigned int i = 0; i < finalStatesSize; ++i) {
		assert(state < numStates);
		uint32_t value = readFromBuffer<uint32_t>(buffer, pos);
		
		for (unsigned int bit = 0; bit < 32 && state < numStates; ++bit) {
			if (value & (1 << bit)) automata->setFinalState(state);
			++state;
		}
	}
	
	ScannerAutomata *scannerAutomata = new ScannerAutomata(automata);
	
	// read stateTokenTypeId
	for (unsigned int i = 0; i < numStates; ++i) {
		int tokenId = readFromBuffer<int32_t>(buffer, pos);
		if (tokenId != -1) scannerAutomata->setStateTokenTypeId(i, tokenId);
	}
	
	unsigned int numIgnoredTokens = readFromBuffer<uint32_t>(buffer, pos);
	for (unsigned int i = 0; i < numIgnoredTokens; ++i) {
		unsigned int ignored = readFromBuffer<uint32_t>(buffer, pos);
		scannerAutomata->addIgnoredToken(ignored);
	}
	
	return scannerAutomata;
}

// See ParserLoader::tableToBuffer for buffer format
ParserTable *ParserLoader::bufferToTable(const unsigned char *buffer) {
	// the first 4 bytes contains the table type
	unsigned int pos = 0;
	uint32_t type = readFromBuffer<uint32_t>(buffer, pos);
	
	ParserTable *table = NULL;
	
	switch (type) {
		case TABLE_LL1:
			table = loadTableLL1(buffer);
			break;
		case TABLE_SLR1:
			table = loadTableSLR1(buffer);
			break;
		default:
			abort();
	}
	
	return table;
}

// See ParserLoader::saveToBuffer for buffer format
void ParserLoader::loadBuffer(const unsigned char *buffer, Grammar **grammar,
		ScannerAutomata **scannerAutomata, ParserTable **parserTable) {
	
	unsigned int pos = 0;
	
	Grammar *g = new Grammar();
	ScannerAutomata *automata = NULL;
	ParserTable *table = NULL;
	
	unsigned int tokenCount = readFromBuffer<uint32_t>(buffer, pos);
	for (unsigned int i = 0; i < tokenCount; ++i) {
		std::string name = readFromBuffer<std::string>(buffer, pos);
		assert(!g->hasTokenName(name));
		g->getOrCreateTokenId(name);
	}
	
	unsigned int ntCount = readFromBuffer<uint32_t>(buffer, pos);
	for (unsigned int i = 0; i < ntCount; ++i) {
		std::string name = readFromBuffer<std::string>(buffer, pos);
		g->createNonTerminalId(name);
	}
	
	unsigned int automataSize = readFromBuffer<uint32_t>(buffer, pos);
	if (automataSize) {
		automata = bufferToAutomata(buffer + pos);
		pos += automataSize;
	}
	
	unsigned int tableSize = readFromBuffer<uint32_t>(buffer, pos);
	if (tableSize) {
		table = bufferToTable(buffer + pos);
		pos += tableSize;
	}
	
	if (grammar) *grammar = g;
	else delete(g);
	if (scannerAutomata) *scannerAutomata = automata;
	else delete(automata);
	if (parserTable) *parserTable = table;
	else delete(table);
}

/*****************************************************************************
 * Save functions
 *****************************************************************************/
/*
 * Buffer format:
 * struct {
 * 		uint32_t numStates;
 * 		int32_t transitions[ALPHABET_SIZE][numStates];
 * 		
 * 		// each bit is one boolean
 * 		uint32_t finalStates[numStates / 32 + (numStates % 32 == 0 ? 0 : 1)];
 * 		
 * 		int32_t stateTokenTypeId[numStates];
 * 		
 * 		uint32_t numIgnoredTokens;
 * 		uint32_t ignoredTokens[numIgnoredTokens];
 * };
 */
unsigned char *ParserLoader::automataToBuffer(const ScannerAutomata *scannerAutomata,
		unsigned int *bufferSize) {
	Automata *automata = scannerAutomata->getAutomata();
	
	unsigned int numStates = automata->getNumStates();
	unsigned int finalStatesSize = numStates / 32 + (numStates % 32 == 0 ? 0 : 1);
	
	const ScannerAutomata::TokenTypeIDSet & ignoredTokens = scannerAutomata->getIgnoredTokens();
	unsigned int numIgnoredTokens = ignoredTokens.size();
	
	*bufferSize = sizeof(uint32_t); // numStates
	*bufferSize += sizeof(int32_t) * ALPHABET_SIZE * numStates; // transitions
	*bufferSize += sizeof(uint32_t) * finalStatesSize; // finalStates
	*bufferSize += sizeof(int32_t) * numStates; // stateTokenTypeId
	*bufferSize += sizeof(uint32_t); // numIgnoredTokens
	*bufferSize += sizeof(uint32_t) * numIgnoredTokens; // ignoredTokens
	
	unsigned char *buffer = new unsigned char[*bufferSize];
	unsigned int pos = 0;
	
	writeToBuffer(buffer, pos, (uint32_t)numStates);
	
	// save the transitionTable
	const int * const *transitionTable = automata->getTransitionTable();
	for (unsigned int input = 0; input < ALPHABET_SIZE; ++input) {
		for (unsigned int state = 0; state < numStates; ++state) {
			writeToBuffer(buffer, pos, (int32_t)transitionTable[input][state]);
		}
	}
	
	// save the final states
	unsigned int bit = 0;
	uint32_t value = 0;
	for (unsigned int i = 0; i < numStates; ++i) {
		if (automata->isFinalState(i)) value |= 1 << bit;
		++bit;
		
		if (bit >= 32) {
			writeToBuffer(buffer, pos, value);
			bit = 0;
			value = 0;
		}
	}
	if (bit > 0) writeToBuffer(buffer, pos, value);
	
	// save stateTokenTypeId
	for (unsigned int i = 0; i < numStates; ++i) {
		writeToBuffer(buffer, pos, (int32_t)scannerAutomata->getStateTokenTypeId(i));
	}
	
	// save ignored tokens
	writeToBuffer(buffer, pos, (uint32_t)numIgnoredTokens);
	for (ScannerAutomata::TokenTypeIDSet::const_iterator it = ignoredTokens.begin();
			it != ignoredTokens.end(); ++it) {
		writeToBuffer(buffer, pos, (uint32_t)*it);
	}
	
	assert(pos == *bufferSize);
	
	return buffer;
}

unsigned char *ParserLoader::tableToBuffer(const ParserTable *parserTable,
		unsigned int *bufferSize) {
	
	unsigned char *buffer = NULL;
	
	if (dynamic_cast<const ParserTableLL1 *>(parserTable)) {
		buffer = saveTableLL1((const ParserTableLL1 *)parserTable, bufferSize);
	}
	else if (dynamic_cast<const ParserTableSLR1 *>(parserTable)) {
		buffer = saveTableSLR1((const ParserTableSLR1 *)parserTable, bufferSize);
	}
	else abort();
	
	return buffer;
}

/*
 * Buffer format:
 * struct {
 * 		uint32_t tokenCount;
 * 		const char *tokenNames[tokenCount]; // null-terminated string
 * 		
 * 		uint32_t nonTerminalCount;
 * 		const char *nonTerminalNames[tokenCount]; // null-terminated string
 * 		
 * 		uint32_t automataBufferSize; // 0 if no automata is present
 * 		struct Automata; // from automataToBuffer
 * 		
 * 		uint32_t tableBufferSize; //0 if no table is present
 * 		struct Table; // from tableToBuffer
 * }
 */
unsigned char *ParserLoader::saveBuffer(Grammar *grammar, ScannerAutomata *scannerAutomata,
		ParserTable *parserTable, unsigned int *bufferSize) {
	
	*bufferSize = 0;
	
	std::vector<const char *> tokenNames, ntNames;
	getGrammarTokNtNames(grammar, tokenNames, ntNames, bufferSize);
	
	unsigned char *automataBuffer = NULL, *tableBuffer = NULL;
	uint32_t automataBufferSize = 0, tableBufferSize = 0;
	
	*bufferSize += sizeof(uint32_t); // automataBufferSize
	if (scannerAutomata) {
		unsigned int size;
		automataBuffer = automataToBuffer(scannerAutomata, &size);
		automataBufferSize = size;
		*bufferSize += automataBufferSize;
	}
	
	*bufferSize += sizeof(uint32_t); // tableBufferSize
	if (parserTable) {
		unsigned int size;
		tableBuffer = tableToBuffer(parserTable, &size);
		tableBufferSize = size;
		*bufferSize += tableBufferSize;
	}
	
	unsigned char *buffer = new unsigned char[*bufferSize];
	unsigned int pos = 0;
	
	writeGrammarTokenNtNames(buffer, pos, tokenNames, ntNames);
	writeToBuffer(buffer, pos, automataBufferSize);
	writeToBuffer(buffer, pos, automataBuffer, automataBufferSize);
	writeToBuffer(buffer, pos, tableBufferSize);
	writeToBuffer(buffer, pos, tableBuffer, tableBufferSize);
	
	delete[](automataBuffer);
	delete[](tableBuffer);
	
	assert(pos == *bufferSize);
	
	return buffer;
}

/*****************************************************************************
 * Parser load functions
 *****************************************************************************/
// See saveTableLL1 for buffer format
static ParserTable *loadTableLL1(const unsigned char *buffer) {
	unsigned int pos = 0;
	
	unsigned int tableType = readFromBuffer<uint32_t>(buffer, pos);
	if (tableType != TABLE_LL1) abort();
	
	ParserTable::NonTerminalID rootNonTerminalId = readFromBuffer<uint32_t>(buffer, pos);
	unsigned int parserTableRows = readFromBuffer<uint32_t>(buffer, pos);
	unsigned int parserTableCols = readFromBuffer<uint32_t>(buffer, pos);
	
	ParserTableLL1 *table = new ParserTableLL1(parserTableRows, parserTableCols);
	table->setRootNonTerminalId(rootNonTerminalId);
	
	for (unsigned int i = 0; i < parserTableRows; ++i) {
		for (unsigned int j = 0; j < parserTableCols; ++j) {
			table->setRule(i, j, readFromBuffer<int32_t>(buffer, pos));
		}
	}
	
	unsigned int ruleListSize = readFromBuffer<uint32_t>(buffer, pos);
	for (unsigned int i = 0; i < ruleListSize; ++i) {
		ParsingTree::NonTerminalID nonTerminalId = readFromBuffer<uint32_t>(buffer, pos);
		unsigned int localIndex = readFromBuffer<uint32_t>(buffer, pos);
		unsigned int numRuleNodes = readFromBuffer<uint32_t>(buffer, pos);
		
		ParserTableLL1::Rule *rule = new ParserTableLL1::Rule(nonTerminalId, localIndex);
		for (unsigned int j = 0; j < numRuleNodes; ++j) {
			ParserTableLL1::RuleNode node;
			node.type = (ParserTableLL1::RuleNodeType)readFromBuffer<uint32_t>(buffer, pos);
			node.id = readFromBuffer<uint32_t>(buffer, pos);
			rule->addRuleNode(node);
		}
		
		table->addRule(rule);
	}
	
	return table;
}

// See saveTableSLR1 for buffer format
static ParserTable *loadTableSLR1(const unsigned char *buffer) {
	unsigned int pos = 0;
	
	unsigned int tableType = readFromBuffer<uint32_t>(buffer, pos);
	if (tableType != TABLE_SLR1) abort();
	
	ParserTable::NonTerminalID rootNonTerminalId = readFromBuffer<uint32_t>(buffer, pos);
	unsigned int numStates = readFromBuffer<uint32_t>(buffer, pos);
	unsigned int numNonTerminals = readFromBuffer<uint32_t>(buffer, pos);
	unsigned int numTokens = readFromBuffer<uint32_t>(buffer, pos);
	
	ParserTableSLR1 *table = new ParserTableSLR1(numStates, numNonTerminals, numTokens);
	table->setRootNonTerminalId(rootNonTerminalId);
	
	for (unsigned int i = 0; i < numStates; ++i) {
		for (unsigned int j = 0; j < numNonTerminals; ++j) {
			table->setGoto(readFromBuffer<int32_t>(buffer, pos), i, j);
		}
	}
	
	for (unsigned int i = 0; i < numStates; ++i) {
		for (unsigned int j = 0; j < numTokens; ++j) {
			unsigned int type = readFromBuffer<uint32_t>(buffer, pos);
			unsigned int num = readFromBuffer<uint32_t>(buffer, pos);
			ParserTableSLR1::Action action((ParserTableSLR1::ActionType)type, num);
			table->setAction(action, i, j);
		}
	}
	
	unsigned int ruleListSize = readFromBuffer<uint32_t>(buffer, pos);
	for (unsigned int i = 0; i < ruleListSize; ++i) {
		ParsingTree::NonTerminalID nonTerminalId = readFromBuffer<uint32_t>(buffer, pos);
		unsigned int localIndex = readFromBuffer<uint32_t>(buffer, pos);
		unsigned int numRuleNodes = readFromBuffer<uint32_t>(buffer, pos);
		
		ParserTableSLR1::Rule *rule = new ParserTableSLR1::Rule(nonTerminalId, localIndex);
		for (unsigned int j = 0; j < numRuleNodes; ++j) {
			ParserTableSLR1::RuleNode node;
			node.type = (ParserTableSLR1::RuleNodeType)readFromBuffer<uint32_t>(buffer, pos);
			node.id = readFromBuffer<uint32_t>(buffer, pos);
			rule->addRuleNode(node);
		}
		
		table->addRule(rule);
	}
	
	return table;
}

/*****************************************************************************
 * Parser save functions
 *****************************************************************************/
/*
 * Buffer format:
 * struct {
 * 		uint32_t tableType;
 * 		
 * 		uint32_t rootNonTerminalId;
 * 		
 * 		uint32_t parserTableRows;
 * 		uint32_t parserTableCols;
 * 		int32_t parserTable[parserTableRows][parserTableCols];
 * 		
 * 		uint32_t ruleListSize;
 * 		Rule rules[ruleListSize];
 * };
 * 
 * struct Rule {
 * 		uint32_t ruleNonTerminaId;
 * 		uint32_t localIndex;
 * 		uint32_t numRuleNodes;
 * 		RuleNode ruleNodes[numRuleNodes];
 * };
 * 
 * struct RuleNode {
 * 		uint32_t type;
 * 		uint32_t id;
 * };
 */
static unsigned char *saveTableLL1(const ParserTableLL1 *table, unsigned int *bufferSize) {
	uint32_t tableType = TABLE_LL1;
	uint32_t rootNonTerminalId = table->getRootNonTerminalId();
	
	uint32_t parserTableRows = table->getParserTableRows();
	uint32_t parserTableCols = table->getParserTableCols();
	
	const ParserTableLL1::RuleList & ruleList = table->getRuleList();
	uint32_t ruleListSize = ruleList.size();
	
	*bufferSize = sizeof(uint32_t); // tableType
	*bufferSize += sizeof(uint32_t); // rootNonTerminalId
	
	*bufferSize += sizeof(uint32_t); // parserTableRows
	*bufferSize += sizeof(uint32_t); // parserTableCols
	*bufferSize += parserTableRows * parserTableCols * sizeof(int32_t); // parserTable
	
	*bufferSize += sizeof(uint32_t); // ruleListSize
	unsigned int ruleNodeSize = 2 * sizeof(uint32_t);
	for (ParserTableLL1::RuleList::const_iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
		*bufferSize += sizeof(uint32_t); // ruleNonTerminaId
		*bufferSize += sizeof(uint32_t); // localIndex
		*bufferSize += sizeof(uint32_t); // numRuleNodes
		*bufferSize += (*it)->getRuleNodes().size() * ruleNodeSize;
	}
	
	unsigned int pos = 0;
	unsigned char *buffer = new unsigned char[*bufferSize];
	
	writeToBuffer(buffer, pos, tableType);
	writeToBuffer(buffer, pos, rootNonTerminalId);
	writeToBuffer(buffer, pos, parserTableRows);
	writeToBuffer(buffer, pos, parserTableCols);
	
	int **parserTable = table->getParserTable();
	for (unsigned int i = 0; i < parserTableRows; ++i) {
		for (unsigned int j = 0; j < parserTableCols; ++j) {
			writeToBuffer(buffer, pos, (int32_t)parserTable[i][j]);
		}
	}
	
	writeToBuffer(buffer, pos, (uint32_t)ruleListSize);
	for (unsigned int i = 0; i < ruleListSize; ++i) {
		const ParserTableLL1::Rule *rule = ruleList[i];
		
		const ParserTableLL1::RuleNodeList & nodeList = rule->getRuleNodes();
		
		uint32_t nonTerminalId = rule->getNonTerminalId();
		uint32_t localIndex = rule->getLocalIndex();
		uint32_t ruleSize = nodeList.size();
		
		writeToBuffer(buffer, pos, nonTerminalId);
		writeToBuffer(buffer, pos, localIndex);
		writeToBuffer(buffer, pos, ruleSize);
		
		for (unsigned int j = 0; j < ruleSize; ++j) {
			writeToBuffer(buffer, pos, (uint32_t)nodeList[j].type);
			writeToBuffer(buffer, pos, (uint32_t)nodeList[j].id);
		}
	}
	
	assert(*bufferSize == pos);
	return buffer;
}

/*
 * Buffer format:
 * struct {
 * 		uint32_t tableType;
 * 		
 * 		uint32_t rootNonTerminalId;
 * 		
 * 		uint32_t numStates;
 * 		uint32_t numNonTerminals;
 * 		uint32_t numTokens;
 * 		
 * 		int32_t gotoTable[numStates][numNonTerminals];
 * 		Action actionTable[numStates][numTokens];
 * 		
 * 		uint32_t ruleListSize;
 * 		Rule rules[ruleListSize];
 * };
 * 
 * struct Action {
 * 		uint32_t type;
 * 		uint32_t num;
 * };
 * 
 * struct Rule {
 * 		uint32_t ruleNonTerminaId;
 * 		uint32_t localIndex;
 * 		uint32_t numRuleNodes;
 * 		RuleNode ruleNodes[numRuleNodes];
 * };
 * 
 * struct RuleNode {
 * 		uint32_t type;
 * 		uint32_t id;
 * };
 */
static unsigned char *saveTableSLR1(const ParserTableSLR1 *table, unsigned int *bufferSize) {
	uint32_t tableType = TABLE_SLR1;
	uint32_t rootNonTerminalId = table->getRootNonTerminalId();
	
	uint32_t numStates = table->getNumStates();
	uint32_t numNonTerminals = table->getNumNonTerminals();
	uint32_t numTokens = table->getNumTokens();
	
	const ParserTableSLR1::RuleList & ruleList = table->getRuleList();
	uint32_t ruleListSize = ruleList.size();
	
	*bufferSize = sizeof(uint32_t); // tableType
	*bufferSize += sizeof(uint32_t); // rootNonTerminalId
	*bufferSize += sizeof(uint32_t); // numStates
	*bufferSize += sizeof(uint32_t); // numNonTerminals
	*bufferSize += sizeof(uint32_t); // numTokens
	
	*bufferSize += numStates * numNonTerminals * sizeof(int32_t); // gotoTable
	
	unsigned int actionSize = 2 * sizeof(uint32_t);
	*bufferSize += numStates * numTokens * actionSize; // actionTable
	
	*bufferSize += sizeof(uint32_t); // ruleListSize
	unsigned int ruleNodeSize = 2 * sizeof(uint32_t);
	for (ParserTableSLR1::RuleList::const_iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
		*bufferSize += sizeof(uint32_t); // ruleNonTerminaId
		*bufferSize += sizeof(uint32_t); // localIndex
		*bufferSize += sizeof(uint32_t); // numRuleNodes
		*bufferSize += (*it)->getRuleNodes().size() * ruleNodeSize;
	}
	
	unsigned int pos = 0;
	unsigned char *buffer = new unsigned char[*bufferSize];
	
	writeToBuffer(buffer, pos, tableType);
	writeToBuffer(buffer, pos, rootNonTerminalId);
	writeToBuffer(buffer, pos, numStates);
	writeToBuffer(buffer, pos, numNonTerminals);
	writeToBuffer(buffer, pos, numTokens);
	
	int **gotoTable = table->getGotoTable();
	for (unsigned int i = 0; i < numStates; ++i) {
		for (unsigned int j = 0; j < numNonTerminals; ++j) {
			writeToBuffer(buffer, pos, (int32_t)gotoTable[i][j]);
		}
	}
	
	ParserTableSLR1::Action **actionTable = table->getActionTable();
	for (unsigned int i = 0; i < numStates; ++i) {
		for (unsigned int j = 0; j < numTokens; ++j) {
			writeToBuffer(buffer, pos, (int32_t)actionTable[i][j].getType());
			writeToBuffer(buffer, pos, (int32_t)actionTable[i][j].getNum());
		}
	}
	
	writeToBuffer(buffer, pos, (uint32_t)ruleListSize);
	for (unsigned int i = 0; i < ruleListSize; ++i) {
		const ParserTableSLR1::Rule *rule = ruleList[i];
		
		const ParserTableSLR1::RuleNodeList & nodeList = rule->getRuleNodes();
		
		uint32_t nonTerminalId = rule->getNonTerminalId();
		uint32_t localIndex = rule->getLocalIndex();
		uint32_t ruleSize = nodeList.size();
		
		writeToBuffer(buffer, pos, nonTerminalId);
		writeToBuffer(buffer, pos, localIndex);
		writeToBuffer(buffer, pos, ruleSize);
		
		for (unsigned int j = 0; j < ruleSize; ++j) {
			writeToBuffer(buffer, pos, (uint32_t)nodeList[j].type);
			writeToBuffer(buffer, pos, (uint32_t)nodeList[j].id);
		}
	}
	
	assert(*bufferSize == pos);
	return buffer;
}

/*****************************************************************************
 * Buffer save functions
 *****************************************************************************/
// get the grammar tokens/non terminals names, sorted by id
// it also update buffer size
static void getGrammarTokNtNames(Grammar *grammar, std::vector<const char *> & tokenNames,
		std::vector<const char *> & ntNames, unsigned int *bufferSize) {
	
	*bufferSize += sizeof(uint32_t); // tokenCount
	*bufferSize += sizeof(uint32_t); // nonTerminalCount
	
	if (grammar) {
		const Grammar::TokenMap & tokenMap = grammar->getTokenIds();
		const Grammar::NonTerminalMap & nonTerminalMap = grammar->getNonTermonalIds();
		
		tokenNames.resize(tokenMap.size(), NULL);
		ntNames.resize(nonTerminalMap.size(), NULL);
		
		for (Grammar::TokenMap::const_iterator it = tokenMap.begin(); it != tokenMap.end(); ++it) {
			TokenTypeID id = it->second;
			assert(id < tokenNames.size());
			assert(!tokenNames[id]);
			
			tokenNames[id] = it->first.c_str();
			*bufferSize += it->first.size() + 1;
		}
		
		for (Grammar::TokenMap::const_iterator it = nonTerminalMap.begin(); it != nonTerminalMap.end(); ++it) {
			ParsingTree::NonTerminalID id = it->second;
			assert(id < ntNames.size());
			assert(!ntNames[id]);
			
			ntNames[id] = it->first.c_str();
			*bufferSize += it->first.size() + 1;
		}
	}
}

static void writeGrammarTokenNtNames(unsigned char *buffer, unsigned int & pos,
		const std::vector<const char *> & tokenNames, const std::vector<const char *> & ntNames) {
	
	writeToBuffer(buffer, pos, (uint32_t)tokenNames.size());
	for (std::vector<const char *>::const_iterator it = tokenNames.begin(); it != tokenNames.end(); ++it) {
		writeToBuffer(buffer, pos, *it);
	}
	
	writeToBuffer(buffer, pos, (uint32_t)ntNames.size());
	for (std::vector<const char *>::const_iterator it = ntNames.begin(); it != ntNames.end(); ++it) {
		writeToBuffer(buffer, pos, *it);
	}
}

/*****************************************************************************
 * Auxiliar functions
 *****************************************************************************/
// read a null-terminated string
template<>
inline std::string readFromBuffer(const void *buffer, unsigned int & pos) {
	std::string value;
	const unsigned char *buf = (const unsigned char *)buffer;
	
	while (buf[pos]) value.push_back(buf[pos++]);
	pos++; // consume the '\0'
	
	return value;
}

template<class T>
inline static T readFromBuffer(const void *buffer, unsigned int & pos) {
	T value = *(T *)((unsigned char *)buffer + pos);
	pos += sizeof(T);
	return value;
}

// write a null-terminated string
template<>
inline void writeToBuffer(void *buffer, unsigned int & pos, const char *value) {
	unsigned char *buf = (unsigned char *)buffer;
	
	while (*value) buf[pos++] = *(value++);
	buf[pos++] = '\0';
}

template<class T>
inline static void writeToBuffer(void *buffer, unsigned int & pos, T value) {
	*(T *)((unsigned char *)buffer + pos) = value;
	pos += sizeof(T);
}

static void writeToBuffer(void *buffer, unsigned int & pos, const void *bufValue, unsigned int size) {
	if (bufValue) {
		memcpy((unsigned char *)buffer + pos, bufValue, size);
		pos += size;
	}
}
