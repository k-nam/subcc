#pragma once

#include "lexer_generator.h" // for string
#include "type.h"
#include "symbol_table.h"

#include <iostream>
#include <deque>
#include <memory>

using std::deque;
using std::shared_ptr;
using std::dynamic_pointer_cast;


namespace subcc {

class SyntaxTreeNode;

extern int syntaxTreeNodeCount;

// Intermediate representation: output of parser
class SyntaxTree {
public: 
  SyntaxTree(SyntaxTreeNode const *root, shared_ptr<GlobalSymbolTable const> symtab);
	~SyntaxTree(); 
  SyntaxTreeNode const *getRoot() const { return root_; }
  shared_ptr<GlobalSymbolTable const> getSymbolTable() const { return symbolTable_; }		
private:
	void findAllNodes(std::set<SyntaxTreeNode const*> &resultSet, SyntaxTreeNode const *currentRoot);
	void deleteAllNodes();
	void deleteAllNodes2();
  SyntaxTreeNode const *root_;
  shared_ptr<GlobalSymbolTable const> symbolTable_;
};

class SyntaxTreeNode {
public:
  SyntaxTreeNode();
	virtual ~SyntaxTreeNode();
  void setParent(SyntaxTreeNode const *parent) { parent_ = parent; }
	virtual bool isNonTerminal() const { return false; }
public:
	static void countObjects();
protected:
  SyntaxTreeNode const *parent_;
private:
	static int objectCount_;
};

// Special nodes for storing dimension of an array declaration
class ArrayDeclNode: public SyntaxTreeNode {
public:
  ArrayDeclNode(int dimension): dimension_(dimension) { }
  int getDimension() const { return dimension_; }
protected:
  int dimension_;
};

class ArrayDeclListNode: public SyntaxTreeNode {
public:
  deque<int> const &getDimensions() const { return dimensions_; }
  void pushDimension(int size) { dimensions_.push_back(size); }
protected:
  deque<int> dimensions_; // ex) int [3][4] -> vector [3, 4]
};

// Leaf node; other kinds of tokens are ignored in syntax tree
// Symboltableindex is not correct until the declaration is parsed through
// Stores the line number it appeard for error reporting with this ID
class IdNode: public SyntaxTreeNode {
public:
  IdNode(string const &lexeme, int lineNum): 
    lexeme_(lexeme), lineNum_(lineNum) { }
  string const &getLexeme() const { return lexeme_; }
  void setSymbolTableIndex(int index) { symbolTableIndex_ = index; }
  int getSymbolTableIndex() const { return symbolTableIndex_; }
  int getLineNum() const { return lineNum_; }
protected:
  string lexeme_; 
  int symbolTableIndex_; // pointer to symbol table
  int lineNum_; // line number of source code the ID is in
};

// Leaf node for constant tokens
// Constants are inserted to the symbol table immediately when they are shifted
class ConstantNode: public SyntaxTreeNode {
public:
  ConstantNode(int index): symbolTableIndex_(index) { }
  int getSymbolTableIndex() const { return symbolTableIndex_; }
protected:
  int symbolTableIndex_;
};

// Interior node
class NonTerminalNode: public SyntaxTreeNode {
public:
  NonTerminalNode(int productionKey): productionKey_(productionKey) { }
	virtual ~NonTerminalNode();
  virtual void pushChild(SyntaxTreeNode const *child) 
    { children_.push_back(child); }
  int getNumChildren() const { return static_cast<int>(children_.size()); }
  SyntaxTreeNode *getChild(int index) const 
    { return const_cast<SyntaxTreeNode *> (children_[index]); }
  int getProductionKey() const { return productionKey_; }
  void changeProductionKey(int newKey);
	virtual bool isNonTerminal() const { return true; }
protected:
  int const productionKey_; // production key
  vector<SyntaxTreeNode const *> children_;
};

// Node for function definition: mainly for managing stack size
class FunctionNode: public NonTerminalNode {
public:
  FunctionNode(int productionKey, int maxStackSize): 
    NonTerminalNode(productionKey), maxStackSize_(maxStackSize) { }
  int getMaxStackSize() const { return maxStackSize_; }
protected:
  int const maxStackSize_;
};

// Node for 'Stmt' construct: supports additional parsing info for statements
// Managing 'break' statement is important: we have to know into what Stmt that
// ..break applies to. And we have to detect invalid 'break' statement, too.
// Revised to manage break statement in top-down traversing of syntax tree 
// ..(inherited attribute)
// Checking return action in
class StmtNode: public NonTerminalNode {
public:
  StmtNode(int productionKey, bool isReturning): 
    NonTerminalNode(productionKey), isReturning_(isReturning) { }
  int getBreakLabel() const { return breakLabel_; }
  void setBreakLabel(int breakLabel) { breakLabel_ = breakLabel; }
  bool isReturning() const { return isReturning_; }
  void setReturning(bool value) { isReturning_ = value; } 
protected:
  // The label number to which 'break' will jump; -1 if break is not allowed
  int breakLabel_; // inherited arrtibute
  // True if all possible flow of execution of this statement 'returns' 
  bool isReturning_; // synthesized attribute
};

// Node for formal parameter list: product typed
// Only typed node (param) can be children of this node
class ParamListNode: public NonTerminalNode {
public: 
  ParamListNode(int productionKey): NonTerminalNode(productionKey) { }
  shared_ptr<ProductType const> getType() const 
		{ return shared_ptr<ProductType const>(new ProductType(typeVector_)); }
  void pushParamType(shared_ptr<Type const> paramType);
  // Synonyms
  void pushArgType(shared_ptr<Type const> argType) { pushParamType(argType); }
  // Get the total size of formal parameters
protected:
  vector<shared_ptr<Type const>> typeVector_;
};

// ArgumentListNode is the same as ParamListNode
typedef ParamListNode ArgumentListNode;

// Node with its type info: TypeSpecifier, StructType,
// ..StructTypeDeclaration, StructTypeReference, Param,
// ..Expr, UnaryExpr, PostfixUnaryExpr, BinaryExpr, ...
class TypedNode: public NonTerminalNode {
public:
  TypedNode(int productionKey, shared_ptr<Type const> type): 
    NonTerminalNode(productionKey), type_(type) { }
	//virtual ~TypedNode() { }
  shared_ptr<Type const> getType() const { return type_; }
protected:
  shared_ptr<Type const> type_;
};

// Node for expressions: each expr node has temporary symbol for it
class ExprNode: public TypedNode {
public:
  ExprNode(int productionKey, shared_ptr<Type const> type, bool isConstant);
	//virtual ~ExprNode() { std::cout << "expr deleted\n"; }
  int getRvalueIndex() const { return rvalueIndex_; }
  void setRvalueIndex(int index) { rvalueIndex_ = index; }
  // Has l-value or not
  bool isAssignable() const { return isAssignable_; }
  bool isConstant() const { return isConstant_; }
protected:
  ExprNode(int productionKey, shared_ptr<Type const> type, bool isAssignable, 
      bool isConstant);
  bool isAssignable_; // has l-value or not
  bool isConstant_;
  // index for the r-value symbol of expression
  int rvalueIndex_;
};

// Expression with l-value (subset of primary expression)
class LvalueNode: public ExprNode {
public:
  LvalueNode(int productionKey, shared_ptr<Type const> type);
  int getBaseIndex() const        { return baseIndex_; }
  int getIndexIndex() const       { return indexIndex_; }
  int getTempIndex() const        { return tempIndex_; }
  void setBaseIndex(int index)    { baseIndex_  = index; }
  void setIndexIndex(int index)   { indexIndex_ = index; }
  void setTempIndex(int index)    { tempIndex_  = index; }
protected:
  // L-value: l-value of base + r-value of index (in bytes)
  int baseIndex_; // must be a name
  int indexIndex_; // can be a name, temporary, or constant
  // Temporary storage
  int tempIndex_; // must be a temporary
};


}    // end namespace




