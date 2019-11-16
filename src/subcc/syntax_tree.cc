#include "syntax_tree.h"

namespace subcc
{
//------------------ SyntaxTree methods -------------------

SyntaxTree::SyntaxTree(SyntaxTreeNode const *root, shared_ptr<GlobalSymbolTable const> symtab):
      root_(root), symbolTable_(symtab) { }

SyntaxTree::~SyntaxTree() {
	deleteAllNodes2();
}

void SyntaxTree::findAllNodes(std::set<SyntaxTreeNode const*> &resultSet, 
															SyntaxTreeNode const *currentRoot) {
	resultSet.insert(currentRoot);
	if (currentRoot->isNonTerminal()) {
		int numChildren = dynamic_cast<NonTerminalNode const *>(currentRoot)->getNumChildren();
		for (int i = 0; i < numChildren; i++) {
			findAllNodes(resultSet, dynamic_cast<NonTerminalNode const *>(currentRoot)->getChild(i));
		}
	}
}

void SyntaxTree::deleteAllNodes() {
	delete root_; 
}

void SyntaxTree::deleteAllNodes2() {
	std::set<SyntaxTreeNode const*> result;
	findAllNodes(result, root_);
	//std::cout << "total nodes # was:" << result.size() << "\n";
	for (std::set<SyntaxTreeNode const*>::const_iterator iter = result.begin(); iter != result.end(); ++iter) {
		delete (*iter);
	}
}

/************* SyntaxTreeNode methods *************/

SyntaxTreeNode::SyntaxTreeNode() { 
	objectCount_++;
	//std::cout << "SyntaxTreeNode created: " << objectCount_ << "\n";
}

SyntaxTreeNode::~SyntaxTreeNode() { 
	objectCount_--;
	//std::cout << "SyntaxTreeNode destroyed: " << objectCount_ << "\n"; 
	if (this->isNonTerminal()) {
		//std::cout << "Nonterminal destroyed\n";
	} else {
		//std::cout << "Terminal destroyed\n";
	}
}

int SyntaxTreeNode::objectCount_ = 0;

void SyntaxTreeNode::countObjects() {
	if (objectCount_ != 0) {
		std::cout << "SyntaxTreeNode object memory leak! # was: " << objectCount_ << "\n";
		exit(0);
	} else {
		std::cout << "SyntaxTreeNode object memory OK!\n";
	}
	std::cout << "\n";
}

/************ NonTerminalNode methods *************/

NonTerminalNode::~NonTerminalNode() {
	// Now I use 'find all nodes and delete them all' methods instead of 
	// deleting nodes recursively. 
	// So this kind of special case doesn't occur. 
	/*
	//std::cout << "NonterminalNode deleted key: " << productionKey_ <<
		//	"size: " << children_.size() << "\n";
	int childToDelete = 0;
	// Catch "x += y" type expressions
	if (productionKey_ == 191 && getNumChildren() == 2 &&
				getChild(1)->isNonTerminal() == true) { // "x = NonTerminal" type
		NonTerminalNode const *rhsExprNode = 
			dynamic_cast<NonTerminalNode const *>(getChild(1));
		if (rhsExprNode->getProductionKey() >= 281 &&
				rhsExprNode->getProductionKey() <= 288 && // "x = a binOp b" type
				(getChild(0) == rhsExprNode->getChild(0))) { // "x = x binOp y" type
			// Skip first child in deleting to avoid double delete call
			childToDelete = 1;
			//std::cout << "----avoiding double delete!\n";
		}
	}
	for (; childToDelete <static_cast<int>(children_.size()); childToDelete++) {
		//std::cout << "Nonternalnode deleting children\n";
		delete children_[childToDelete];
	}
	*/
}

void NonTerminalNode::changeProductionKey(int newKey) {
  int &productionKey = const_cast<int &> (productionKey_);
  productionKey = newKey;
}

/*************** FunctionNode methods ****************/


/************** ParamListNode methods ***************/

void ParamListNode::pushParamType(shared_ptr<Type const> paramType) {
  typeVector_.push_back(paramType);
}

/****************** ExprNode methods ***************/

ExprNode::ExprNode(int key, shared_ptr<Type const> type, bool isConstant):
  TypedNode(key, type), isAssignable_(false), isConstant_(isConstant),
  rvalueIndex_(-1) { }

ExprNode::ExprNode(int key, shared_ptr<Type const> type, bool isAssignable, 
    bool isConstant):
  TypedNode(key, type), isAssignable_(isAssignable), 
  isConstant_(isConstant), rvalueIndex_(-1) { }

/****************** LvalueNode methods ***************/

LvalueNode::LvalueNode(int key, shared_ptr<Type const> type):
  ExprNode(key, type, true, false), 
  baseIndex_(-1), indexIndex_(-1), tempIndex_(-1)  { }

} // end namespace subcc
