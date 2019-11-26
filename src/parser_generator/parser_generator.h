#pragma once

/* 2008-04-19: Changed ContextFreeLanguage to non-template class.
* ..It does not need to be generic to character set, because it will only
* ..deal with tokens (which are already processed by lexer who took care of
* ..actual characters).
* 2008-04-20: Changed ContextFreeLanguage to class template again. Tokens
* ..are definitely an alphabet, and it has to be defined outside this class
* 2008-04-26: Changed to non-template form again! Integers are enough to denote
* ..each terminals and nonterminals. Same solution as the graph.hh. It semms
* ..that when things are starting to get messy, we must cut some crap.
* 2008-04-26: Done first and follow, and all SLR(1) methods.
* ..Have to finish LALR(1) to do compiler HW2.
* 2008-04-27: Succeeded in SLR(1) parsing!
* 2008-05-04: Succeeded in LR(1)
* 2008-05-05: Succeeded in LALR(1) with both of constructing method
* 2008-05-06: Corrected fast way of LALR(1) construction: 'kernel' and 'kernel
* ..item' are different things! 'kernel' means the set of kenel items in a LR0
* ..or LR1 item set. Lookahead propagating method deals with each
* ..'kernel item', but note that propagation can occur only for adjacent LR0
* ..states. That is, we set propagation only for kernel items in X and Y for
* ..which X goto Y (in LR0 item set). Same items in different LR0 item sets
* ..are different kernel items. And we need to close each kernel (ie., kernel
* ..items in it) to make reduce Action, because there exist non-kernel item
* ..which lead to reduce action: kEpsilonMarker Production. This happends in dangling
* ..else case, where ELSESTMT can be epsilon.
*  Also done precedence and associativity part. But I am finding my system
*  ..getting messier now, because modularity is weak for lexer, parser, and
*  ..main control. Maybe this is an intrinsic property of compiler system.
*/

#include <iostream>
#include <cstdarg> // for variable argument number
#include <vector>
#include <set>
#include <map>

namespace subcc
{
using namespace std;

int const kDollarMarker = 0;   // $: mark the end of input tokens
int const kEpsilonMarker = -1; // empty string
// for variable length fuction arguments (mark end of Production body or input text)
int const kEndMarker = -2;

// Model of a formal language: a concrete class to be instantiated!
// Terminals and non-terminals are all integers; terminal value should be
// ..less the predetermined threshold. Grammar symbols are all non-negative
// -1 is reserved for epsilon, -2 for $ sign.
class ContextFreeLanguage
{
public:
	struct ParseTreeNode
	{
		virtual ~ParseTreeNode() {}
		ParseTreeNode *parent;
		int Production; // each Production is index by vector productions_
	};
	typedef ParseTreeNode ParseTree;
	// The head of first Production is always start symbol
	struct Production
	{
		bool operator==(Production const &rhs) const;
		bool operator<(Production const &rhs) const;
		Production(int key, int inHead) : key_(key), head_(inHead) {}
		Production(Production const &rhs);
		Production(Production &&rhs);
		int key_;
		int head_;
		vector<int> body_;
	};
	/*****************************************************************
		// 0: reserved for $ symbol!!!
		// 1 ~ terminalEnd - 1: terminals
		// terminalEnd : reserved for augmented grammar's start symbol!!!
		// terminalEnd + 1 ~ nonterminalEnd - 1: nonterminals
		// ****************************************************************/
	ContextFreeLanguage(int terminalEnd, int nonTerminalEnd);
	~ContextFreeLanguage()
	{
		//cout << "LANGUAGE DESTRUCTED!!!!\n";
	}
	// Using variable length constructor for specifying Production
	// Returns the key of the Production
	int add(int key, int inHead, int inSymbol, ...);
	// Calculate needed data
	void finalize();
	int getNumProductions() const { return static_cast<int>(productions_.size()); }
	// Read index's Production for client
	Production const &getProduction(int index) const
	{
		return productions_[index];
	}
	Production const &findProduction(int key) const
	{
		return keyToProduction_.find(key)->second;
	}
	// Functions for parser generators
	void getFirst(vector<int> const &symbolstring, int begin,
				  int end, set<int> &) const;
	void getFollow(int nonterminal, set<int> &) const;

	int const terminalEnd_;
	int const nonTerminalEnd_;

private:
	void calculateFirstSets();  // firstSets_
	void calculateFollowSets(); // followSets_

	bool isFinal_; // Once finalized, cannot be modified
	map<int, Production> keyToProduction_;
	// index of each Production is identifier to it.
	vector<Production> productions_;
	// epsilon (empty string) represented by null token.
	map<int, set<int>> firstSets_;
	map<int, set<int>> followSets_;
};

// Types for parsing
enum class Action : int
{ // Action::Error is just empty slot
	Shift = 0,
	Reduce = 1,
	Accept = 2,
	Error = 3
};
enum class Association : int
{ // type of associativity
	Left = 1,
	Right = 2,
	None = 3 // allow no associativity
};

class SlrParserGenerator
{
public:
	typedef ContextFreeLanguage::Production Production;
	typedef int State; // parser is a DFA
	struct LR0Item
	{
		// Default dot position is 0
		LR0Item(Production const &inPro) : production_(inPro), dot_(0) {}
		LR0Item(Production const &inPro, int dot) : production_(inPro), dot_(dot) {}
		LR0Item(LR0Item const &);
		LR0Item(LR0Item &&);
		bool operator<(LR0Item const &rhs) const;
		bool operator==(LR0Item const &rhs) const;
		Production production_;
		// indicates the index of dot in the body of Production
		// 0 means the start of the body, (len of body) means the end
		unsigned dot_;
	};
	SlrParserGenerator(ContextFreeLanguage const &inLang) : language_(inLang), size_(0) {}
	virtual ~SlrParserGenerator() {}
	// Build parser for given formal language
	// if doReduce == false, do not set reduce Action (for lalr)
	virtual void build(bool doReduce = true);
	virtual void checkConflict();
	// Resolving conflicts by precedence (non-negative int)
	// Parser will reduce only when: reduing Production has higer
	// ..precedence, or same precedence and left associativity
	void setPrecedence(int terminal, int precedence)
	{
		precedence_[terminal] = precedence;
	}
	// If associativity is not set, it causes conflict when parsing
	void setAssociation(int precedence, Association inAssoc)
	{
		association_[precedence] = inAssoc;
	}
	// Receive an array of tokens and write out its rightmost derivation
	// The result is in reverse order of derivation (bottom up).
	void parse(int input, ...) const;
	void parse(vector<int> &, vector<int> &result) const;
	// Step by step parsing: take one token and output action
	void initialize(); // start with new input text
	pair<Action, int> parse(int inToken);

protected:
	// Get move of a given item set with a given grammar symbol
	// Move is NOT a closure!
	void getMove(set<LR0Item> const &src, int symbol,
				 set<LR0Item> &target) const;
	void getClosure(set<LR0Item> const &kernels, set<LR0Item> &closure) const;
	State makeNewState();
	// Precedence of a Production is the precedence of its rightmost
	// ..terminal
	int getPrecedence(int terminal) const
	{ // return -1 when not set
		if (precedence_.count(terminal) == 0)
			return -1;
		return precedence_.find(terminal)->second;
	}
	int getPrecedence(Production) const;
	// return non-assoc if not set
	Association getAssoc(int precedence) const
	{
		if (association_.count(precedence) == 0)
			return Association::None;
		return association_.find(precedence)->second;
	};

	ContextFreeLanguage const &language_;
	// Each item set is a state of the parser
	map<set<LR0Item>, State> lR0ItemSetToState_;
	// Each Production's key is its index in its ContextFreeLanguage
	map<Production, int> productionToInt_;
	// Map each head nonterminal to its productions
	map<int, vector<Production>> headToProduction_;
	// Action table: row = state, column = terminal,
	// value = (reduce, Production) or value = (shift, state)
	// Using sets to cover conflicts efficiently.
	vector<vector<set<pair<Action, int>>>> actionTable_;
	// Goto table: row = state,
	// column = nonterminal - CONSTANT, value = state
	vector<vector<int>> gotoTable_;
	// Parsing stack
	vector<int> stack_;
	// Starting state
	State initialState_;
	// Current state of the automata
	State currentState_;
	// Num of total states
	int size_;
	// Map each terminal to its precedence (higher value->high precedence)
	map<int, int> precedence_;
	// Map each precedence level to its associativity
	// Default is Association::Right (shift)
	map<int, Association> association_;
};

// Canonical LR(1) parser: using LR(1) item sets
class Lr1ParserGenerator : public SlrParserGenerator
{
public:
	typedef ContextFreeLanguage::Production Production;
	using SlrParserGenerator::State;
	// LR1's lookahead means that there exists a rightmost derivation with
	// ..its Production which has lookahead token right next to it.
	// Therefore, lookahead token limits the option for reducing with the
	// ..given Production (shift and goto are all the same with SLR)
	struct LR1Item : public SlrParserGenerator::LR0Item
	{
		// Default dot position is 0
		LR1Item(Production const &inPro, int inLookAhead) : LR0Item(inPro), lookahead_(inLookAhead) {}
		LR1Item(LR0Item const &inLR0Item, int inLookAhead) : LR0Item(inLR0Item), lookahead_(inLookAhead) {}
		LR1Item(LR1Item const &);
		LR1Item(LR1Item &&);
		bool operator<(LR1Item const &rhs) const;
		bool operator==(LR1Item const &rhs) const;
		int lookahead_; // one terminal
		static int compareCount_;
	};
	Lr1ParserGenerator(ContextFreeLanguage const &inLang) : SlrParserGenerator(inLang) {}
	virtual ~Lr1ParserGenerator() {}
	virtual void build();

protected:
	void getMove(set<LR1Item> const &src, int symbol,
				 set<LR1Item> &target) const;
	void getClosure(set<LR1Item> const &kernels, set<LR1Item> &closure) const;
	map<set<LR1Item>, State> LR1ItemSetToState_;
};

// Lookahead LR parser: similar to slr except for making use of LR(1) items
class LalrParserGenerator : public Lr1ParserGenerator
{
public:
	typedef ContextFreeLanguage::Production Production;
	LalrParserGenerator(ContextFreeLanguage const &inLang) : Lr1ParserGenerator(inLang) {}
	virtual ~LalrParserGenerator() {}
	virtual void build();
	virtual void buildFast();

protected:
	bool isSameCore(set<LR1Item> const &, set<LR1Item> const &);
};

} // end namespace subcc

