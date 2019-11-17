#ifndef LIB_CALVIN__SUBCC__SUBCC_PARSER_H
#define LIB_CALVIN__SUBCC__SUBCC_PARSER_H

#include "parser_generator.h"
#include "lexer.h"
#include "type.h"
#include "symbol_table.h"
#include "syntax_tree.h"

#include <iostream>
#include <string>

/* 2008-05-21: Have been setting up Type and SymbolTable class for the last 
 * ..few days. Now I start to build abstract syntax tree.
 * One important point I discovered is to convert sequenced structure (like
 * ..in DeclarationList) to tree structure. This can be done easily, and it
 * ..would make overall process much simpler.
 * Some nodes will have certain attribute with them: vector of int for array
 * ..declaration list, type for many other nodes.
 * 2008-05-30: Completed type checking and other basic checks: return type, 
 * ..break statement validity, absence of return statement, undeclared 
 * ..variable, duplicate declaration, etc.
 * 2008-06-09: Revised expression nodes (eliminated selector list and made it
 * ..recursive structure). The grammar specification from the compilers class
 * ..turned out to be a bad one. I referenced the K&R book this time. Working
 * ..on three address code now. 
 * 2008-06-10: Revising to manage constant expression appropriately (making 
 * ..the constant symbols instead of temporary symbols). This will make 
 * ..all constant-value computation happen at compile-time. 
 */

namespace subcc
{

using namespace std;

// Contains the lexer and builds syntax tree and does static checking.
class Parser
{
public:
  Parser(string const &inText); // start parsing
  ~Parser();
  void test();
  // build syntax tree with type checking
  shared_ptr<SyntaxTree const> getSyntaxTree() { return syntaxTree_; }

private:
  void parse();
  // Do appropriate operation with stack and symbol table to complete
  // ..syntax tree
  void shiftWith(int Token);
  void reduceWith(int productionKey);
  // routine for statement node checking (break statement)
  void check();
  void check(StmtNode *node, int breakLabel);

  // Pop children and push parent with making links
  void popAndPush(int childrenNum, NonTerminalNode *parent);
  // Print error with current line number
  void printError(std::string const &errorMessage) const;
  void printError(int lineNum, std::string const &errorMessage) const;
  void printError(IdNode const &identifier, std::string const &) const;
  void printWarning(std::string const &warningMessage) const;

  string getTempName(); // return "t1", "t2", ...

  void makeLanguage(ContextFreeLanguage &);
  void makeParser(LalrParserGenerator &);

  Lexer lexer_;
  ContextFreeLanguage subcc_;
  LalrParserGenerator subccParser_;

  shared_ptr<GlobalSymbolTable> symbolTable_; // takes care of range of scopes itself
  vector<SyntaxTreeNode *> stack_;            // stack for building syntax tree
  SyntaxTreeNode *root_;                      // root of the syntax tree

  // return type of the currently defining function
  shared_ptr<Type const> returnType_;

  int curTemporaryNumber_; // t1, t2, ...(used in temporary for three-adderss)

  // resulted syntax tree by 'parse()' method
  shared_ptr<SyntaxTree const> syntaxTree_;
};

} // namespace subcc

#endif
