#include <iostream>
#include "three_address.h"

// Label which can not exist: indicating error with 'break' or 'continue'

namespace subcc
{
	using std::cout;
	using std::shared_ptr;
	using std::dynamic_pointer_cast;
	using std::cout;
	using std::endl;
	int const errorLabel = -1;

	/********************************* Address ********************************/

	Address::Address() :
		addressType_(AddressType_Null), symbolTableIndex_(-1) { }

	Address::Address(
		enum AddressType addressType, int index) :
		addressType_(addressType), symbolTableIndex_(index) { }

	enum AddressType
		Address::getType() const { return addressType_; }

	int Address::getSymbolTableIndex() const { return symbolTableIndex_; }

	bool Address::isNull() const { return addressType_ == AddressType_Null; }

	/******************************** Instruction *******************************/

	/*** Instruction ***/

	Instruction::Instruction(
		enum InstructionType instType) : instructionType_(instType) {
		objectCount_++;
	}

	Instruction::~Instruction() {
		objectCount_--;
		//std::cout << "instruction count = " << instructionCount << "\n";
	}

	enum InstructionType
		Instruction::getType() const { return instructionType_; }

	void Instruction::countObjects() {
		if (objectCount_ != 0) {
			std::cout << "Instruction object memory leak! # was: " << objectCount_ << "\n";
			exit(0);
		}
		else {
			std::cout << "Instruction object memory OK!\n";
		}
		std::cout << "\n";
	}

	int Instruction::objectCount_ = 0;

	/*** FunctionStartInstrucion ***/

	FunctionStartInstruction::FunctionStartInstruction(c_string const &functionName, int stackSize) :
		Instruction(Instruction_Function_Start), functionName_(functionName),
		stackSize_(stackSize) { }

	subcc::c_string const &
		FunctionStartInstruction::getFunctionName() const { return functionName_; }

	int FunctionStartInstruction::getStackSize() const { return stackSize_; }

	/*** FunctionEndInstrucion ***/

	FunctionEndInstruction::FunctionEndInstruction(
		c_string const &functionName, int stackSize) :
		Instruction(Instruction_Function_End), functionName_(functionName),
		stackSize_(stackSize) { }

	subcc::c_string const &
		FunctionEndInstruction::getFunctionName() const { return functionName_; }

	int FunctionEndInstruction::getStackSize() const { return stackSize_; }

	/*** LabelInstruction ***/

	LabelInstruction::LabelInstruction(int label) :
		Instruction(Instruction_Label), label_(label) { }

	int LabelInstruction::getLabel() const { return label_; }

	/*** AssignInstruction ***/

	AssignInstruction::AssignInstruction(enum InstructionType instType,
		Address const &target, Address const &source) :
		Instruction(instType), target_(target), source_(source) { }

	Address const &
		AssignInstruction::getSource() const { return source_; }

	Address const &
		AssignInstruction::getTarget() const { return target_; }

	/*** CopyInstruction ***/

	CopyInstruction::CopyInstruction(Address const &target, Address const &source) :
		AssignInstruction(Instruction_Assign_Copy, target, source) { }

	/*** UnaryAssignInstruction ***/

	UnaryAssignInstruction::UnaryAssignInstruction(Address const &target,
		enum UniOp op, Address const &source) :
		AssignInstruction(Instruction_Assign_Unary, target, source), operator_(op) { }

	enum UniOp
		UnaryAssignInstruction::getOperator() const { return operator_; }

	/*** BinaryAssignInstruction ***/

	BinaryAssignInstruction::BinaryAssignInstruction(Address const &target,
		Address const &srcA, enum BinOps op, Address const &srcB) :
		AssignInstruction(Instruction_Assign_Binary, target, srcA),
		srcB_(srcB), operator_(op) { }

	Address const &
		BinaryAssignInstruction::getSrcA() const { return source_; }

	Address const &
		BinaryAssignInstruction::getSrcB() const { return srcB_; }

	enum BinOps
		BinaryAssignInstruction::getOperator() const { return operator_; }

	Address const &
		BinaryAssignInstruction::getSource() const { return source_; }

	/*** FunctionCallInstruction ***/

	FunctionCallInstruction::FunctionCallInstruction(Address const &returnValue,
		Address const &function) :
		Instruction(Instruction_Function_Call),
		returnValue_(returnValue), function_(function) { }

	Address const &
		FunctionCallInstruction::getFunction() const { return function_; }

	Address const &
		FunctionCallInstruction::getReturnValue() const { return returnValue_; }

	/*** ReturnInstruction ***/

	ReturnInstruction::ReturnInstruction() :
		Instruction(Instruction_Return), returnValue_(Address()) { }

	ReturnInstruction::ReturnInstruction(Address const &returnValue) :
		Instruction(Instruction_Return), returnValue_(returnValue) { }

	Address const &
		ReturnInstruction::getReturnValue() const { return returnValue_; }

	/*** ParameterInstruction ***/

	ParameterInstruction::ParameterInstruction(Address const &parameter) :
		Instruction(Instruction_Parameter), parameter_(parameter) { }

	Address const &
		ParameterInstruction::getParameter() const { return parameter_; }

	/*** PopInstruction ***/

	PopInstruction::PopInstruction(int popSize) :
		Instruction(Instruction_Pop), popSize_(popSize) { }

	int PopInstruction::getPopSize() const { return popSize_; }

	/*** BranchInstruction ***/

	BranchInstruction::BranchInstruction(enum InstructionType type,
		int targetLabel) : Instruction(type), targetLabel_(targetLabel) {

		if (targetLabel_ < 0) { // error
			cout << "Branch target error\n";
			exit(0);
		}
	}

	int BranchInstruction::getTargetLabel() const { return targetLabel_; }

	/*** JumpInstruction ***/

	JumpInstruction::JumpInstruction(int targetLabel) :
		BranchInstruction(Instruction_Jump, targetLabel) { }

	/*** UnaryBranchInstruction ***/

	UnaryBranchInstruction::UnaryBranchInstruction(Address const &cond,
		int targetLabel) :
		BranchInstruction(Instruction_Branch_Unary, targetLabel), condition_(cond) { }

	Address const &
		UnaryBranchInstruction::getCondition() const { return condition_; }

	/*** UnaryNotBranchInstruction ***/

	UnaryNotBranchInstruction::UnaryNotBranchInstruction(Address const &cond,
		int targetLabel) :
		BranchInstruction(Instruction_Branch_Unary_Not, targetLabel),
		condition_(cond) { }

	Address const &
		UnaryNotBranchInstruction::getCondition() const { return condition_; }

	/*** BinaryBranchInstruction ***/

	BinaryBranchInstruction::BinaryBranchInstruction(Address const &condA,
		enum BinOps op, Address const &condB, int targetLabel) :
		BranchInstruction(Instruction_Branch_Binary, targetLabel),
		conditionA_(condA), conditionB_(condB), operator_(op) { }

	Address const &
		BinaryBranchInstruction::getConditionA() const { return conditionA_; }

	Address const &
		BinaryBranchInstruction::getConditionB() const { return conditionB_; }

	enum BinOps
		BinaryBranchInstruction::getOperator() const { return operator_; }

	/*** IndexedSrcCopyInstruction ***/

	IndexedSrcCopyInstruction::IndexedSrcCopyInstruction(
		Address const &target, Address const &source, Address const &index) :
		AssignInstruction(Instruction_Copy_Indexed_Src, target, source),
		index_(index) { }

	Address const &
		IndexedSrcCopyInstruction::getIndex() const { return index_; }

	/*** IndexedTargetCopyInstruction ***/

	IndexedTargetCopyInstruction::IndexedTargetCopyInstruction(
		Address const &target, Address const &index, Address const &source) :
		AssignInstruction(Instruction_Copy_Indexed_Target, target, source),
		index_(index) { }

	Address const &
		IndexedTargetCopyInstruction::getIndex() const { return index_; }


	/***************************** ThreeAdressCode  ****************************/

	ThreeAdressCode::ThreeAdressCode(shared_ptr<GlobalSymbolTable const> symbolTable) :
		symbolTable_(symbolTable) { }

	ThreeAdressCode::~ThreeAdressCode() {
		for (size_t i = 0; i < instructions_.size(); ++i) {
			delete instructions_[i];
		}
	}

	void ThreeAdressCode::pushInstruction(Instruction const *instruction) {
		instructions_.push_back(instruction);
	}

	void ThreeAdressCode::print() const {
		vector<Instruction const *>::const_iterator iter;
		for (iter = instructions_.begin(); iter != instructions_.end(); ++iter) {
			print(**iter);
		}
	}

	void ThreeAdressCode::print(Instruction const &instruction) const {
		switch (instruction.getType()) {
		case Instruction_Function_Start:
		{
			FunctionStartInstruction const &inst =
				dynamic_cast<FunctionStartInstruction const &> (instruction);
			cout << inst.getFunctionName();
			cout << ":" << endl;
			break;
		}
		case Instruction_Function_End:
		{
			FunctionEndInstruction const &inst =
				dynamic_cast<FunctionEndInstruction const &> (instruction);
			cout << "\n";
			break;
		}
		case Instruction_Label:
		{
			LabelInstruction const &inst =
				dynamic_cast<LabelInstruction const &> (instruction);
			cout << "L" << inst.getLabel() << ":" << endl;
			break;
		}
		case Instruction_Assign_Copy:
		{
			CopyInstruction const &inst =
				dynamic_cast<CopyInstruction const &> (instruction);
			cout << "  ";
			print(inst.getTarget());
			cout << "  =  ";
			print(inst.getSource());
			cout << endl;
			break;
		}
		case Instruction_Assign_Unary:
		{
			UnaryAssignInstruction const &inst =
				dynamic_cast<UnaryAssignInstruction const &> (instruction);
			cout << "  ";
			print(inst.getTarget());
			cout << "  =  ";
			print(inst.getOperator());
			cout << "  ";
			print(inst.getSource());
			cout << endl;
			break;
		}
		case Instruction_Assign_Binary:
		{
			BinaryAssignInstruction const &inst =
				dynamic_cast<BinaryAssignInstruction const &> (instruction);
			cout << "  ";
			print(inst.getTarget());
			cout << "  =  ";
			print(inst.getSrcA());
			cout << "  ";
			print(inst.getOperator());
			cout << "  ";
			print(inst.getSrcB());
			cout << endl;
			break;
		}
		case Instruction_Function_Call:
		{
			FunctionCallInstruction const &inst =
				dynamic_cast<FunctionCallInstruction const &> (instruction);
			cout << "  ";
			print(inst.getReturnValue());
			cout << "  =  Call  ";
			print(inst.getFunction());
			cout << endl;
			break;
		}
		case Instruction_Return:
		{
			ReturnInstruction const &inst =
				dynamic_cast<ReturnInstruction const &> (instruction);
			cout << "  Return  ";
			print(inst.getReturnValue());
			cout << endl;
			break;
		}
		case Instruction_Parameter:
		{
			ParameterInstruction const &inst =
				dynamic_cast<ParameterInstruction const &> (instruction);
			cout << "  Param  ";
			print(inst.getParameter());
			cout << endl;
			break;
		}
		case Instruction_Jump:
		{
			JumpInstruction const &inst =
				dynamic_cast<JumpInstruction const &> (instruction);
			cout << "  Goto  L" << inst.getTargetLabel() << endl;
			break;
		}
		case Instruction_Branch_Unary:
		{
			UnaryBranchInstruction const &inst =
				dynamic_cast<UnaryBranchInstruction const &> (instruction);
			cout << "  If ( ";
			print(inst.getCondition());
			cout << " )  Goto  L" << inst.getTargetLabel() << endl;
			break;
		}
		case Instruction_Branch_Unary_Not:
		{
			UnaryNotBranchInstruction const &inst =
				dynamic_cast<UnaryNotBranchInstruction const &> (instruction);
			cout << "  IfFalse ( ";
			print(inst.getCondition());
			cout << " )  Goto  L" << inst.getTargetLabel() << endl;
			break;
		}
		case Instruction_Branch_Binary:
		{
			BinaryBranchInstruction const &inst =
				dynamic_cast<BinaryBranchInstruction const &> (instruction);
			cout << "  If ( ";
			print(inst.getConditionA());
			cout << " ";
			print(inst.getOperator());
			cout << " ";
			print(inst.getConditionB());
			cout << " )  Goto  " << inst.getTargetLabel() << endl;
			break;
		}
		case Instruction_Copy_Indexed_Src:
		{
			IndexedSrcCopyInstruction const &inst =
				dynamic_cast<IndexedSrcCopyInstruction const &> (instruction);
			cout << "  ";
			print(inst.getTarget());
			cout << "  =  ";
			print(inst.getSource());
			cout << " [ ";
			print(inst.getIndex());
			cout << " ] " << endl;
			break;
		}
		case Instruction_Copy_Indexed_Target:
		{
			IndexedTargetCopyInstruction const &inst =
				dynamic_cast<IndexedTargetCopyInstruction const &> (instruction);
			cout << "  ";
			print(inst.getTarget());
			cout << " [ ";
			print(inst.getIndex());
			cout << " ]  =  ";
			print(inst.getSource());
			cout << endl;
			break;
		}
		}
	}

	void ThreeAdressCode::print(Address const &address) const {
		if (address.isNull()) {
			cout << "(  )";
			return;
		}
		int index = address.getSymbolTableIndex();
		c_string const &lexeme = symbolTable_->lookUp(index)->getLexeme();
		cout << lexeme;
	}

	void ThreeAdressCode::print(enum UniOp op) const {
		switch (op) {
		case UniOp_Arith_Minus:
			cout << "-";
			break;
		case UniOp_Arith_Inc:
			cout << "++";
			break;
		case UniOp_Arith_Dec:
			cout << "--";
			break;
		case UniOp_Logic_Not:
			cout << "!";
			break;
		case UniOp_Logic_Bit_Not:
			cout << "~";
			break;
		}
	}

	void ThreeAdressCode::print(enum BinOps op) const {
		switch (op) {
		case BinOp_Arith_Plus:
			cout << "+";
			break;
		case BinOp_Arith_Minus:
			cout << "-";
			break;
		case BinOp_Arith_Multi:
			cout << "*";
			break;
		case BinOp_Arith_Divide:
			cout << "/";
			break;
		case BinOp_Arith_Mod:
			cout << "%";
			break;
		case BinOp_Logic_And:
			cout << "&&";
			break;
		case BinOp_Logic_Or:
			cout << "||";
			break;
		case BinOp_Logic_Bit_And:
			cout << "&";
			break;
		case BinOp_Logic_Bit_Or:
			cout << "|";
			break;
		case BinOp_Logic_Bit_Xor:
			cout << "^";
			break;
		case BinOp_Rel_EQ:
			cout << "==";
			break;
		case BinOp_Rel_NE:
			cout << "!=";
			break;
		case BinOp_Rel_GT:
			cout << ">";
			break;
		case BinOp_Rel_GE:
			cout << ">=";
			break;
		case BinOp_Rel_LT:
			cout << "<";
			break;
		case BinOp_Rel_LE:
			cout << "<=";
			break;
		}
	}

	/***************************** CodeGenerator ********************************/

	ThreeAdressCodeGenerator::ThreeAdressCodeGenerator(shared_ptr<SyntaxTree const> syntaxTree) :
		root_(syntaxTree->getRoot()), symbolTable_(syntaxTree->getSymbolTable()),
		code_(shared_ptr<ThreeAdressCode>(new ThreeAdressCode(symbolTable_))), curLabel_(1)
	{
		//std::cout << "start\n";
		generateCode();
		//std::cout << "end\n";
	}

	void ThreeAdressCodeGenerator::generateCode() {
		NonTerminalNode const *root = dynamic_cast<NonTerminalNode const *> (root_);
		for (int i = 0; i < root->getNumChildren(); ++i) {
			// Translate each function definitions
			NonTerminalNode const *decl =
				dynamic_cast<NonTerminalNode const *> (root->getChild(i));
			if (decl->getProductionKey() == 82)  // function definition
				translateFunction(dynamic_cast<FunctionNode const *>(decl));
		}
	}

	void ThreeAdressCodeGenerator::translateFunction(FunctionNode const *funcDef) {
		// Print function name (label)
		// FuncDecl -> FuncDefHead CompoundStmt
		// FuncDefHead -> Typespec ID Params
		NonTerminalNode const *tempNode =
			dynamic_cast<NonTerminalNode const *>(funcDef->getChild(0));
		IdNode const *funcName = dynamic_cast<IdNode const *> (tempNode->getChild(1));
		int stackSize = funcDef->getMaxStackSize();
		pushInstruction(
			new FunctionStartInstruction(funcName->getLexeme(), stackSize));
		// Translate function body
		StmtNode const *funcBody =
			dynamic_cast<StmtNode const *>(funcDef->getChild(1));
		int nextLabel = getNewLabel();
		translateStatement(funcBody, nextLabel);
		pushInstruction(new LabelInstruction(nextLabel));
		pushInstruction(new FunctionEndInstruction(funcName->getLexeme(), stackSize));
	}

	void ThreeAdressCodeGenerator::translateStatement(StmtNode const *node, int nextLabel) {
		translateStatement(node, nextLabel, errorLabel, errorLabel);
	}

	void ThreeAdressCodeGenerator::translateStatement(StmtNode const *node, int nextLabel,
		int breakLabel, int continueLabel) {

		switch (node->getProductionKey()) {
		case 101: // CompoundStmt
		{ // just postpone to stmtListNode
			StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(1));
			translateStatement(stmtNode, nextLabel, breakLabel, continueLabel);
			break;
		}
		case 122: // StmtList
		{ // just work through all children
			int numStmt = node->getNumChildren();
			for (int i = 0; i < numStmt; ++i) {
				StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(i));
				if (i < numStmt - 1) { // not last statement
					int tempLabel = getNewLabel();
					translateStatement(stmtNode, tempLabel, breakLabel, continueLabel);
					pushInstruction(new LabelInstruction(tempLabel));
				}
				else // last statement
					translateStatement(stmtNode, nextLabel, breakLabel, continueLabel);
			}
			break;
		}
		case 123: // Stmt -> Expr ;
		{
			ExprNode *exprNode = dynamic_cast<ExprNode *>(node->getChild(0));
			// We do not need to know the value of the entire statement like
			// ..x = f(v1, v2) or x = y * z
			translateExpression(exprNode, false);
			break;
		}
		case 141: // IfStmt -> Expr , Stmt , ElseStmt
		{
			ExprNode *exprNode = dynamic_cast<ExprNode *>(node->getChild(0));
			StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(1));
			StmtNode *elseStmtNode = dynamic_cast<StmtNode *>(node->getChild(2));
			if (elseStmtNode->getProductionKey() == 142) { // else is empty
														   // Condition (true -> L1, false -> End)
														   // L1
														   // Stmt
				int L1 = getNewLabel();
				translateExpression(exprNode, L1, nextLabel);
				pushInstruction(new LabelInstruction(L1));
				translateStatement(stmtNode, nextLabel, breakLabel, continueLabel);
			}
			else {
				// else has some statements
				// Condition (true -> L1, false -> L3)
				// L1
				// Stmt
				// L2
				// Goto next
				// L3
				// ElseStmt
				int L1 = getNewLabel();
				int L2 = getNewLabel();
				int L3 = getNewLabel();
				translateExpression(exprNode, L1, L3);
				pushInstruction(new LabelInstruction(L1));
				translateStatement(stmtNode, L2, breakLabel, continueLabel);
				pushInstruction(new LabelInstruction(L2));
				pushInstruction(new JumpInstruction(nextLabel));
				pushInstruction(new LabelInstruction(L3));
				translateStatement(elseStmtNode, nextLabel, breakLabel, continueLabel);
			}
			break;
		}
		case 151: // WhileStmt -> Expr , Stmt
		{
			ExprNode *exprNode = dynamic_cast<ExprNode *>(node->getChild(0));
			StmtNode *stmtNode = dynamic_cast<StmtNode *>(node->getChild(1));
			// L1
			// Condition (true -> L2, false -> End)
			// L2
			// Stmt
			// L3
			// goto L1
			int L1 = getNewLabel();
			int L2 = getNewLabel();
			int L3 = getNewLabel();
			pushInstruction(new LabelInstruction(L1));
			translateExpression(exprNode, L2, nextLabel);
			pushInstruction(new LabelInstruction(L2));
			translateStatement(stmtNode, L3, nextLabel, L1);
			pushInstruction(new LabelInstruction(L3));
			pushInstruction(new JumpInstruction(L1));
			break;
		}
		case 161: // Stmt -> BREAK
		{
			pushInstruction(new JumpInstruction(breakLabel));
			break;
		}
		case 162: // Stmt -> CONTINUE
		{
			pushInstruction(new JumpInstruction(continueLabel));
			break;
		}
		case 171: // SwitchStmt -> Expr , SwitchCaseList , SwitchDefaultCase
		{
			// Switch statement catches only 'break', and pass 'continue'
			ExprNode *exprNode = dynamic_cast<ExprNode *>(node->getChild(0));
			StmtNode *caseList = dynamic_cast<StmtNode *>(node->getChild(1));
			StmtNode *defaultStmt = dynamic_cast<StmtNode *>(node->getChild(2));
			int caseNum = caseList->getNumChildren();
			vector<int> labels(caseNum); // L1, L2, .... Lk
			for (int i = 0; i < caseNum; ++i)
				labels[i] = getNewLabel();
			int defaultLabel = getNewLabel();

			// Expr
			// expr[0]
			// if (expr[0] == Expr) goto L[0]
			// expr[1]
			// if (expr[1] == Expr) goto L[1]
			// ...
			// goto L_default
			//
			// L[0]
			// StmtList[0]
			// L[1]
			// StmtList[1]
			// ...
			// L_default
			// StmtListDefault			

			// Get address of Expr
			Address exprAddr(getAddress(exprNode->getRvalueIndex()));
			translateExpression(exprNode, true);
			for (int i = 0; i < caseNum; ++i) {
				ExprNode *caseExpr = dynamic_cast<ExprNode *>(
					dynamic_cast<StmtNode *>(caseList->getChild(i))->getChild(0));
				translateExpression(caseExpr, true);
				// Get address of expr_k
				Address caseExprAddr(getAddress(caseExpr->getRvalueIndex()));
				pushInstruction(new BinaryBranchInstruction(
					exprAddr, BinOp_Rel_EQ, caseExprAddr, labels[i]));
			}
			pushInstruction(new JumpInstruction(defaultLabel));
			for (int i = 0; i < caseNum; ++i) {
				pushInstruction(new LabelInstruction(labels[i]));
				StmtNode *caseStmt = dynamic_cast<StmtNode *>(
					dynamic_cast<StmtNode *>(caseList->getChild(i))->getChild(1));
				int endLabel = (i == caseNum - 1) ? defaultLabel : labels[i + 1];
				translateStatement(caseStmt, endLabel, nextLabel, continueLabel);
			}
			pushInstruction(new LabelInstruction(defaultLabel));
			translateStatement(defaultStmt, nextLabel, nextLabel, continueLabel);
			break;
		}
		case 181: // Stmt -> return (void)
		{
			pushInstruction(new ReturnInstruction());
			break;
		}
		case 182: // Stmt -> return Expr
		{
			ExprNode *exprNode = dynamic_cast<ExprNode *>(node->getChild(0));
			Address returnValue(getAddress(exprNode->getRvalueIndex()));
			translateExpression(exprNode, true);
			pushInstruction(new ReturnInstruction(returnValue));
			break;
		}
		default:
		{
			cout << "translateStatement default\n";
			exit(0);
		}
		}
	}

	// Flow control with boolean expression
	void ThreeAdressCodeGenerator::translateExpression(ExprNode const *node,
		int trueNext, int falseNext) {
		// Recursive call only for && , || , !
		// Other constructs reduces to value translation for that expression
		int productionKey = node->getProductionKey();
		switch (productionKey) {
		case 226: // !
		{ // expr (true -> falseNext, false -> trueNext)
			ExprNode *expr = dynamic_cast<ExprNode *> (node->getChild(0));
			translateExpression(expr, falseNext, trueNext);
			break;
		}
		case 295: // ||
		{ // leftExpr (true -> trueNext, false -> L1)
		  // L1
		  // rightExpr (true -> trueNext, false -> falseNext)
			ExprNode *leftExpr = dynamic_cast<ExprNode *> (node->getChild(0));
			ExprNode *rightExpr = dynamic_cast<ExprNode *> (node->getChild(1));
			int L1 = getNewLabel();
			translateExpression(leftExpr, trueNext, L1);
			pushInstruction(new LabelInstruction(L1));
			translateExpression(rightExpr, trueNext, falseNext);
			break;
		}
		case 296: // &&
		{ // leftExpr (true -> L1, false -> falseNext)
		  // L1
		  // rightExpr (true -> trueNext, false -> falseNext)
			ExprNode *leftExpr = dynamic_cast<ExprNode *> (node->getChild(0));
			ExprNode *rightExpr = dynamic_cast<ExprNode *> (node->getChild(1));
			int L1 = getNewLabel();
			translateExpression(leftExpr, L1, falseNext);
			pushInstruction(new LabelInstruction(L1));
			translateExpression(rightExpr, trueNext, falseNext);
			break;
		}
		default: // all other constructs
		{ // expr (save r-value to x) 
		  // if (x) goto trueNext
		  // goto falseNext
			translateExpression(node, true);
			Address condition(getAddress(node->getRvalueIndex()));
			// Manage constant condition: transform to Jump (instead of branch)
			if (condition.getType() == AddressType_Constant) {
				char cond = dynamic_pointer_cast<Constant const>
					(symbolTable_->lookUp(node->getRvalueIndex()))->getCharValue();
				if (cond)
					pushInstruction(new JumpInstruction(trueNext));
				else
					pushInstruction(new JumpInstruction(falseNext));
			}
			else {
				pushInstruction(new UnaryBranchInstruction(condition, trueNext));
				pushInstruction(new JumpInstruction(falseNext));
			}
			break;
		}
		}
	}

	// Getting value of the expression
	void ThreeAdressCodeGenerator::translateExpression(ExprNode const *node, bool saveRvalue) {
		int productionKey = node->getProductionKey();
		switch (productionKey) {
		case 191: // unaryExpr = expr
		{
			LvalueNode *lhs = dynamic_cast<LvalueNode *> (node->getChild(0));
			ExprNode *rhs = dynamic_cast<ExprNode *> (node->getChild(1));
			translateExpression(lhs, false);
			translateExpression(rhs, true);
			Address rvalue(getAddress(rhs->getRvalueIndex()));
			Address base(getAddress(lhs->getBaseIndex()));
			Address index(getAddress(lhs->getIndexIndex()));
			if (index.isNull()) // lhs is an single variable (without [] or .)
				pushInstruction(new CopyInstruction(base, rvalue));
			else
				pushInstruction(
					new IndexedTargetCopyInstruction(base, index, rvalue));
			break;
		}
		case 192: // +=
		case 193: // -=
		case 194:
		case 195:
		case 196:
		case 197:
		case 198:
		case 199:
		{ // Above were already converted to binary op + assignment
			cout << "translateExpression: invalid key\n";
			exit(0);
		}
		case 222: // + (unary)
		case 223: // - (unary)
		case 226: // ! (unary)
		case 227: // ~ (unary)
		{
			ExprNode *expr = dynamic_cast<ExprNode *>(node->getChild(0));
			translateExpression(expr, true);
			Address rvalue(getAddress(expr->getRvalueIndex()));
			Address result(getAddress(node->getRvalueIndex()));
			enum UniOp op;
			switch (productionKey) {
			case 222:
				pushInstruction(new CopyInstruction(result, rvalue));
				break;
			case 223:
				op = UniOp_Arith_Minus;
				break;
			case 226:
				op = UniOp_Logic_Not;
				break;
			case 227:
				op = UniOp_Logic_Bit_Not;
				break;
			default:
				cout << "translateExpression - uniop error\n";
				exit(0);
			}
			if (productionKey != 222)
				pushInstruction(new UnaryAssignInstruction(result, op, rvalue));
			break;
		}
		case 224: // ++ (unary)
		case 225: // -- (unary)
		{ // ++expr
			LvalueNode *expr = dynamic_cast<LvalueNode *>(node->getChild(0));
			translateExpression(expr, true);
			Address rvalue(getAddress(expr->getRvalueIndex()));
			Address base(getAddress(expr->getBaseIndex()));
			Address index(getAddress(expr->getIndexIndex()));
			Address result(getAddress(node->getRvalueIndex()));
			int oneIndex = std::const_pointer_cast<GlobalSymbolTable>(symbolTable_)->insert(
				shared_ptr<Symbol>(new Constant(1)));
			enum BinOps op;
			if (productionKey == 224)
				op = BinOp_Arith_Plus;
			else
				op = BinOp_Arith_Minus;
			pushInstruction(new BinaryAssignInstruction(
				result, rvalue, op, getAddress(oneIndex)));
			if (index.isNull()) // lhs is an single variable (without [] or .)
				pushInstruction(new CopyInstruction(base, result));
			else
				pushInstruction(
					new IndexedTargetCopyInstruction(base, index, result));
			break;
		}
		case 231: // sizeof ID
		case 232: // sizeof typeSpec
		case 233: // Constant
		{ // Above were done in parsing 
			break;
		}
		case 242: // lvalue [ expr ]
		{
			LvalueNode const *resultNode = dynamic_cast<LvalueNode const *> (node);
			LvalueNode *lvalueNode = dynamic_cast<LvalueNode *> (node->getChild(0));
			ExprNode *exprNode = dynamic_cast<ExprNode *> (node->getChild(1));
			int typeWidth = dynamic_pointer_cast<ArrayType const>(
				lvalueNode->getType())->getElementType()->getWidth();
			int widthIndex = std::const_pointer_cast<GlobalSymbolTable>(symbolTable_)->insert(
				shared_ptr<Symbol>(new Constant(typeWidth)));
			Address base(getAddress(lvalueNode->getBaseIndex()));
			Address oldIndex(getAddress(lvalueNode->getIndexIndex()));
			Address width(getAddress(widthIndex));
			Address rvalue(getAddress(exprNode->getRvalueIndex()));
			Address temp(getAddress(resultNode->getTempIndex()));
			Address newIndex(getAddress(resultNode->getIndexIndex()));
			Address newRvalue(getAddress(resultNode->getRvalueIndex()));
			// lvalue
			// expr
			// temp = lvalue.typeWidth * expr 
			// newIndex = oldIndex + temp
			translateExpression(lvalueNode, false);
			translateExpression(exprNode, true);
			pushInstruction(new BinaryAssignInstruction(
				temp, width, BinOp_Arith_Multi, rvalue));
			if (oldIndex.isNull()) { // lvalue is not indexed (a single Name)
				pushInstruction(new CopyInstruction(newIndex, temp));
			}
			else { // lvalue is indexed
				pushInstruction(new BinaryAssignInstruction(
					newIndex, oldIndex, BinOp_Arith_Plus, temp));
			}
			if (saveRvalue) { // If rvalue is needed, do an indexed copy
				pushInstruction(new IndexedSrcCopyInstruction(
					newRvalue, base, newIndex));
			}
			break;
		}
		case 243: // expr . ID
		{
			LvalueNode const *resultNode = dynamic_cast<LvalueNode const *> (node);
			LvalueNode *lvalueNode = dynamic_cast<LvalueNode *> (node->getChild(0));
			IdNode *idNode = dynamic_cast<IdNode *> (node->getChild(1));
			// Get offset of the field named ID
			c_string const &field = idNode->getLexeme();
			int fieldIndex = dynamic_pointer_cast<RecordType const>(
				lvalueNode->getType())->getIndexOf(field);
			int recordOffset = symbolTable_->lookUp(fieldIndex)->getOffset();
			int offsetIndex = std::const_pointer_cast<GlobalSymbolTable>(symbolTable_)->insert(
				shared_ptr<Symbol>(new Constant(recordOffset)));
			// Setting three-addr addresses
			Address base(getAddress(lvalueNode->getBaseIndex()));
			Address oldIndex(getAddress(lvalueNode->getIndexIndex()));
			Address offset(getAddress(offsetIndex));
			Address newIndex(getAddress(resultNode->getIndexIndex()));
			Address newRvalue(getAddress(resultNode->getRvalueIndex()));
			// Do translation
			translateExpression(lvalueNode, false);
			if (oldIndex.isNull()) // lvalue is not indexed (a single Name)
				pushInstruction(new CopyInstruction(newIndex, offset));
			else // lvalue is indexed
				pushInstruction(new BinaryAssignInstruction(
					newIndex, oldIndex, BinOp_Arith_Plus, offset));
			if (saveRvalue) // If rvalue is needed, do an indexed copy
				pushInstruction(new IndexedSrcCopyInstruction(
					newRvalue, base, newIndex));
			break;
		}
		case 244: // expr ( argumentList )
		{
			ExprNode const *resultNode = dynamic_cast<ExprNode const *> (node);
			LvalueNode *funcNode = dynamic_cast<LvalueNode *> (node->getChild(0));
			ArgumentListNode *argListNode =
				dynamic_cast<ArgumentListNode *>(node->getChild(1));
			int argsNum = argListNode->getNumChildren();
			vector<int> args(argsNum); // symtab index for r-values of each arg
			int argsWidth = 0; // total widths of arguments
							   // Translate function: 
							   // :Tricky: 
							   // In real implementation, we will need r-value of function expression,
							   // ..because that will the the address to jump. Here, we simply assume
							   // ..that every function call is a single identifier (not via function 
							   // ..pointer nor record access), so it suffices to get l-value
			translateExpression(funcNode, false);
			// Translate each argument
			for (int i = 0; i < argsNum; ++i) {
				ExprNode *argExpr = dynamic_cast<ExprNode *>(argListNode->getChild(i));
				translateExpression(argExpr, true);
				args[i] = argExpr->getRvalueIndex();
				shared_ptr<Symbol const> arg = symbolTable_->lookUp(argExpr->getRvalueIndex());
				int width = ((arg->getType()->getWidth() - 1) / 4 + 1) * 4;
				argsWidth += width;
			}
			// Push arguments: in reverse order!!!
			for (int i = argsNum - 1; i >= 0; i--) {
				Address paramAddr(getAddress(args[i]));
				pushInstruction(new ParameterInstruction(paramAddr));
			}
			// Call
			Address funcAddress(getAddress(funcNode->getBaseIndex()));
			if (saveRvalue) {
				Address rvalue(getAddress(node->getRvalueIndex()));
				pushInstruction(new FunctionCallInstruction(rvalue, funcAddress));
			}
			else { // return value is thrown away
				pushInstruction(new FunctionCallInstruction(Address(), funcAddress));
			}
			// Pop arguments: need to know the total width or formal parameters
			pushInstruction(new PopInstruction(argsWidth));
			break;
		}
		case 281: // + (binary)
		case 282: // - (binary)
		case 283: // * (binary)
		case 284: // / (binary)
		case 285: // % (binary)
		case 286: // & (binary)
		case 287: // | (binary)
		case 288: // ^ (binary)
		case 289: // > (binary)
		case 290: // >= (binary)
		case 291: // < (binary)
		case 292: // <= (binary)
		case 293: // == (binary)
		case 294: // != (binary)
		case 295: // || (binary)
		case 296: // && (binary)
		{
			ExprNode *leftExpr = dynamic_cast<ExprNode *>(node->getChild(0));
			ExprNode *rightExpr = dynamic_cast<ExprNode *>(node->getChild(1));
			translateExpression(leftExpr, true);
			translateExpression(rightExpr, true);
			Address leftValue(getAddress(leftExpr->getRvalueIndex()));
			Address rightValue(getAddress(rightExpr->getRvalueIndex()));
			Address resultValue(getAddress(node->getRvalueIndex()));
			enum BinOps op;
			switch (productionKey) {
			case 281:
				op = BinOp_Arith_Plus;
				break;
			case 282:
				op = BinOp_Arith_Minus;
				break;
			case 283:
				op = BinOp_Arith_Multi;
				break;
			case 284:
				op = BinOp_Arith_Divide;
				break;
			case 285:
				op = BinOp_Arith_Mod;
				break;
			case 286:
				op = BinOp_Logic_Bit_And;
				break;
			case 287:
				op = BinOp_Logic_Bit_Or;
				break;
			case 288:
				op = BinOp_Logic_Bit_Xor;
				break;
			case 289:
				op = BinOp_Rel_GT;
				break;
			case 290:
				op = BinOp_Rel_GE;
				break;
			case 291:
				op = BinOp_Rel_LT;
				break;
			case 292:
				op = BinOp_Rel_LE;
				break;
			case 293:
				op = BinOp_Rel_EQ;
				break;
			case 294:
				op = BinOp_Rel_NE;
				break;
			case 295:
				op = BinOp_Logic_Or;
				break;
			case 296:
				op = BinOp_Logic_And;
				break;
			}
			pushInstruction(new BinaryAssignInstruction(
				resultValue, leftValue, op, rightValue));
			break;
		}
		default:
		{
			break;
		}
		}
	}

	void ThreeAdressCodeGenerator::pushInstruction(Instruction const *instruction) {
		code_->pushInstruction(instruction);
	}

	int ThreeAdressCodeGenerator::getNewLabel() {
		return curLabel_++;
	}

	Address
		ThreeAdressCodeGenerator::getAddress(int symtabIndex) const {
		if (symtabIndex < 0)  // return null address
			return Address();
		shared_ptr<Symbol const> symbol = symbolTable_->lookUp(symtabIndex);
		enum AddressType addressType;
		if (symbol->isTemporary()) {
			addressType = AddressType_Temporary;
		}
		else if (symbol->isVariable()) {
			addressType = AddressType_Name;
		}
		else if (symbol->isConstant()) {
			addressType = AddressType_Constant;
		}
		else if (symbol->getSymbolKind() == SYMBOL_FUNC) {
			addressType = AddressType_Name;
		}
		else {
			cout << "ThreeAdressCodeGenerator::getAddress() error\n";
			exit(0);
		}
		return Address(addressType, symtabIndex);
	}
} // end namespace subcc
