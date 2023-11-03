#ifndef PARSER_PARSING_TREE_H
#define PARSER_PARSING_TREE_H

#include <cassert>
#include <string>
#include <vector>

#include "parser/InputLocation.h"
#include "parser/TokenType.h"

class ParsingTree {
	public:
		typedef unsigned int NonTerminalID;
		
		enum NodeType {
			NODE_NON_TERMINAL,
			NODE_TOKEN
		};
		
		class Node {
			public:
				Node(NodeType type);
				virtual ~Node();
				
				NodeType getNodeType() const;
				
				// return true if this node know where it is in the input
				// a token always know where it was declared
				// a NonTerminal will know where it was declared if
				// at least one of it's subnodes know where it was declared
				virtual bool hasInputLocation() const = 0;
				
				// return the input position of this node
				virtual InputLocation getInputLocation() const = 0;
				
			private:
				NodeType nodeType;
		};
		typedef std::vector<Node *> NodeList;
		
		class Token;
		
		class NonTerminal : public Node {
			public:
				NonTerminal(NonTerminalID id);
				NonTerminal(NonTerminalID id, unsigned int rule);
				virtual ~NonTerminal();
				
				NonTerminalID getNonTerminalId() const;
				
				const NodeList & getNodeList() const;
				
				void addNode(Node *node);
				
				unsigned int getNonTerminalRule() const;
				void setNonTerminalRule(unsigned int rule);
				
				inline Node *getNodeAt(unsigned int index) const {
					assert(index < nodes.size());
					
					return nodes[index];
				}
				
				inline NonTerminal *getNonTerminalAt(unsigned int index) const {
					assert(index < nodes.size());
					assert(nodes[index]->getNodeType() == NODE_NON_TERMINAL);
					
					return (NonTerminal *)nodes[index];
				}
				
				inline Token *getTokenAt(unsigned int index) const {
					assert(index < nodes.size());
					assert(nodes[index]->getNodeType() == NODE_TOKEN);
					
					return (Token *)nodes[index];
				}
				
				void clearNodesList();
				void deleteSubnodes();
				
				virtual bool hasInputLocation() const;
				
				virtual InputLocation getInputLocation() const;
				
			private:
				NonTerminalID nonTerminalId;
				
				NodeList nodes;
				
				// the rule of this non terminal used.
				// NOTE: the rule is based on the nonterminal, not the rule of the grammar
				unsigned int nonTerminalRule;
		};
		
		class Token : public Node {
			public:
				Token(TokenTypeID id);
				Token(TokenTypeID id, const std::string & tok, const InputLocation & location);
				
				TokenTypeID getTokenTypeId() const;
				
				const std::string & getToken() const;
				void setToken(const std::string & tok);
				
				virtual bool hasInputLocation() const;
				
				virtual InputLocation getInputLocation() const;
				
				Token & operator=(const Token & other);
				
			private:
				TokenTypeID tokenTypeId;
				
				std::string token;
				
				InputLocation inputLocation;
		};
		
		
	private:
		ParsingTree();
};

#endif
