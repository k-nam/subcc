/* 2008-06-08
 */
#pragma once

#include "parser.h"

/* 2008-06-09: Done basic classes. It took me some time to get the feelings
 * ..of l-value and r-values of expressions. For each ExprNode, I will prepare 
 * ..one 'value' temporary which has the same type as the expression and will
 * ..hold the r-value of the expression if needed. Every r-valued expression
 * ..is required to hold the value, but some l-valued expressions are not. For
 * ..example, in  x = (a + b), the r-value of x is not needed anywhere. 
 * L-valued temporaries will basically hold their l-value only, which consists
 * ..of 'base address' and 'offset'. Base address should be a NAME address, and
 * ..offset can be any kind of address. The resulting l-value is: the l-value of
 * ..the NAME plus the r-value of the offset. 
 * If we use pointers, the * operator will take an r-value and return an 
 * ..l-valued temporary of appropriate type (the type to which the pointer
 * ..points)
 * 2008-06-10: Starting to implement actual translation methods.
 */

namespace subcc 
{
class ThreeAdressCodeGenerator;
}

namespace subcc {
extern int instructionCount;

enum BinOps {
  BinOp_Arith_Plus      = 1,
  BinOp_Arith_Minus     = 2,
  BinOp_Arith_Multi     = 3,
  BinOp_Arith_Divide    = 4,
  BinOp_Arith_Mod       = 5,
  BinOp_Logic_And       = 11,
  BinOp_Logic_Or        = 12,
  BinOp_Logic_Bit_And   = 21,
  BinOp_Logic_Bit_Or    = 22,
  BinOp_Logic_Bit_Xor   = 23,
  BinOp_Rel_EQ          = 31,
  BinOp_Rel_NE          = 32,
  BinOp_Rel_GT          = 33,
  BinOp_Rel_GE          = 34,
  BinOp_Rel_LT          = 35,
  BinOp_Rel_LE          = 36
};

enum UniOp {
  UniOp_Arith_Minus     = 101,
  UniOp_Arith_Inc       = 102,
  UniOp_Arith_Dec       = 103,
  UniOp_Logic_Not       = 111,
  UniOp_Logic_Bit_Not   = 121
};

// Address: Name, Constant, or Temporary
enum AddressType {
  AddressType_Name         = 1,
  AddressType_Constant     = 2,
  AddressType_Temporary    = 3,
  AddressType_Null         = 4
};

enum InstructionType {
  Instruction_Function_Start       = 1,  
  Instruction_Function_End         = 2,  
  Instruction_Label                = 5,  
  Instruction_Assign_Binary        = 31,
  Instruction_Assign_Unary         = 32,
  Instruction_Assign_Copy          = 33,
  Instruction_Function_Call        = 11,
  Instruction_Return               = 12,
  Instruction_Parameter            = 15,
  Instruction_Pop                  = 16,
  Instruction_Jump                 = 21,
  Instruction_Branch_Unary         = 22,
  Instruction_Branch_Unary_Not     = 23,
  Instruction_Branch_Binary        = 24,
  Instruction_Copy_Indexed_Src     = 41,
  Instruction_Copy_Indexed_Target  = 42
};


/********************************* Address **********************************/

// Array or dot selectors must be transformed to indexed copy instruction:
// ..three address code does not allow indexed addressing mode.
// For example, in x = foo.bar, the address of the rhs expression is a 
// ..temporary symbol (of bar's type), whose l-value has been calculated
// ..just before with the dot selector. 
class Address {
public:
  Address(); // null addrsss
  Address(enum AddressType addressType, int symtabIndex);
  enum AddressType getType() const;
  int getSymbolTableIndex() const;
  bool isNull() const;
protected:
  enum AddressType addressType_;
  int symbolTableIndex_; // key to the symbol
};


/******************************** Instruction *******************************/

class Instruction {
public:
  enum InstructionType getType() const;
  virtual ~Instruction();
public:
	static void countObjects();
protected:
  Instruction(enum InstructionType instType);
  enum InstructionType instructionType_;
private:
	static int objectCount_;
};

// foo:
class FunctionStartInstruction: public Instruction {
public:
  FunctionStartInstruction(string const &functionName, int stackSize);
  string const &getFunctionName() const;
  int getStackSize() const;
protected:
  string functionName_;
  int stackSize_;
};

// Just for convenience; mark the end of a function
class FunctionEndInstruction: public Instruction {
public:
  FunctionEndInstruction(string const &functionName, int stackSize);
  string const &getFunctionName() const;
  int getStackSize() const;
protected:
  string functionName_;
  int stackSize_;
};

// L34:
class LabelInstruction: public Instruction {
public:
  LabelInstruction(int labelNumber);
  int getLabel() const;
protected:
  int label_;
};

// Various assignment instructions: abstract class
class AssignInstruction: public Instruction {
public:
  Address const &getTarget() const;
  Address const &getSource() const;
protected:
  AssignInstruction(enum InstructionType, Address const &target,
      Address const &source);
	virtual ~AssignInstruction() { }
  Address source_;
  Address target_;
};

// x = y;
class CopyInstruction: public AssignInstruction {
public:
  CopyInstruction(Address const &target, Address const &src);
};

// x = op y;
class UnaryAssignInstruction: public AssignInstruction {
public:
  UnaryAssignInstruction(Address const &target, enum UniOp, 
      Address const &src);
  enum UniOp getOperator() const;
protected:
  enum UniOp operator_;
};

// x = y op z;
class BinaryAssignInstruction: public AssignInstruction {
public:
  BinaryAssignInstruction(Address const &target,
      Address const &srcA, BinOps, Address const &srcB);
  Address const &getSrcA() const;
  Address const &getSrcB() const;
  BinOps getOperator() const;
protected:
  Address srcB_;
  BinOps operator_;
private: 
  Address const &getSource() const; // shadow 
};

// x = call (y)
class FunctionCallInstruction: public Instruction {
public:
  FunctionCallInstruction(Address const &returnValue, 
      Address const &function);
  Address const &getFunction() const;
  Address const &getReturnValue() const;
protected:
  Address returnValue_;
  Address function_;
};

// return (x) ; if returning void, set address as null
class ReturnInstruction: public Instruction {
public:
  ReturnInstruction(); // returning void
  ReturnInstruction(Address const &returnValue);
  Address const &getReturnValue() const;
protected:
  Address returnValue_;
};

// param (x) 
class ParameterInstruction: public Instruction {
public:
  ParameterInstruction(Address const &parameter);
  Address const &getParameter() const;
protected:
  Address parameter_;
};

class PopInstruction: public Instruction {
public:
  PopInstruction(int popSize);
  int getPopSize() const;
protected:
  int popSize_;
};

// Unconditional and conditional branches: abstract class
class BranchInstruction: public Instruction {
public:
  int getTargetLabel() const;
protected:
  BranchInstruction(enum InstructionType, int targetLabel);
	virtual ~BranchInstruction() { }
  int targetLabel_;
};

// goto L
class JumpInstruction: public BranchInstruction {
public:
  JumpInstruction(int targetLabel);
};

// if (x) goto L
class UnaryBranchInstruction: public BranchInstruction {
public:
  UnaryBranchInstruction(Address const &cond, int targetLabel);
  Address const &getCondition() const;
protected:
  Address condition_;
};

// if (!x) goto L
class UnaryNotBranchInstruction: public BranchInstruction {
public:
  UnaryNotBranchInstruction(Address const &cond, int targetLabel);
  Address const &getCondition() const;
protected:
  Address condition_;
};

// if (x op y) goto L
class BinaryBranchInstruction: public BranchInstruction {
public: 
  BinaryBranchInstruction(Address const &condA, BinOps, 
      Address const &condB, int targetLabel);
  Address const &getConditionA() const;
  Address const &getConditionB() const;
  BinOps getOperator() const;
protected:
  Address conditionA_;
  Address conditionB_;
  BinOps operator_;
};

// x = y[i]
class IndexedSrcCopyInstruction: public AssignInstruction {
public:
  IndexedSrcCopyInstruction(Address const &target, Address const &src, 
      Address const &index);
  Address const &getIndex() const;
protected:
  Address index_;
};

// x[i] = y
class IndexedTargetCopyInstruction: public AssignInstruction {
public:
  IndexedTargetCopyInstruction(Address const &target, Address const &index, 
      Address const &src);
  Address const &getIndex() const;
protected:
  Address index_;
};

/************************** Code and Code Generator **************************/

// Intermediate Representation: final output of front-end module
// Sequence of three-address instructions with predefined addressing modes
// ..and operators
class ThreeAdressCode {
public:
  ThreeAdressCode(shared_ptr<GlobalSymbolTable const> symbolTable);
	~ThreeAdressCode();
  void pushInstruction(Instruction const *instruction);
  void print() const;
	void print(Instruction const &) const;
  void print(Address const &) const;
  void print(enum UniOp) const;
  void print(BinOps) const;
	shared_ptr<GlobalSymbolTable const> getSymbolTable() const { return symbolTable_; }	
	vector<Instruction const *> const *getInstructions() const { return &instructions_; }
private:
  shared_ptr<GlobalSymbolTable const> symbolTable_;
  vector<Instruction const *> instructions_;
};

// syntax tree (including symbol table)  ---> IR
class ThreeAdressCodeGenerator {
public:
  ThreeAdressCodeGenerator(shared_ptr<SyntaxTree const> syntaxTree);
	~ThreeAdressCodeGenerator() { }
	shared_ptr<ThreeAdressCode const> getCode() const { return code_; }
    
private:
  void generateCode();
  void translateFunction(FunctionNode const *);
  // Call without break and continue label sets those labels to error
  void translateStatement(StmtNode const *, int nextLabel); 
  void translateStatement(StmtNode const *, int nextLabel, 
      int breakLabel, int continueLabel);
  // Two kinds of translation is possible for expressions: control and value
  // Translate for control: two branches (true & false)
  void translateExpression(ExprNode const *, int trueLabel, int falseLabel);
  // Translate for value: we may need its r-value or not
  void translateExpression(ExprNode const *, bool saveRvalue);
  void pushInstruction(Instruction const *instruction);
    
  int getNewLabel();
  Address getAddress(int symtabIndex) const; // finds out address type

	shared_ptr<SyntaxTree const> syntaxTree_; // should hold shared_ptr itself as well
  SyntaxTreeNode const *root_;
  shared_ptr<GlobalSymbolTable const> symbolTable_;
  shared_ptr<ThreeAdressCode> code_;
  int curLabel_;
};

} // End namespace subcc





