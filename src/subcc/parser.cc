/***************  SubccParser.cc     ***************/

#include "parser.h"
#include "type.h"
#include "utility.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdlib.h>

namespace subcc
{

using namespace std;

typedef SlrParserGenerator::Production Production;

Parser::Parser(string const &inText) : lexer_(inText),
                                       subcc_(250, 500), subccParser_(subcc_),
                                       symbolTable_(shared_ptr<GlobalSymbolTable>(new GlobalSymbolTable)),
                                       root_(NULL),
                                       curTemporaryNumber_(1)
{

  makeLanguage(subcc_);
  cout << "Language constructing took "
       << " sec\n";
  makeParser(subccParser_);
  cout << "LALR(1) parser generating took "
       << " sec\n";

  parse();
}

Parser::~Parser()
{
  //delete syntaxTree_;
}

// Check input string for syntax
void Parser::test()
{
  pair<Action, int> curPair;
  int curToken = lexer_.getNextToken(); // starting Token
  while (true)
  {
    // Lexical error will not be reported here
    // Lexer will output error message to std_out and continue matching
    curPair = subccParser_.parse(curToken);
    switch (curPair.first)
    {
    case Action::Shift:
      curToken = lexer_.getNextToken();
      break;
    case Action::Reduce:
      cout << "Reduce with Production " << curPair.second << endl;
      break;
    case Action::Error:
      cout << "Symtax error in line " << lexer_.getLineNum() << endl;
      cout << "Token was ";
      cout << lexer_.getLexeme();
      return;
    case Action::Accept:
      cout << "ACCEPTED\n";
      return;
    default:
      cout << "subcc.cc internal error\n";
      return;
    }
  }
}
// Do real parsing: build syntax tree with filling symbol table and checking
// ..types for each node.
// Manipulating constant value is a problem now: ex) char a = 5555; , char a =
// ..'b' ...
void Parser::parse()
{
  // 1. Keeping only necessary nodes on the stack: discard operators and
  // ..keywords
  // 2. Convert recursive structure to a linear list of children
  pair<Action, int> curPair;
  int curToken = lexer_.getNextToken(); // starting Token
  
  bool finished = false;
  while (!finished)
  {
    curPair = subccParser_.parse(curToken);
    //cout << "lexeme: " << lexer_.getLexeme() << "\n";
    switch (curPair.first)
    {
    case Action::Shift:
      shiftWith(curToken);
      curToken = lexer_.getNextToken();
      // Be careful of USERDEFTYPE: we must manually look at the symbol table
      if (curToken == ID)
      {
        //cout << "shifting ID:  ";
        string const &lexeme = lexer_.getLexeme();
        int index = symbolTable_->findIndex(lexeme);
        if (index >= 0)
        {
          //cout << "this is declared ID\n";
          shared_ptr<Symbol const> symbol = symbolTable_->lookUp(index);
          if (symbol->getSymbolKind() == SYMBOL_TYPENAME)
          {
            //cout << "making USERDEFTYPE Token\n";
            curToken = USERDEFTYPE;
          }
        }
      }
      break;
    case Action::Reduce:
      reduceWith(curPair.second); // use separate procedure
      break;
    case Action::Error:
      cout << "Syntax error: in line " << lexer_.getLineNum();
      cout << ", Token was '";
      cout << lexer_.getLexeme();
      cout << "'\n";
      exit(0);
    case Action::Accept:
      //cout << "ACCEPTED by parser\n";
      // stack must be empty now
      if (!stack_.empty())
      {
        cout << "Parser accept error: stack size was: " << stack_.size() << "\n";
        exit(0);
      }
      finished = true;
      break;
    default:
      cout << "subcc.cc internal error\n";
      exit(0);
    }
  }
  // Do additional static check
  check();
  syntaxTree_ = shared_ptr<SyntaxTree const>(new SyntaxTree(root_, symbolTable_));
}

// Put only necessary things onto the parsing stack
void Parser::shiftWith(int curToken)
{
  string const &lexeme = lexer_.getLexeme();
  switch (curToken)
  {        // discards all trivial tokens
  case ID: // set symbolTableIndex regardless of validity
  {
    //cout << "Making IdNode with ";
    //cout << endl;
    IdNode *idNode = new IdNode(lexeme, lexer_.getLineNum());
    stack_.push_back(idNode);
    int index = symbolTable_->findIndex(lexeme); // may be -1, but OK
    idNode->setSymbolTableIndex(index);
    break;
  }
  case USERDEFTYPE: // same as ID, but it should have symtab entry already
  {
    //cout << "Making IdNode(USERDEFTYPE) with ";
    //cout << endl;
    IdNode *typeNameNode = new IdNode(lexeme, lexer_.getLineNum());
    int index = symbolTable_->findIndex(lexeme);
    if (index < 0)
    {
      cout << "shiftWith USERDEFTYPE internal error\n";
      exit(0);
    }
    typeNameNode->setSymbolTableIndex(index);
    stack_.push_back(typeNameNode);
    break;
  }
  // All constants should be immediately inserted into symbol table
  case CHAR_LITERAL:
  {
    //cout << "Making Alphabet constant Node with ";
    //cout << endl;
    // We need a function to get numeric value from a char constant
    // ..like 'a', '\n'...
    char charValue = charOf(lexeme);
    //cout << "\tcharValue is " << charValue << endl;
    int symTabIndex = symbolTable_->insert(shared_ptr<Symbol>(
        new Constant(charValue)));
    stack_.push_back(new ConstantNode(symTabIndex));
    break;
  }
  case DECIMAL_LITERAL:
  {
    int intValue = intOfDec(lexeme);
    //cout << "\tintValue is " << intValue << endl;
    int symTabIndex = symbolTable_->insert(shared_ptr<Symbol>(
        new Constant(intValue)));
    stack_.push_back(new ConstantNode(symTabIndex));
    break;
  }
  case HEXADECIMAL_LITERAL:
  {
    //cout << "Making int constant Node with ";
    //cout << endl;
    int intValue = intOfHex(lexeme); // atoi function
    //cout << "\tintValue is " << intValue << endl;
    int symTabIndex = symbolTable_->insert(shared_ptr<Symbol>(
        new Constant(intValue)));
    stack_.push_back(new ConstantNode(symTabIndex));
    break;
  }
  case STRING_LITERAL:
  {
    //cout << "Making string constant Node with ";
    //cout << endl;
    // We need a function to get numeric value from a char constant
    // ..like 'a', '\n'...
    int symTabIndex = symbolTable_->insert(shared_ptr<Symbol>(
        new Constant(lexeme)));
    stack_.push_back(new ConstantNode(symTabIndex));
    break;
  }
  }
}

// Main parsing procedure
void Parser::reduceWith(int productionKey)
{
  //cout << "reducing with " << productionKey << " stack size is " <<
  //stack_.size() << endl;
  switch (productionKey)
  {
  case 1: // Program -> DeclList
  {
    if (stack_.size() != 1)
      cout << "Parser::parse() internal error in case 1\n";
    root_ = stack_.back();
    stack_.pop_back();
    break;
  }
  case 10: // DeclarationList -> empty : using key 11
  {
    NonTerminalNode *decListNode = new NonTerminalNode(11);
    stack_.push_back(decListNode);
    break;
  }
  case 11: // DeclarationList -> DeclarationList Declaration
  {
    NonTerminalNode *declListNode =
        dynamic_cast<NonTerminalNode *>(stack_[stack_.size() - 2]);
    NonTerminalNode *declNode =
        dynamic_cast<NonTerminalNode *>(stack_[stack_.size() - 1]);
    declListNode->pushChild(declNode);
    declNode->setParent(declListNode);
    stack_.pop_back();
    break;
  }
  case 21: // TypedefDecl -> Typespec, ID
  {
    NonTerminalNode *typeDefNode = new NonTerminalNode(21);
    TypedNode *typeSpecNode =
        dynamic_cast<TypedNode *>(stack_[stack_.size() - 2]);
    IdNode *idNode = dynamic_cast<IdNode *>(stack_.back());
    string const &lexeme = idNode->getLexeme();
    shared_ptr<Type const> type = typeSpecNode->getType();
    shared_ptr<Symbol> newSymbol(new Symbol(SYMBOL_TYPENAME, lexeme, type));
    int index = symbolTable_->insert(newSymbol);
    if (index < 0)
    { // wrong declaration
      printError(*idNode, "the identifier has been declared already");
    }
    idNode->setSymbolTableIndex(index);
    popAndPush(2, typeDefNode);
    break;
  }
  case 31: // VarDecl -> Typespec, ID
  {
    NonTerminalNode *varDeclNode = new NonTerminalNode(31);
    TypedNode *typeSpecNode =
        dynamic_cast<TypedNode *>(stack_[stack_.size() - 2]);
    IdNode *idNode = dynamic_cast<IdNode *>(stack_.back());
    string const &lexeme = idNode->getLexeme();
    shared_ptr<Type const> type = typeSpecNode->getType();
    if (!type->isVarDeclarable())
    { // type error
      printError(*idNode, "can not declare a variable with the type\n");
    }
    shared_ptr<Symbol> newSymbol;
    if (symbolTable_->isInGlobal())
      newSymbol = shared_ptr<Symbol>(new Symbol(SYMBOL_VAR_G, lexeme, type));
    else
    {
      if (symbolTable_->isInEnvironment())
        newSymbol = shared_ptr<Symbol>(new Symbol(SYMBOL_ELEM, lexeme, type));
      else
        newSymbol = shared_ptr<Symbol>(new Symbol(SYMBOL_VAR_L, lexeme, type));
    }
    int index = symbolTable_->insert(newSymbol);
    if (index < 0)
    { // wrong declaration
      printError(*idNode, "the identifier has been declared already");
    }
    idNode->setSymbolTableIndex(index);
    popAndPush(2, varDeclNode);
    break;
  }
  case 32: // VarDecl -> Typespec, ID, ArrayDecl, 'string constant'
  {
    // Actually this is a declaration + assignment, and we do not implement
    // ..assignment yet (will be done in code generation)
    NonTerminalNode *varDeclNode = new NonTerminalNode(32);
    TypedNode *typeSpecNode =
        dynamic_cast<TypedNode *>(stack_[stack_.size() - 4]);
    IdNode *idNode = dynamic_cast<IdNode *>(stack_[stack_.size() - 3]);
    ArrayDeclNode *arrayNode =
        dynamic_cast<ArrayDeclNode *>(stack_[stack_.size() - 2]);
    string const &lexeme = idNode->getLexeme();
    shared_ptr<Type const> type = typeSpecNode->getType();
    int dimension = arrayNode->getDimension();
    // Check invalid assignment
    if (!isCharType(type) || dimension != 80)
    {
      printError(*idNode, "string constant should be assigned to char[80]");
    }
    shared_ptr<Type const> newType(new ArrayType(dimension, type));
    shared_ptr<Symbol> newSymbol;
    if (symbolTable_->isInGlobal())
      newSymbol = shared_ptr<Symbol>(new Symbol(SYMBOL_VAR_G, lexeme, newType));
    else
    {
      if (symbolTable_->isInEnvironment())
        newSymbol = shared_ptr<Symbol>(new Symbol(SYMBOL_ELEM, lexeme, newType));
      else
        newSymbol = shared_ptr<Symbol>(new Symbol(SYMBOL_VAR_L, lexeme, newType));
    }
    int index = symbolTable_->insert(newSymbol);
    idNode->setSymbolTableIndex(index);
    popAndPush(4, varDeclNode);
    break;
  }
  case 33: // VarDecl -> Typespec, ID, ArrayDeclList
  {
    NonTerminalNode *varDeclNode = new NonTerminalNode(32);
    TypedNode *typeSpecNode =
        dynamic_cast<TypedNode *>(stack_[stack_.size() - 3]);
    IdNode *idNode = dynamic_cast<IdNode *>(stack_[stack_.size() - 2]);
    ArrayDeclListNode *arrayNode =
        dynamic_cast<ArrayDeclListNode *>(stack_[stack_.size() - 1]);
    string const &lexeme = idNode->getLexeme();
    shared_ptr<Type const> type = typeSpecNode->getType();
    if (!type->isVarDeclarable())
    { // type error
      printError(*idNode, "can not declare a variable with the type\n");
    }
    deque<int> dimensions = arrayNode->getDimensions();
    shared_ptr<Type const> newType(new ArrayType(dimensions, type));
    shared_ptr<Symbol> newSymbol;
    if (symbolTable_->isInGlobal())
      newSymbol = shared_ptr<Symbol>(new Symbol(SYMBOL_VAR_G, lexeme, newType));
    else
    {
      if (symbolTable_->isInEnvironment())
        newSymbol = shared_ptr<Symbol>(new Symbol(SYMBOL_ELEM, lexeme, newType));
      else
        newSymbol = shared_ptr<Symbol>(new Symbol(SYMBOL_VAR_L, lexeme, newType));
    }
    int index = symbolTable_->insert(newSymbol);
    if (index < 0)
    { // wrong declaration
      printError(*idNode, "the identifier has been declared already");
    }
    idNode->setSymbolTableIndex(index);
    popAndPush(3, varDeclNode);
    break;
  }
  case 41: // ArrayDeclList -> ArrayDeclList, ArrayDecl
  {
    ArrayDeclListNode *arrayDeclListNode =
        dynamic_cast<ArrayDeclListNode *>(stack_[stack_.size() - 2]);
    ArrayDeclNode *arrayDeclNode =
        dynamic_cast<ArrayDeclNode *>(stack_.back());
    arrayDeclListNode->pushDimension(arrayDeclNode->getDimension());
    stack_.pop_back();
    // To prevent memory leak
    delete arrayDeclNode;
    break;
  }
  case 42: // ArrayDeclList -> ArrayDecl
  {
    ArrayDeclListNode *arrayDeclListNode = new ArrayDeclListNode();
    ArrayDeclNode *arrayDeclNode =
        dynamic_cast<ArrayDeclNode *>(stack_.back());
    arrayDeclListNode->pushDimension(arrayDeclNode->getDimension());
    stack_.pop_back();
    // To prevent memory leak
    delete arrayDeclNode;
    stack_.push_back(arrayDeclListNode);
    break;
  }
  case 43: // ArrayDecl -> [ , IntegerLiteral, ]
  {
    ConstantNode *constantNode =
        dynamic_cast<ConstantNode *>(stack_.back());
    int index = constantNode->getSymbolTableIndex();
    shared_ptr<Constant const> constantSymbol =
        dynamic_pointer_cast<Constant const>(symbolTable_->lookUp(index));
    int dimension;
    switch (constantSymbol->getSymbolKind())
    {
    case SYMBOL_CONSTANT_CHAR:
      dimension = constantSymbol->getCharValue();
      break;
    case SYMBOL_CONSTANT_SHORT:
      dimension = constantSymbol->getShortValue();
      break;
    case SYMBOL_CONSTANT_INT:
      dimension = constantSymbol->getIntValue();
      break;
    }
    ArrayDeclNode *arrayDeclNode = new ArrayDeclNode(dimension);
    stack_.pop_back();
    // To prevent memory leak
    //std::cout << "deleting arrayDecl node\n";
    delete constantNode;
    stack_.push_back(arrayDeclNode);
    break;
  }
  case 51: // StructDecl -> StructTypeDecl, ';'
  {        // using structTypeDecl node itself, changing only the Production key
    NonTerminalNode *structDeclNode =
        dynamic_cast<NonTerminalNode *>(stack_.back());
    structDeclNode->changeProductionKey(51);
    break;
  }
  case 52: // StructTypeDecl -> ID { LocalDeclList }
  {        // Using the symbol table itself as record type
    // All struct declaration
    IdNode *idNode = dynamic_cast<IdNode *>(stack_[stack_.size() - 2]);
    string const &lexeme = idNode->getLexeme();
    SymbolTable const &environment = symbolTable_->getEnvironment();
    shared_ptr<RecordType> recordType(new RecordType(lexeme, environment));
    TypedNode *structTypeNode = new TypedNode(52, recordType);
    // Manage symbol table
    shared_ptr<Symbol> symbol(new Symbol(SYMBOL_STRUCT, lexeme, recordType));
    int index = symbolTable_->insert(symbol);
    if (index < 0)
    { // wrong declaration
      printError(*idNode, "the identifier has been declared already");
    }
    idNode->setSymbolTableIndex(index);
    popAndPush(2, structTypeNode);
    break;
  }
  case 61: // Typespec -> VOID_
  {
    TypedNode *typeSpecNode =
        new TypedNode(61, shared_ptr<Type>(new BaseType(BASETYPE_VOID)));
    stack_.push_back(typeSpecNode);
    break;
  }
  case 62: // Typespec -> CHAR
  {
    TypedNode *typeSpecNode =
        new TypedNode(61, shared_ptr<Type>(new BaseType(BASETYPE_CHAR)));
    stack_.push_back(typeSpecNode);
    break;
  }
  case 63: // Typespec -> SHORT
  {
    TypedNode *typeSpecNode =
        new TypedNode(61, shared_ptr<Type>(new BaseType(BASETYPE_SHORT)));
    stack_.push_back(typeSpecNode);
    break;
  }
  case 64: // Typespec -> INT
  {
    TypedNode *typeSpecNode =
        new TypedNode(61, shared_ptr<Type>(new BaseType(BASETYPE_INT)));
    stack_.push_back(typeSpecNode);
    break;
  }
  case 65: // Typespec -> structType
  {
    TypedNode *structTypeNode = dynamic_cast<TypedNode *>(stack_.back());
    shared_ptr<Type const> structType = structTypeNode->getType();
    TypedNode *typeSpecNode = new TypedNode(65, structType);
    popAndPush(1, typeSpecNode);
    break;
  }
  case 66: // Typespec -> USERDEFTYPE
  {
    IdNode const *typeNameNode =
        dynamic_cast<IdNode const *>(stack_.back());
    int index = typeNameNode->getSymbolTableIndex();
    shared_ptr<Symbol const> typeNameSymbol = symbolTable_->lookUp(index);
    if (typeNameSymbol->getSymbolKind() != SYMBOL_TYPENAME)
    {
      cout << "Parser::reduceWith 66 error\n";
      exit(0);
    }
    shared_ptr<Type const> userDefType = typeNameSymbol->getType();
    TypedNode *typeSpecNode = new TypedNode(66, userDefType);
    popAndPush(1, typeSpecNode);
    break;
  }
  case 71: // StructType -> StructTypeDecl
  {        // do nothing
    break;
  }
  case 72: // StructType -> StructTypeReference
  {        // do nothing
    break;
  }
  case 73: // StructTypeReference -> ID
  {
    IdNode const *structNameNode =
        dynamic_cast<IdNode const *>(stack_.back());
    int index = structNameNode->getSymbolTableIndex();
    if (index < 0)
    { // type error: ID does not name a struct!
      printError(*structNameNode, "the identifier is not declared");
      break;
    }
    shared_ptr<Symbol const> structNameSymbol = symbolTable_->lookUp(index);
    if (structNameSymbol->getSymbolKind() != SYMBOL_STRUCT)
    {
      printError(*structNameNode, "the identifier is not of struct type");
      break;
    }
    shared_ptr<Type const> structType = structNameSymbol->getType();
    if (structType->getType() != TYPE_RECORD)
    {
      cout << "Parser internal error: reduceWith 73\n";
      exit(0);
    }
    TypedNode *structTypeNode = new TypedNode(73, structType);
    popAndPush(1, structTypeNode);
    break;
  }
  case 81: // FuncDecl -> TypeSpec, ID, Params (declaration only)
  {
    NonTerminalNode *funcDeclNode = new NonTerminalNode(81);
    TypedNode *typeSpecNode =
        dynamic_cast<TypedNode *>(stack_[stack_.size() - 3]);
    IdNode *idNode = dynamic_cast<IdNode *>(stack_[stack_.size() - 2]);
    string const &lexeme = idNode->getLexeme();
    ParamListNode *paramListNode =
        dynamic_cast<ParamListNode *>(stack_.back());
    shared_ptr<Type const> returnType = typeSpecNode->getType();
    shared_ptr<ProductType const> argumentType = paramListNode->getType();
    shared_ptr<FuncType> funcType(new FuncType(argumentType, returnType));
    shared_ptr<Symbol> funcSymbol(new Symbol(SYMBOL_FUNC, lexeme, funcType));
    int index = symbolTable_->insert(funcSymbol);
    if (index < 0)
    { // wrong declaration
      printError(*idNode,
                 "the identifier for the function has been declared already");
    }
    idNode->setSymbolTableIndex(index);
    popAndPush(3, funcDeclNode);
    break;
  }
  case 82: // FuncDecl -> FuncDefHead, CompoundStatement (definition)
  {
    StmtNode *compoundStmtNode =
        dynamic_cast<StmtNode *>(stack_.back());
    if (!isVoidType(returnType_) && !compoundStmtNode->isReturning())
    {
      printWarning("not every control of the function defined above \
											 returns properly");
    }
    FunctionNode *funcNode =
        new FunctionNode(82, symbolTable_->getMaxStackSize());
    popAndPush(2, funcNode);
    break;
  }
  case 83: // FuncDefHead -> TypeSpec, ID, Params ( = paramListNode)
  {        // act also as a declaration if not declared already!
    NonTerminalNode *funcDefHeadNode = new NonTerminalNode(83);
    TypedNode *typeSpecNode =
        dynamic_cast<TypedNode *>(stack_[stack_.size() - 3]);
    IdNode *idNode = dynamic_cast<IdNode *>(stack_[stack_.size() - 2]);
    string const &lexeme = idNode->getLexeme();
    ParamListNode *paramListNode =
        dynamic_cast<ParamListNode *>(stack_.back());
    shared_ptr<Type const> returnType = typeSpecNode->getType();
    shared_ptr<ProductType const> argumentType = paramListNode->getType();
    shared_ptr<FuncType> funcType(new FuncType(argumentType, returnType));
    shared_ptr<Symbol> funcSymbol(new Symbol(SYMBOL_FUNC, lexeme, funcType));
    int index = symbolTable_->findIndex(lexeme);
    if (index >= 0)
    { // already declread
      shared_ptr<Type const> declaredType = symbolTable_->lookUp(index)->getType();
      if (*declaredType != *funcType)
        printError(*idNode,
                   "the identifier for the function has been declared with different type");
    }
    else
    { // definition and also declaration
      index = symbolTable_->insert(funcSymbol);
    }
    idNode->setSymbolTableIndex(index);
    popAndPush(3, funcDefHeadNode);
    // Sets member field for return type. This applies to all stmt in
    // function definition that follows
    returnType_ = returnType;

    // Reset the offset of this symbol table: parameters are above the
    // ..frame, and function body starts from the beginning of the frame
    symbolTable_->setOffsetToZero();
    break;
  }
  case 91: // Params -> empty
  case 92: // Params -> VOID_
  {        // type is VOID_ (no arguments), but argument type should always be
    //..product type. Empty product denotes void argument type
    ParamListNode *paramsNode = new ParamListNode(94);
    stack_.push_back(paramsNode);
    break;
  }
  case 93: // Params -> ParamList
  {        // does nothing
    break;
  }
  case 94: // ParamList -> ParamList, Param
  {
    ParamListNode *paramListNode =
        dynamic_cast<ParamListNode *>(stack_[stack_.size() - 2]);
    TypedNode *paramNode = dynamic_cast<TypedNode *>(stack_.back());
    paramListNode->pushChild(paramNode);                // link
    paramListNode->pushParamType(paramNode->getType()); // type management
    paramNode->setParent(paramListNode);
    stack_.pop_back();
    break;
  }

  case 95: // ParamList -> Param : using key 94
  {
    TypedNode *paramNode =
        dynamic_cast<TypedNode *>(stack_.back());
    ParamListNode *paramListNode = new ParamListNode(94);
    paramListNode->pushParamType(paramNode->getType());
    popAndPush(1, paramListNode);
    break;
  }
  case 96: // Param -> TypeSpec, ID
  {
    TypedNode *typeSpecNode =
        dynamic_cast<TypedNode *>(stack_[stack_.size() - 2]);
    IdNode *idNode = dynamic_cast<IdNode *>(stack_.back());
    string const &lexeme = idNode->getLexeme();
    shared_ptr<Type const> type = typeSpecNode->getType();
    TypedNode *paramNode = new TypedNode(96, type);
    shared_ptr<Symbol> newSymbol(new Symbol(SYMBOL_PARAMETER, lexeme, type));
    int index = symbolTable_->insert(newSymbol);
    idNode->setSymbolTableIndex(index);
    popAndPush(2, paramNode);
    break;
  }
  case 97: // Param -> TypeSpec, ID, ArrayDeclList
  {
    TypedNode *typeSpecNode =
        dynamic_cast<TypedNode *>(stack_[stack_.size() - 3]);
    IdNode *idNode = dynamic_cast<IdNode *>(stack_[stack_.size() - 2]);
    ArrayDeclListNode *arrayNode =
        dynamic_cast<ArrayDeclListNode *>(stack_[stack_.size() - 1]);
    string const &lexeme = idNode->getLexeme();
    shared_ptr<Type const> type = typeSpecNode->getType();
    deque<int> dimensions = arrayNode->getDimensions();
    shared_ptr<Type const> newType(new ArrayType(dimensions, type));
    shared_ptr<Symbol> newSymbol(new Symbol(SYMBOL_PARAMETER, lexeme, newType));
    int index = symbolTable_->insert(newSymbol);
    idNode->setSymbolTableIndex(index);
    TypedNode *paramNode = new TypedNode(97, newType);
    popAndPush(3, paramNode);
    break;
  }
  case 101: // CompoundStmt -> LocalDeclList, StmtList
  {         // compoundStmt is a stmt!
    StmtNode *stmtListNode =
        dynamic_cast<StmtNode *>(stack_.back());
    StmtNode *compoundStmtNode =
        new StmtNode(101, stmtListNode->isReturning());
    popAndPush(2, compoundStmtNode);
    break;
  }
  case 111: // LocalDeclList -> empty (using key 112)
  {
    NonTerminalNode *localDeclListNode = new NonTerminalNode(112);
    stack_.push_back(localDeclListNode);
    break;
  }
  case 112: // LocalDeclList -> LocalDeclList , NormalDecl
  {
    NonTerminalNode *localDeclListNode =
        dynamic_cast<NonTerminalNode *>(stack_[stack_.size() - 2]);
    NonTerminalNode *normalDeclNode =
        dynamic_cast<NonTerminalNode *>(stack_.back());
    normalDeclNode->setParent(localDeclListNode);
    localDeclListNode->pushChild(normalDeclNode);
    stack_.pop_back();
    break;
  }
  case 121: // StmtList -> empty (using key 122)
  {         // stmtList is a stmt (to store break info)
    StmtNode *stmtListNode = new StmtNode(122, false);
    stack_.push_back(stmtListNode);
    break;
  }
  case 122: // StmtList -> StmtList , Stmt
  {
    StmtNode *stmtListNode =
        dynamic_cast<StmtNode *>(stack_[stack_.size() - 2]);
    StmtNode *stmtNode =
        dynamic_cast<StmtNode *>(stack_.back());
    if (stmtNode->isReturning())
      stmtListNode->setReturning(true);
    stmtNode->setParent(stmtListNode);
    stmtListNode->pushChild(stmtNode);
    stack_.pop_back();
    break;
  }
  case 123: // Stmt -> Expr
  {
    StmtNode *exprStmtNode = new StmtNode(123, false);
    popAndPush(1, exprStmtNode);
    break;
  }
  case 124:
  case 125:
  case 126:
  case 127: // Stmt -> BreakStmt
  case 130: // Stmt -> ContinueStmt
  case 128:
  case 129:
  { // does nothing
    break;
  }
  case 141: // IfStmt -> Expr , Stmt , ElseStmt
  {
    ExprNode *exprNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 3]);
    shared_ptr<Type const> exprType = exprNode->getType();
    if (!exprType->isNumeric())
    {
      printError("condition expression can not be evaluated \
										 to boolean value in IF statement");
    }
    StmtNode *stmtNode =
        dynamic_cast<StmtNode *>(stack_[stack_.size() - 2]);
    StmtNode *elseStmtNode = dynamic_cast<StmtNode *>(stack_.back());
    StmtNode *ifStmtNode = new StmtNode(
        141, stmtNode->isReturning() && elseStmtNode->isReturning());
    popAndPush(3, ifStmtNode);
    break;
  }
  case 142: // ElseStmt -> empty
  {
    StmtNode *elseStmtNode = new StmtNode(142, false);
    stack_.push_back(elseStmtNode);
    break;
  }
  case 143: // ElseStmt -> Stmt
  {         // does nothing
    break;
  }
  case 151: // WhileStmt -> Expr , Stmt
  {
    ExprNode *exprNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 2]);
    shared_ptr<Type const> exprType = exprNode->getType();
    if (!exprType->isNumeric())
    {
      printError("condition expression can not be evaluated to boolean value in WHILE statement");
    }
    // while statement can always jump to next statement
    StmtNode *whileStmtNode = new StmtNode(151, false);
    popAndPush(2, whileStmtNode);
    break;
  }
  case 161: // BreakStmt -> empty
  {
    StmtNode *breakStmt = new StmtNode(161, false);
    stack_.push_back(breakStmt);
    break;
  }
  case 162: // ContinueStmt -> empty
  {
    StmtNode *continueStmt = new StmtNode(162, false);
    stack_.push_back(continueStmt);
    break;
  }
  case 171: // SwitchStmt -> Expr , SwitchCaseList, SwitchDefaultCase
  {
    StmtNode *switchCaseListNode =
        dynamic_cast<StmtNode *>(stack_[stack_.size() - 2]);
    StmtNode *switchDefaultCaseNode =
        dynamic_cast<StmtNode *>(stack_.back());
    StmtNode *switchStmt =
        new StmtNode(171, switchCaseListNode->isReturning() &&
                              switchDefaultCaseNode->isReturning());
    popAndPush(3, switchStmt);
    break;
  }
  case 172: // SwitchCaseList -> SwitchCaseList, SwitchCase
  {
    StmtNode *switchCaseListNode =
        dynamic_cast<StmtNode *>(stack_[stack_.size() - 2]);
    StmtNode *switchCaseNode =
        dynamic_cast<StmtNode *>(stack_.back());
    if (!switchCaseNode->isReturning())
      switchCaseListNode->setReturning(false);
    switchCaseListNode->pushChild(switchCaseNode);
    switchCaseNode->setParent(switchCaseListNode);
    stack_.pop_back();
    break;
  }
  case 173: // SwitchCaseList -> SwitchCase
  {         // using key 172 for list node
    StmtNode *switchCaseNode = dynamic_cast<StmtNode *>(stack_.back());
    StmtNode *switchCaseListNode = new StmtNode(172, switchCaseNode->isReturning());
    popAndPush(1, switchCaseListNode);
    break;
  }
  case 174: // SwitchCase -> Expr , Stmt
  {
    StmtNode *stmtNode = dynamic_cast<StmtNode *>(stack_.back());
    ExprNode *exprNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 2]);
    if (!exprNode->isConstant())
    {
      printError("only constant expression can be used with 'case'");
    }
    StmtNode *switchCaseNode = new StmtNode(174, stmtNode->isReturning());
    popAndPush(2, switchCaseNode);
    break;
  }
  case 175: // SwitchDefaultCase -> empty
  {
    StmtNode *switchDefaultCaseNode = new StmtNode(175, false);
    stack_.push_back(switchDefaultCaseNode);
    break;
  }
  case 176: // SwitchDefaultCase -> Stmt
  {         // does nothing
    break;
  }
  case 181: // ReturnStmt -> empty
  {
    if (!isVoidType(returnType_))
    {
      printError("return type does not match function declaration");
    }
    StmtNode *returnStmt = new StmtNode(181, true);
    stack_.push_back(returnStmt);
    break;
  }
  case 182: // ReturnStmt -> Expr
  {
    ExprNode *exprNode = dynamic_cast<ExprNode *>(stack_.back());
    if (*returnType_ != *(exprNode->getType()))
    {
      printError("return type does not match function declaration");
    }
    StmtNode *returnStmt = new StmtNode(182, true);
    popAndPush(1, returnStmt);
    break;
  }
  case 191: // Expr -> UnaryExpr , '=' , Expr
  {
    ExprNode *unaryExprNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 2]);
    ExprNode *exprNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 1]);
    // Type checking for lhs: only basetype or struct type can appear in
    // ..assignment. And lhs should have l-value
    if (!unaryExprNode->isAssignable())
    {
      printError("left-hand side of assignment does not have l-value");
    }
    shared_ptr<Type const> lhsType = unaryExprNode->getType();
    shared_ptr<Type const> rhsType = exprNode->getType();
    bool isValidAssignment = false;
    if (*lhsType == *rhsType && lhsType->getWidth() == rhsType->getWidth())
    {
      // Normal case
      isValidAssignment = true;
    }
    else if (lhsType->isArrayType() && rhsType->isArrayType())
    {
      auto lhsArray = dynamic_pointer_cast<ArrayType const>(lhsType);
      auto rhsArray = dynamic_pointer_cast<ArrayType const>(rhsType);
      if (*lhsArray->getElementType() == *rhsArray->getElementType())
      {
        if (lhsArray->getArraySize() >= rhsArray->getArraySize())
        {
          isValidAssignment = true;
        }
      }
    }
    else if (lhsType->isNumeric() && rhsType->isNumeric())
    {
      isValidAssignment = true;
    }

    if (!isValidAssignment)
    {
      printError("assign statement type error");
    }
    // In C, assign expression doesn't have l-value (different from C++)
    // The value of the entire expression is just the value of rhs
    ExprNode *assignExpr = new ExprNode(191, lhsType, exprNode->isConstant());
    // Rvalue is just the same as that of rhs
    assignExpr->setRvalueIndex(exprNode->getRvalueIndex());
    popAndPush(2, assignExpr);
    break;
  }
  case 192: // '+='  281
  case 193: // '-='
  case 194: // '*='
  case 195: // '/='
  case 196: // '%='
  case 197: // '&='
  case 198: // '|='
  case 199: // '^='  288
  {         // Different operators are represented by different keys!
    // This is to avoid making redundant AST nodes
    // Simply reduce to assignment and binary operation
    ExprNode *unaryExprNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 2]);
    //ExprNode *newNode = new ExprNode(*unaryExprNode);
    ExprNode *exprNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 1]);
    // x += y  ---> x = x + y
    stack_.pop_back();               // x +=
    stack_.pop_back();               //
    stack_.push_back(unaryExprNode); // x =
    stack_.push_back(unaryExprNode); // x = x
    stack_.push_back(exprNode);      // x = x + y
    reduceWith(productionKey + 89);  // reduce binary operator
    reduceWith(191);                 // reduce assignment
    break;
  }
  case 211: // expr -> unaryExpr
  case 212: // expr -> binaryExpr
  case 221: // unaryExpr -> postfixExpr
  {         // does nothing
    break;
  }
  case 222: // unaryExpr -> '+' , unaryExpr
  case 223: // unaryExpr -> '-' , unaryExpr
  case 227: // unaryExpr -> '~' , unaryExpr
  {
    ExprNode *unaryExprNode =
        dynamic_cast<ExprNode *>(stack_.back());
    shared_ptr<Type const> type = unaryExprNode->getType();
    if (!type->isNumeric())
    {
      printError("unary operator can only be used with numeric type");
    }
    ExprNode *exprNode =
        new ExprNode(productionKey, type, unaryExprNode->isConstant());
    // The expression loses l-value after applying arithmetic operation
    shared_ptr<Symbol> rvalue;
    if (unaryExprNode->isConstant())
    {
      shared_ptr<Constant const> constant =
          dynamic_pointer_cast<Constant const>(
              symbolTable_->lookUp(unaryExprNode->getRvalueIndex()));
      int beforeValue = constant->getIntValue();
      switch (productionKey)
      {
      case 222:
        rvalue = shared_ptr<Symbol>(new Constant(beforeValue));
        break;
      case 223:
        rvalue = shared_ptr<Symbol>(new Constant(-beforeValue));
        break;
      case 227:
        rvalue = shared_ptr<Symbol>(new Constant(~beforeValue));
        break;
      }
    }
    else
    {
      rvalue = shared_ptr<Symbol>(new Temporary(getTempName(), type));
    }
    int index = symbolTable_->insert(rvalue);
    exprNode->setRvalueIndex(index);

    popAndPush(1, exprNode);
    break;
  }
  case 224: // unaryExpr -> '++' , unaryExpr
  case 225: // unaryExpr -> '--' , unaryExpr
  {
    ExprNode *unaryExprNode =
        dynamic_cast<ExprNode *>(stack_.back());
    shared_ptr<Type const> type = unaryExprNode->getType();
    if (!type->isNumeric())
    {
      printError("unary operator can only be used with numeric type");
    }
    if (!unaryExprNode->isAssignable())
    { // no l-value
      printError("inc/dec operator can only be used with l-valued expressions");
    }
    // In C, prefix increment and decrement operator returns only value,
    // ..so this expression does not have l-value (different from C++)
    ExprNode *exprNode = new ExprNode(productionKey, type, false);
    // Setting memory for temporary
    shared_ptr<Symbol> rvalue(new Temporary(getTempName(), type));
    int index = symbolTable_->insert(rvalue);
    exprNode->setRvalueIndex(index);
    popAndPush(1, exprNode);
    break;
  }
  case 226: // unaryExpr -> '!' , unaryExpr
  {
    ExprNode *unaryExprNode =
        dynamic_cast<ExprNode *>(stack_.back());
    shared_ptr<Type const> type = unaryExprNode->getType();
    if (!type->isNumeric())
    {
      printError("unary operator can only be used with numeric type");
    }
    // The expression loses l-value after applying arithmetic operation
    // The result of logical operator is always bool type
    ExprNode *exprNode = new ExprNode(productionKey,
                                      shared_ptr<Type const>(new BaseType(BASETYPE_BOOL)),
                                      unaryExprNode->isConstant());
    // Setting memory for temporary
    shared_ptr<Symbol> rvalue;
    if (unaryExprNode->isConstant())
    {
      shared_ptr<Constant const> constant =
          dynamic_pointer_cast<Constant const>(
              symbolTable_->lookUp(unaryExprNode->getRvalueIndex()));
      char beforeValue = constant->getCharValue();
      rvalue = shared_ptr<Symbol>(new Constant(!beforeValue));
    }
    else
    {
      rvalue = shared_ptr<Symbol>(
          new Temporary(getTempName(),
                        shared_ptr<Type const>(new BaseType(BASETYPE_BOOL))));
    }
    int index = symbolTable_->insert(rvalue);
    exprNode->setRvalueIndex(index);
    popAndPush(1, exprNode);
    break;
  }
  case 231: // unaryExpr -> sizeof , ID
  {         // ID's type width is the value
    IdNode *idNode = dynamic_cast<IdNode *>(stack_.back());
    int index = idNode->getSymbolTableIndex();
    if (index < 0)
    {
      printError(*idNode, "identifier was not declared\n");
    }
    shared_ptr<Type const> type = symbolTable_->lookUp(index)->getType();
    int width = type->getWidth();
    ExprNode *exprNode =
        new ExprNode(231, shared_ptr<Type const>(new BaseType(BASETYPE_INT)), true);
    // Setting memory for temporary
    shared_ptr<Symbol> rvalue(new Constant(width));
    index = symbolTable_->insert(rvalue);
    exprNode->setRvalueIndex(index);
    popAndPush(1, exprNode);
    break;
  }
  case 232: // unaryExpr -> sizeof, typeSpec
  {         // given type's width is the value
    TypedNode *typeSpecNode = dynamic_cast<TypedNode *>(stack_.back());
    shared_ptr<Type const> type = typeSpecNode->getType();
    int width = type->getWidth();
    ExprNode *exprNode =
        new ExprNode(232, shared_ptr<Type const>(new BaseType(BASETYPE_INT)), true);
    // Setting memory for temporary
    shared_ptr<Symbol> rvalue(new Constant(width));
    int index = symbolTable_->insert(rvalue);
    exprNode->setRvalueIndex(index);
    popAndPush(1, exprNode);
    break;
  }
  case 233: // UnaryExpr -> ConstantExpr
  {
    break;
  }
  case 234: // ConstantExpr -> integerLiteral (constantNode)
  case 235: // ConstantExpr -> STRING_LITERAL (constantNode)
  case 236: // ConstantExpr -> CHAR_LITERAL (constantNode)
  {
    ConstantNode *constantNode = dynamic_cast<ConstantNode *>(stack_.back());
    int index = constantNode->getSymbolTableIndex();
    shared_ptr<Type const> constantType = symbolTable_->lookUp(index)->getType();
    ExprNode *constExpr = new ExprNode(productionKey, constantType, true);
    // Setting memory for temporary
    constExpr->setRvalueIndex(index);
    popAndPush(1, constExpr);
  }
  case 237: // integerLiteral -> DECIMAL_LITERAL
  case 238: // integerLiteral -> HEXADECIMAL_LITERAL
  {         // does nothing: Constant node were already made when shifting tokens
    break;
  }
  case 241: // PostfixExpr -> PrimaryExpr
  {         // does nothing
    break;
  }
  case 242: // PostfixExpr -> PostfixExpr [ Expr ]
  {
    LvalueNode *lvalueNode =
        dynamic_cast<LvalueNode *>(stack_[stack_.size() - 2]);
    ExprNode *arrayIndexNode =
        dynamic_cast<ExprNode *>(stack_.back());
    shared_ptr<Type const> exprType = lvalueNode->getType();
    shared_ptr<Type const> arrayIndexType = arrayIndexNode->getType();
    if (!arrayIndexType->isNumeric())
    {
      printError("array index should be numeric type");
    }
    if (exprType->getType() != TYPE_ARRAY)
    {
      printError("[] operator can only be used with array type");
    }
    shared_ptr<ArrayType const> arrayType =
        dynamic_pointer_cast<ArrayType const>(exprType);
    shared_ptr<Type const> elementType = arrayType->getElementType();
    LvalueNode *resultNode = new LvalueNode(242, elementType);
    // Setting memory for l-value and r-value
    // temp is for multiplication result of index calculation
    shared_ptr<Symbol> index(new Temporary(getTempName(),
                                           shared_ptr<Type const>(new BaseType(BASETYPE_INT))));
    shared_ptr<Symbol> temp(new Temporary(getTempName(),
                                          shared_ptr<Type const>(new BaseType(BASETYPE_INT))));
    shared_ptr<Symbol> rvalue(new Temporary(getTempName(), elementType));
    int indexIndex = symbolTable_->insert(index);
    int tempIndex = symbolTable_->insert(temp);
    int rvalueIndex = symbolTable_->insert(rvalue);
    resultNode->setBaseIndex(lvalueNode->getBaseIndex());
    resultNode->setIndexIndex(indexIndex);
    resultNode->setTempIndex(tempIndex);
    resultNode->setRvalueIndex(rvalueIndex);

    popAndPush(2, resultNode);
    break;
  }

  case 243: // PostfixExpr -> PostfixExpr . ID
  {
    LvalueNode *lvalueNode =
        dynamic_cast<LvalueNode *>(stack_[stack_.size() - 2]);
    IdNode const *fieldNameNode =
        dynamic_cast<IdNode const *>(stack_.back());
    shared_ptr<Type const> exprType = lvalueNode->getType();
    string const &field = fieldNameNode->getLexeme();
    if (exprType->getType() != TYPE_RECORD)
    {
      printError("dot selector can only be used with struct type.");
    }
    shared_ptr<RecordType const> recordType =
        dynamic_pointer_cast<RecordType const>(exprType);
    shared_ptr<Type const> fieldType = recordType->getTypeOf(field);
    if (fieldType->isError())
    {
      printError("the struct type does not have specified field");
    }
    LvalueNode *resultNode = new LvalueNode(243, fieldType);
    // Setting memory for l-value and r-value
    shared_ptr<Symbol> index(new Temporary(getTempName(),
                                           shared_ptr<Type const>(new BaseType(BASETYPE_INT))));
    shared_ptr<Symbol> rvalue(new Temporary(getTempName(), fieldType));
    int indexIndex = symbolTable_->insert(index);
    int rvalueIndex = symbolTable_->insert(rvalue);
    resultNode->setBaseIndex(lvalueNode->getBaseIndex());
    resultNode->setIndexIndex(indexIndex);
    resultNode->setRvalueIndex(rvalueIndex);

    popAndPush(2, resultNode);
    break;
  }
  case 244: // PostfixExpr -> PostfixExpr ( ArgumentList )
  {
    ExprNode *functionNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 2]);
    ArgumentListNode *argumentListNode =
        dynamic_cast<ArgumentListNode *>(stack_.back());
    shared_ptr<Type const> tempType = functionNode->getType();
    if (tempType->getType() != TYPE_FUNC)
    {
      printError("the expression is not a function type (can not call)");
    }
    shared_ptr<FuncType const> funcType =
        dynamic_pointer_cast<FuncType const>(tempType);
    shared_ptr<Type const> codomainType = funcType->getCodomainType();
    // Both domain type and arguement type are product types: even void
    // argument list are represented by empty product.
    shared_ptr<ProductType const> domainType = funcType->getDomainType();
    shared_ptr<ProductType const> argumentType = argumentListNode->getType();
    if (domainType->getNum() != argumentType->getNum())
    {
      printError("the number of function call arguments is wrong");
    }
    if (!isCompatible(*domainType, *argumentType))
    {
      printError("the type of argument to the function call is wrong");
    }
    // Function's return expression can not be l-value
    ExprNode *exprNode = new ExprNode(244, codomainType, false);
    // Setting memory for temporary
    shared_ptr<Symbol> rvalue(new Temporary(getTempName(), codomainType));
    int index = symbolTable_->insert(rvalue);
    exprNode->setRvalueIndex(index);

    popAndPush(2, exprNode);
    break;
  }

  case 251: // PrimaryExpr -> ID
  {
    IdNode *idNode = dynamic_cast<IdNode *>(stack_.back());
    int index = idNode->getSymbolTableIndex();
    if (index < 0)
    { // not declared ID
      printError(*idNode, "identifier not declared");
    }
    shared_ptr<Symbol const> symbol = symbolTable_->lookUp(index);
    shared_ptr<Type const> type = symbol->getType();
    LvalueNode *lvalueNode = new LvalueNode(251, type);
    // Setting memory for l-value and r-value
    lvalueNode->setBaseIndex(index); // index of the ID symbol
    lvalueNode->setRvalueIndex(index);
    popAndPush(1, lvalueNode);
    break;
  }

  case 252: // PrimaryExpr -> ( Expr )
  {         // does nothing
    break;
  }
  case 271: // argumentList -> empty
  {         // using key 273
    ArgumentListNode *argumentListNode = new ArgumentListNode(273);
    stack_.push_back(argumentListNode);
    break;
  }
  case 272: // argumentList -> expr
  {         // using key 273
    ArgumentListNode *argumentListNode = new ArgumentListNode(273);
    ExprNode *exprNode = dynamic_cast<ExprNode *>(stack_.back());
    shared_ptr<Type const> argType = exprNode->getType();
    argumentListNode->pushArgType(argType);
    popAndPush(1, argumentListNode);
    break;
  }
  case 273: // argumentList -> argumentList , expr
  {
    ArgumentListNode *argumentListNode =
        dynamic_cast<ArgumentListNode *>(stack_[stack_.size() - 2]);
    ExprNode *exprNode =
        dynamic_cast<ExprNode *>(stack_.back());
    shared_ptr<Type const> argType = exprNode->getType();
    argumentListNode->pushChild(exprNode);
    argumentListNode->pushArgType(argType);
    exprNode->setParent(argumentListNode);
    stack_.pop_back();
    break;
  }
  case 281: // +
  case 282: // -
  case 283: // *
  case 284: // /
  case 285: // %
  case 286: // &
  case 287: // |
  case 288: // ^
  {         // returns bigger type of two operands
    ExprNode *leftExprNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 2]);
    ExprNode *rightExprNode =
        dynamic_cast<ExprNode *>(stack_.back());
    shared_ptr<Type const> leftType = leftExprNode->getType();
    shared_ptr<Type const> rightType = rightExprNode->getType();
    if (!leftType->isNumeric() || !rightType->isNumeric())
    {
      printError("arithmetic binary operator can only be used with numeric types");
    }
    shared_ptr<Type const> biggerType = getMaxOfTypes(leftType, rightType);
    ExprNode *exprNode = new ExprNode(productionKey, biggerType,
                                      leftExprNode->isConstant() && rightExprNode->isConstant());
    // Setting memory for temporary
    shared_ptr<Symbol> rvalue(new Temporary(getTempName(), biggerType));
    int rvalueIndex = symbolTable_->insert(rvalue);
    exprNode->setRvalueIndex(rvalueIndex);

    popAndPush(2, exprNode);
    break;
  }
  case 289: // >
  case 290: // >=
  case 291: // <
  case 292: // <=
  case 293: // == : equivalence can be tested with record type? (NO in C)
  case 294: // !=
  case 295: // ||
  case 296: // &&
  {         // return boolean type
    ExprNode *leftExprNode =
        dynamic_cast<ExprNode *>(stack_[stack_.size() - 2]);
    ExprNode *rightExprNode =
        dynamic_cast<ExprNode *>(stack_.back());
    shared_ptr<Type const> leftType = leftExprNode->getType();
    shared_ptr<Type const> rightType = rightExprNode->getType();
    if (!leftType->isNumeric() || !rightType->isNumeric())
    {
      printError("logical binary operator can only be used with boolable types");
    }
    ExprNode *exprNode = new ExprNode(productionKey,
                                      shared_ptr<Type const>(new BaseType(BASETYPE_BOOL)),
                                      leftExprNode->isConstant() && rightExprNode->isConstant());
    // Setting memory for temporary
    shared_ptr<Symbol> rvalue(new Temporary(getTempName(),
                                            shared_ptr<Type const>(new BaseType(BASETYPE_BOOL))));
    int rvalueIndex = symbolTable_->insert(rvalue);
    exprNode->setRvalueIndex(rvalueIndex);

    popAndPush(2, exprNode);
    break;
  }

  case 501: // DUMMY_PushSymTab -> empty
    symbolTable_->pushTable();
    break;
  case 502: // DUMMY_PopSymTab -> empty
    symbolTable_->popTable();
    break;
  case 503: // DUMMY_PushEnv -> empty
    symbolTable_->pushEnvironment();
    break;
  case 504: // DUMMY_PushFunc -> empty
    symbolTable_->pushFunction();
  default:
    return;
  }
}

// Do additional static checking (top-down) and generate code
void Parser::check()
{
  if (root_ == NULL)
  { // the source code was erroneous in previous step
    std::cout << "Parser::check() error\n";
    exit(0);
  }
  // Find all functions definitions and run recursive routine
  NonTerminalNode *root = dynamic_cast<NonTerminalNode *>(root_);
  for (int i = 0; i < root->getNumChildren(); ++i)
  {
    // For each global declarations,
    NonTerminalNode *decl = dynamic_cast<NonTerminalNode *>(root->getChild(i));
    if (decl->getProductionKey() == 82)
    { // if a function definition,
      StmtNode *compoundStmt = dynamic_cast<StmtNode *>(decl->getChild(1));
      check(compoundStmt, -1); // there can not be a break in global
    }
  }
}

void Parser::check(StmtNode *node, int breakLabel)
{
  //cout << "doing gencode for " << node.getProductionKey() << endl;
  switch (node->getProductionKey())
  {
  case 101: // CompoundStmt
  {         // just postpone to stmtListNode
    StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(1));
    check(stmtNode, breakLabel);
    break;
  }
  case 122: // StmtList
  {         // just look through all children
    for (int i = 0; i < node->getNumChildren(); ++i)
    {
      StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(i));
      check(stmtNode, breakLabel);
    }
    break;
  }
  case 141: // IfStmt -> Expr , Stmt , ElseStmt
  {
    StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(1));
    StmtNode *elseStmtNode = dynamic_cast<StmtNode *>(node->getChild(2));
    check(stmtNode, breakLabel);
    check(elseStmtNode, breakLabel);
    break;
  }
  case 151: // WhileStmt -> Expr , Stmt
  {
    StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(1));
    check(stmtNode, 1);
    break;
  }
  case 161: // Stmt -> BREAK
  {
    if (breakLabel < 0)
    {
      printError("this break statement is illegal");
    }
    break;
  }
  case 171: // SwitchStmt -> Expr , SwitchCaseList , SwitchDefaultCase
  {
    StmtNode *caseList = dynamic_cast<StmtNode *>(node->getChild(1));
    StmtNode *defaultCase = dynamic_cast<StmtNode *>(node->getChild(2));
    check(caseList, 1);
    check(defaultCase, 1);
    break;
  }
  case 172: // SwitchCaselist
  {         // just look through all children
    for (int i = 0; i < node->getNumChildren(); ++i)
    {
      StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(i));
      check(stmtNode, breakLabel);
    }
    break;
  }
  case 174: // SwitchCase -> Expr , Stmt
  {
    StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(1));
    check(stmtNode, breakLabel);
    break;
  }
  default:
  {
    //cout << "terminal stmt is " << node.getProductionKey() << endl;
    break;
  }
  }
}

void Parser::popAndPush(int childrenNum, NonTerminalNode *parent)
{
  for (int i = 0; i < childrenNum; ++i)
  {
    SyntaxTreeNode *child = stack_[stack_.size() - childrenNum + i];
    child->setParent(parent);
    parent->pushChild(child);
  }
  stack_.resize(stack_.size() - childrenNum);
  stack_.push_back(parent);
}

// This prints the lexeme and line number of error detection: note that
// ..this might be inapproriate in many cases.
void Parser::printError(std::string const &errorMessage) const
{
  cout << "error (in line " << lexer_.getLineNum() << "): " << errorMessage << endl;
  exit(0);
}

// When error line could be identified
void Parser::printError(int lineNum,
                        std::string const &errorMessage) const
{
  cout << "error (in line " << lineNum << "): " << errorMessage << endl;
  exit(0);
}

// This prints the source of the error with given id node
void Parser::printError(IdNode const &idNode,
                        std::string const &errorMessage) const
{
  cout << "error (in line " << idNode.getLineNum() << ", '";
  cout << idNode.getLexeme();
  cout << "'): " << errorMessage << endl;
  exit(0);
}

void Parser::printWarning(std::string const &warningMessage) const
{
  cout << "warning (in line " << lexer_.getLineNum() << "): " << warningMessage << endl;
}

subcc::string
Parser::getTempName()
{
  int number = curTemporaryNumber_++;
  string temp("_t");
  string num = stringOfInt(number);
  return temp + num;
}

enum NonTerminal
{
  Program = 251,
  DeclarationList = 252,
  Declaration = 253,
  NormalDeclaration = 254,  // non-function declaration
  FuncDeclaration = 255,    // declaration or definition
  FuncDefinitionHead = 256, // declaration part of definition
  TypedefDeclaration = 257,
  VarDeclaration = 258,
  StructDeclaration = 259,

  TypeSpecifier = 271,
  ArrayDeclaration = 272,
  ArrayDeclarationList = 273,
  StructTypeDeclaration = 274,
  StructType = 275,
  StructTypeReference = 276,
  LocalDeclarationList = 277,
  Param = 278,
  Params = 279,
  ParamList = 280,

  CompoundStatement = 281,
  StatementList = 282,
  Statement = 283,
  AssignStatement = 284,
  SelectionStatement = 285,
  IterationStatement = 286,
  BreakStatement = 287,
  ContinueStatement = 294,
  SwitchStatement = 288,
  ReturnStatement = 289,
  ElseStatement = 290,
  SwitchCaseList = 291,
  SwitchCase = 292,
  SwitchDefaultCase = 293,

  Expr = 301,
  UnaryExpr = 302,
  BinaryExpr = 303,
  UnaryOperator = 304,
  PostfixExpr = 305,
  PrimaryExpr = 306,
  ConstantExpr = 307,

  ArgumentList = 312,
  IntegerLiteral = 313
};

enum Dummy
{ // for SDT (special nonterminals)
  DUMMY_PushSymTab = 401,
  DUMMY_PushEnv = 402,
  DUMMY_PushFunc = 403,
  DUMMY_PopSymTab = 410
};

void Parser::makeLanguage(ContextFreeLanguage &subcc)
{
  /*******  register each Production ********/
  subcc.add(1, Program, DeclarationList, kEndMarker);
  subcc.add(10, DeclarationList, kEndMarker);
  subcc.add(11, DeclarationList, DeclarationList, Declaration, kEndMarker);
  subcc.add(12, NormalDeclaration, TypedefDeclaration, PUNC_SEMICOLON, kEndMarker);
  subcc.add(14, NormalDeclaration, VarDeclaration, kEndMarker);
  subcc.add(15, NormalDeclaration, StructDeclaration, kEndMarker);
  subcc.add(16, Declaration, FuncDeclaration, kEndMarker);
  subcc.add(17, Declaration, NormalDeclaration, kEndMarker); // non-functions decls
  subcc.add(21, TypedefDeclaration, TYPEDEF, TypeSpecifier, ID, kEndMarker);
  subcc.add(31, VarDeclaration, TypeSpecifier, ID, PUNC_SEMICOLON, kEndMarker);
  // ex: char name[80] = "calvin";
  // subcc doesn't have pointer, so I will implement this as simple assign of type 'char[80]'
  //subcc.add(32, VarDeclaration, TypeSpecifier, ID, ArrayDeclaration, ASSIGN,
  //  STRING_LITERAL, PUNC_SEMICOLON, kEndMarker);
  subcc.add(33, VarDeclaration, TypeSpecifier, ID, ArrayDeclarationList, PUNC_SEMICOLON, kEndMarker);
  subcc.add(41, ArrayDeclarationList, ArrayDeclarationList, ArrayDeclaration, kEndMarker);
  subcc.add(42, ArrayDeclarationList, ArrayDeclaration, kEndMarker);
  subcc.add(43, ArrayDeclaration, DELIM_BRACKET_OPEN, IntegerLiteral, DELIM_BRACKET_CLOSE, kEndMarker);
  subcc.add(51, StructDeclaration, StructTypeDeclaration, PUNC_SEMICOLON, kEndMarker);
  // Be careful: push environment (not symtab) for type declaration
  subcc.add(52, StructTypeDeclaration, STRUCT, ID, DELIM_BRACE_OPEN, DUMMY_PushEnv,
            LocalDeclarationList, DUMMY_PopSymTab, DELIM_BRACE_CLOSE, kEndMarker);
  subcc.add(61, TypeSpecifier, VOID_, kEndMarker);
  subcc.add(62, TypeSpecifier, CHAR, kEndMarker);
  subcc.add(63, TypeSpecifier, SHORT, kEndMarker);
  subcc.add(64, TypeSpecifier, INT, kEndMarker);
  subcc.add(65, TypeSpecifier, StructType, kEndMarker);
  subcc.add(66, TypeSpecifier, USERDEFTYPE, kEndMarker);
  subcc.add(71, StructType, StructTypeDeclaration, kEndMarker);
  subcc.add(72, StructType, StructTypeReference, kEndMarker);
  subcc.add(73, StructTypeReference, STRUCT, ID, kEndMarker);
  subcc.add(81, FuncDeclaration, TypeSpecifier, ID, DELIM_PAREN_OPEN, DUMMY_PushFunc,
            Params, DELIM_PAREN_CLOSE, DUMMY_PopSymTab, PUNC_SEMICOLON, kEndMarker);
  subcc.add(82, FuncDeclaration, FuncDefinitionHead, CompoundStatement, DUMMY_PopSymTab, kEndMarker);
  subcc.add(83, FuncDefinitionHead, TypeSpecifier, ID, DELIM_PAREN_OPEN, DUMMY_PushFunc,
            Params, DELIM_PAREN_CLOSE, kEndMarker);
  subcc.add(91, Params, kEndMarker);
  subcc.add(92, Params, VOID_, kEndMarker);
  subcc.add(93, Params, ParamList, kEndMarker);
  subcc.add(94, ParamList, ParamList, PUNC_COMMA, Param, kEndMarker);
  subcc.add(95, ParamList, Param, kEndMarker);
  subcc.add(96, Param, TypeSpecifier, ID, kEndMarker);
  subcc.add(97, Param, TypeSpecifier, ID, ArrayDeclarationList, kEndMarker);
  subcc.add(101, CompoundStatement, DELIM_BRACE_OPEN, DUMMY_PushSymTab, LocalDeclarationList,
            StatementList, DUMMY_PopSymTab, DELIM_BRACE_CLOSE, kEndMarker);
  subcc.add(111, LocalDeclarationList, kEndMarker);
  subcc.add(112, LocalDeclarationList, LocalDeclarationList, NormalDeclaration, kEndMarker);

  subcc.add(121, StatementList, kEndMarker);
  subcc.add(122, StatementList, StatementList, Statement, kEndMarker);
  subcc.add(123, Statement, Expr, PUNC_SEMICOLON, kEndMarker); // stmt -> expr
  subcc.add(124, Statement, CompoundStatement, kEndMarker);
  subcc.add(125, Statement, SelectionStatement, kEndMarker);
  subcc.add(126, Statement, IterationStatement, kEndMarker);
  subcc.add(127, Statement, BreakStatement, PUNC_SEMICOLON, kEndMarker);
  subcc.add(130, Statement, ContinueStatement, PUNC_SEMICOLON, kEndMarker);
  subcc.add(128, Statement, SwitchStatement, kEndMarker);
  subcc.add(129, Statement, ReturnStatement, PUNC_SEMICOLON, kEndMarker);
  subcc.add(141, SelectionStatement, IF, DELIM_PAREN_OPEN, Expr, DELIM_PAREN_CLOSE,
            Statement, ElseStatement, kEndMarker);
  subcc.add(142, ElseStatement, kEndMarker);
  subcc.add(143, ElseStatement, ELSE, Statement, kEndMarker);
  subcc.add(151, IterationStatement, WHILE, DELIM_PAREN_OPEN, Expr, DELIM_PAREN_CLOSE,
            Statement, kEndMarker);
  subcc.add(161, BreakStatement, BREAK, kEndMarker);
  subcc.add(162, ContinueStatement, CONTINUE, kEndMarker);
  subcc.add(171, SwitchStatement, SWITCH, DELIM_PAREN_OPEN, Expr, DELIM_PAREN_CLOSE,
            DELIM_BRACE_OPEN, SwitchCaseList, SwitchDefaultCase, DELIM_BRACE_CLOSE, kEndMarker);
  subcc.add(172, SwitchCaseList, SwitchCaseList, SwitchCase, kEndMarker);
  subcc.add(173, SwitchCaseList, SwitchCase, kEndMarker);
  subcc.add(174, SwitchCase, CASE, Expr, PUNC_COLON, StatementList, kEndMarker);
  subcc.add(175, SwitchDefaultCase, kEndMarker);
  subcc.add(176, SwitchDefaultCase, DEFAULT, PUNC_COLON, StatementList, kEndMarker);
  subcc.add(181, ReturnStatement, RETURN, kEndMarker);
  subcc.add(182, ReturnStatement, RETURN, Expr, kEndMarker);

  // Assign expressions
  subcc.add(191, Expr, UnaryExpr, ASSIGN, Expr, kEndMarker);
  subcc.add(192, Expr, UnaryExpr, ASSIGN_PLUS, Expr, kEndMarker);
  subcc.add(193, Expr, UnaryExpr, ASSIGN_MINUS, Expr, kEndMarker);
  subcc.add(194, Expr, UnaryExpr, ASSIGN_MULTI, Expr, kEndMarker);
  subcc.add(195, Expr, UnaryExpr, ASSIGN_DIVIDE, Expr, kEndMarker);
  subcc.add(196, Expr, UnaryExpr, ASSIGN_MOD, Expr, kEndMarker);
  subcc.add(197, Expr, UnaryExpr, ASSIGN_BIT_AND, Expr, kEndMarker);
  subcc.add(198, Expr, UnaryExpr, ASSIGN_BIT_OR, Expr, kEndMarker);
  subcc.add(199, Expr, UnaryExpr, ASSIGN_BIT_XOR, Expr, kEndMarker);

  subcc.add(211, Expr, UnaryExpr, kEndMarker);
  subcc.add(212, Expr, BinaryExpr, kEndMarker);

  subcc.add(221, UnaryExpr, PostfixExpr, kEndMarker);
  subcc.add(222, UnaryExpr, ARITH_PLUS, UnaryExpr, kEndMarker);
  subcc.add(223, UnaryExpr, ARITH_MINUS, UnaryExpr, kEndMarker);
  subcc.add(224, UnaryExpr, ARITH_INCREMENT, UnaryExpr, kEndMarker);
  subcc.add(225, UnaryExpr, ARITH_DECREMENT, UnaryExpr, kEndMarker);
  subcc.add(226, UnaryExpr, LOGIC_NOT, UnaryExpr, kEndMarker);
  subcc.add(227, UnaryExpr, LOGIC_BIT_NOT, UnaryExpr, kEndMarker);

  subcc.add(231, UnaryExpr, SIZEOF, DELIM_PAREN_OPEN, ID, DELIM_PAREN_CLOSE, kEndMarker);
  subcc.add(232, UnaryExpr, SIZEOF, DELIM_PAREN_OPEN, TypeSpecifier, DELIM_PAREN_CLOSE, kEndMarker);
  subcc.add(233, UnaryExpr, ConstantExpr, kEndMarker);
  subcc.add(234, ConstantExpr, IntegerLiteral, kEndMarker);
  subcc.add(235, ConstantExpr, STRING_LITERAL, kEndMarker);
  subcc.add(236, ConstantExpr, CHAR_LITERAL, kEndMarker);
  subcc.add(237, IntegerLiteral, DECIMAL_LITERAL, kEndMarker);
  subcc.add(238, IntegerLiteral, HEXADECIMAL_LITERAL, kEndMarker);

  subcc.add(241, PostfixExpr, PrimaryExpr, kEndMarker);
  subcc.add(242, PostfixExpr, PostfixExpr, DELIM_BRACKET_OPEN, Expr, DELIM_BRACKET_CLOSE, kEndMarker);
  subcc.add(243, PostfixExpr, PostfixExpr, PUNC_PERIOD, ID, kEndMarker);
  subcc.add(244, PostfixExpr, PostfixExpr, DELIM_PAREN_OPEN, ArgumentList, DELIM_PAREN_CLOSE, kEndMarker);

  subcc.add(251, PrimaryExpr, ID, kEndMarker);
  subcc.add(252, PrimaryExpr, DELIM_PAREN_OPEN, Expr, DELIM_PAREN_CLOSE, kEndMarker);

  subcc.add(271, ArgumentList, kEndMarker);
  subcc.add(272, ArgumentList, Expr, kEndMarker);
  subcc.add(273, ArgumentList, ArgumentList, PUNC_COMMA, Expr, kEndMarker);

  subcc.add(281, BinaryExpr, Expr, ARITH_PLUS, Expr, kEndMarker);
  subcc.add(282, BinaryExpr, Expr, ARITH_MINUS, Expr, kEndMarker);
  subcc.add(283, BinaryExpr, Expr, ARITH_MULTI, Expr, kEndMarker);
  subcc.add(284, BinaryExpr, Expr, ARITH_DIVIDE, Expr, kEndMarker);
  subcc.add(285, BinaryExpr, Expr, ARITH_MOD, Expr, kEndMarker);
  subcc.add(286, BinaryExpr, Expr, LOGIC_BIT_AND, Expr, kEndMarker);
  subcc.add(287, BinaryExpr, Expr, LOGIC_BIT_OR, Expr, kEndMarker);
  subcc.add(288, BinaryExpr, Expr, LOGIC_BIT_XOR, Expr, kEndMarker);
  subcc.add(289, BinaryExpr, Expr, COMP_GT, Expr, kEndMarker);
  subcc.add(290, BinaryExpr, Expr, COMP_GE, Expr, kEndMarker);
  subcc.add(291, BinaryExpr, Expr, COMP_LT, Expr, kEndMarker);
  subcc.add(292, BinaryExpr, Expr, COMP_LE, Expr, kEndMarker);
  subcc.add(293, BinaryExpr, Expr, COMP_EQ, Expr, kEndMarker);
  subcc.add(294, BinaryExpr, Expr, COMP_NE, Expr, kEndMarker);
  subcc.add(295, BinaryExpr, Expr, LOGIC_OR, Expr, kEndMarker);
  subcc.add(296, BinaryExpr, Expr, LOGIC_AND, Expr, kEndMarker);

  // For dummy nonterminals
  subcc.add(501, DUMMY_PushSymTab, kEndMarker);
  subcc.add(502, DUMMY_PopSymTab, kEndMarker);
  subcc.add(503, DUMMY_PushEnv, kEndMarker);
  // open function scope (right before the first parameter)
  subcc.add(504, DUMMY_PushFunc, kEndMarker);
  subcc.finalize();
}

void Parser::makeParser(LalrParserGenerator &myParser)
{
  myParser.buildFast();

  /*************  Set precedence and associativity  ****************/
  // ++, --
  myParser.setPrecedence(ARITH_INCREMENT, 90);
  myParser.setPrecedence(ARITH_DECREMENT, 90);
  myParser.setAssociation(90, Association::Left);
  // !, ~
  myParser.setPrecedence(LOGIC_BIT_NOT, 80);
  myParser.setPrecedence(LOGIC_NOT, 80);
  myParser.setAssociation(80, Association::Right);
  // *, /, %
  myParser.setPrecedence(ARITH_MULTI, 75);
  myParser.setPrecedence(ARITH_DIVIDE, 75);
  myParser.setPrecedence(ARITH_MOD, 75);
  myParser.setAssociation(75, Association::Left);
  // +, -
  myParser.setPrecedence(ARITH_PLUS, 70);
  myParser.setPrecedence(ARITH_MINUS, 70);
  myParser.setAssociation(70, Association::Left);
  // <, <=, >, >=
  myParser.setPrecedence(COMP_GT, 65);
  myParser.setPrecedence(COMP_GE, 65);
  myParser.setPrecedence(COMP_LT, 65);
  myParser.setPrecedence(COMP_LE, 65);
  myParser.setAssociation(65, Association::None);
  // ==, !=
  myParser.setPrecedence(COMP_EQ, 60);
  myParser.setPrecedence(COMP_NE, 60);
  myParser.setAssociation(60, Association::None);
  // &
  myParser.setPrecedence(LOGIC_BIT_AND, 55);
  myParser.setAssociation(55, Association::Left);
  // ^
  myParser.setPrecedence(LOGIC_BIT_XOR, 50);
  myParser.setAssociation(50, Association::Left);
  // |
  myParser.setPrecedence(LOGIC_BIT_OR, 45);
  myParser.setAssociation(45, Association::Left);
  // &&
  myParser.setPrecedence(LOGIC_AND, 40);
  myParser.setAssociation(40, Association::Left);
  // ||
  myParser.setPrecedence(LOGIC_OR, 35);
  myParser.setAssociation(35, Association::Left);
  // =, +=, -=, *=, /=, %=, &=, |=, ^=
  myParser.setPrecedence(ASSIGN, 20);
  myParser.setPrecedence(ASSIGN_PLUS, 20);
  myParser.setPrecedence(ASSIGN_MINUS, 20);
  myParser.setPrecedence(ASSIGN_MULTI, 20);
  myParser.setPrecedence(ASSIGN_DIVIDE, 20);
  myParser.setPrecedence(ASSIGN_MOD, 20);
  myParser.setPrecedence(ASSIGN_BIT_XOR, 20);
  myParser.setPrecedence(ASSIGN_BIT_AND, 20);
  myParser.setPrecedence(ASSIGN_BIT_OR, 20);
  myParser.setAssociation(20, Association::Right);

  myParser.checkConflict();
  myParser.initialize();
}

} // end namespace subcc