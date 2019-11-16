#include <iostream>
#include <memory>
#include "code_generator.h"

namespace subcc
{
	using std::cout;
	using std::shared_ptr;
	using std::dynamic_pointer_cast;

	// Printing a C code with some constraints 
	void CCodeGenerator::printCCode() const {
		// For other declarations
		cout << "#include <stdio.h> \n";
		cout << "#include \"subcc_interpreter.h\"\n\n";
		cout << "namespace subcc_interpreter\n{\n";
		// Data segment (Stack segment is declared in subcc_interpreter.h)
		cout << "char DATA[1024];\n";
		cout << "char STRING_SEGMENT[1024] =\"" << symbolTable_->getStringSegment() << "\";\n";
		// General registers
		cout << "int GPR0;\n";
		cout << "int GPR1;\n";
		cout << "int GPR2;\n";
		cout << "int const ZERO = 0;\n";
		// Big register for return value and record copy
		cout << "char M0[256];\n\n";

		for (auto iter = threeAddrCode_->getInstructions()->begin();
			iter != threeAddrCode_->getInstructions()->end(); ++iter) {
			printCCode(**iter);
		}
		cout << "\n}"; // end namespace
	}

	void CCodeGenerator::printCCode(Instruction const &instruction) const {
		switch (instruction.getType()) {
		case Instruction_Function_Start:
		{
			FunctionStartInstruction const &inst =
				dynamic_cast<FunctionStartInstruction const &>(instruction);
			int stackSize = inst.getStackSize();
			cout << "void ";
			if (inst.getFunctionName() == "main") {
				cout << "_";
			}
			cout << inst.getFunctionName();
			cout << "() {\n\n";
			// Calle's stack manipulation
			cout << "\tSTORE ( FP, STACK, SP - 4, 4);\n";
			cout << "\tSP = SP - 4;\n";
			cout << "\tFP = SP;\n";
			cout << "\tSP = SP - " << stackSize << ";\n";
			break;
		}
		case Instruction_Function_End:
		{
			FunctionEndInstruction const &inst =
				dynamic_cast<FunctionEndInstruction const &>(instruction);
			// Calle's stack manipulation
			cout << "\tSP = FP;\n";
			cout << "\tLOAD ( FP, STACK, SP, 4);\n";
			cout << "\tSP = SP + 4;\n";
			cout << "\tRETURN;\n";
			cout << "} // end of( ";
			cout << inst.getFunctionName();
			cout << " )\n\n";
			break;
		}
		case Instruction_Label:
		{
			threeAddrCode_->print(instruction);
			break;
		}
		case Instruction_Assign_Binary:
		{
			BinaryAssignInstruction const &inst =
				dynamic_cast<BinaryAssignInstruction const &> (instruction);
			Address const &target = inst.getTarget();
			Address const &srcA = inst.getSrcA();
			enum BinOps op = inst.getOperator();
			Address const &srcB = inst.getSrcB();
			printCCode(LOAD, GPR0, srcA);
			printCCode(LOAD, GPR1, srcB);
			cout << "\t";
			printCCode(GPR2);
			cout << " = ";
			printCCode(GPR0);
			threeAddrCode_->print(op);
			printCCode(GPR1);
			cout << ";\n";
			printCCode(STORE, GPR2, target);
			break;
		}
		case Instruction_Assign_Unary:
		{
			UnaryAssignInstruction const &inst =
				dynamic_cast<UnaryAssignInstruction const &> (instruction);
			Address const &target = inst.getTarget();
			Address const &source = inst.getSource();
			enum UniOp op = inst.getOperator();
			printCCode(LOAD, GPR0, source);
			cout << "\t";
			printCCode(GPR1);
			cout << " = ";
			threeAddrCode_->print(op);
			printCCode(GPR0);
			cout << ";\n";
			printCCode(STORE, GPR1, target);
			break;
		}
		case Instruction_Assign_Copy:
		{
			CopyInstruction const &inst =
				dynamic_cast<CopyInstruction const &> (instruction);
			Address const &target = inst.getTarget();
			Address const &source = inst.getSource();
			shared_ptr<Symbol const> sourceSymbol =
				symbolTable_->lookUp(source.getSymbolTableIndex());
			int width = sourceSymbol->getType()->getWidth();
			printCCode(LOAD, M0, source, width);
			/*
			cout << "\t";
			printCCode(GPR1);
			cout << " = ";
			printCCode(GPR0);
			cout << ";\n";
			*/
			printCCode(STORE, M0, target, width);
			break;
		}
		case Instruction_Function_Call:
		{
			FunctionCallInstruction const &inst =
				dynamic_cast<FunctionCallInstruction const &> (instruction);
			Address const &function = inst.getFunction();
			Address const &returnValue = inst.getReturnValue();
			cout << "\tCALL( ";
			threeAddrCode_->print(function);
			cout << ");\n";
			int tempIndex = function.getSymbolTableIndex();
			shared_ptr<Symbol const> funcSymbol = symbolTable_->lookUp(tempIndex);
			shared_ptr<Type const> returnType =
				dynamic_pointer_cast<FuncType const>(funcSymbol->getType())
				->getCodomainType();
			int returnValueWidth = returnType->getWidth();
			if (!returnValue.isNull()) { // need to retrieve return value
										 // After returning, the return value is in M0
				printCCode(STORE, M0, returnValue, returnValueWidth);
			}
			break;
		}
		case Instruction_Return:
		{
			ReturnInstruction const &inst =
				dynamic_cast<ReturnInstruction const &> (instruction);
			Address const &returnValue = inst.getReturnValue();
			int tempIndex = returnValue.getSymbolTableIndex();
			shared_ptr<Symbol const> symbol = symbolTable_->lookUp(tempIndex);
			int returnValueWidth = symbol->getType()->getWidth();
			// Copy return value to M0
			printCCode(LOAD, M0, returnValue, returnValueWidth);
			// Calle's stack manipulation
			cout << "\tSP = FP;\n";
			cout << "\tLOAD ( FP, STACK, SP, 4);\n";
			cout << "\tSP = SP + 4;\n";
			cout << "\tRETURN;\n";
			break;
		}
		case Instruction_Parameter:
		{
			ParameterInstruction const &inst =
				dynamic_cast<ParameterInstruction const &> (instruction);
			Address const &parameter = inst.getParameter();
			int tempIndex = parameter.getSymbolTableIndex();
			shared_ptr<Symbol const> symbol = symbolTable_->lookUp(tempIndex);
			int parameterWidth = symbol->getType()->getWidth();
			int width = ((parameterWidth - 1) / 4 + 1) * 4;
			printCCode(LOAD, M0, parameter, width);
			cout << "\tSTORE ( M0, STACK, SP - " << width <<
				" ," << width << " );\n";
			cout << "\tSP = SP - " << width << ";\n";
			break;
		}
		case Instruction_Pop:
		{ // Pop arguments after returning from a function
			PopInstruction const &inst =
				dynamic_cast<PopInstruction const &> (instruction);
			int popSize = inst.getPopSize();
			cout << "\tSP = SP + " << popSize << " ;\n";
			break;
		}
		case Instruction_Jump:
		{
			JumpInstruction const &inst =
				dynamic_cast<JumpInstruction const &> (instruction);
			int targetLabel = inst.getTargetLabel();
			cout << "\tGOTO (L" << targetLabel << ");\n";
			break;
		}
		case Instruction_Branch_Unary:
		{
			UnaryBranchInstruction const &inst =
				dynamic_cast<UnaryBranchInstruction const &> (instruction);
			Address const &cond = inst.getCondition();
			int targetLabel = inst.getTargetLabel();
			printCCode(LOAD, GPR0, cond);
			cout << "\tIF (";
			printCCode(GPR0);
			cout << ", L" << targetLabel << " );\n";
			break;
		}
		case Instruction_Branch_Unary_Not:
		{
			UnaryNotBranchInstruction const &inst =
				dynamic_cast<UnaryNotBranchInstruction const &> (instruction);
			Address const &cond = inst.getCondition();
			int targetLabel = inst.getTargetLabel();
			printCCode(LOAD, GPR0, cond);
			cout << "\tIF_FALSE (";
			printCCode(GPR0);
			cout << ", L" << targetLabel << " );\n";
			break;
		}
		case Instruction_Branch_Binary:
		{
			BinaryBranchInstruction const &inst =
				dynamic_cast<BinaryBranchInstruction const &> (instruction);
			Address const &condA = inst.getConditionA();
			Address const &condB = inst.getConditionB();
			int targetLabel = inst.getTargetLabel();
			enum BinOps op = inst.getOperator();
			printCCode(LOAD, GPR0, condA);
			printCCode(LOAD, GPR1, condB);
			cout << "\tIF (";
			printCCode(GPR0);
			threeAddrCode_->print(op);
			printCCode(GPR1);
			cout << ", L" << targetLabel << " );\n";
			break;
		}
		case Instruction_Copy_Indexed_Src:
		{
			IndexedSrcCopyInstruction const &inst =
				dynamic_cast<IndexedSrcCopyInstruction const &> (instruction);
			Address const &target = inst.getTarget();
			Address const &source = inst.getSource();
			Address const &index = inst.getIndex();
			// target = source[index]
			// First load index's r-value into a register, and LOAD with index
			printCCode(LOAD, GPR1, index);
			printCCode(LOAD, GPR0, source, GPR1);
			printCCode(STORE, GPR0, target);
			break;
		}
		case Instruction_Copy_Indexed_Target:
		{
			IndexedTargetCopyInstruction const &inst =
				dynamic_cast<IndexedTargetCopyInstruction const &> (instruction);
			Address const &target = inst.getTarget();
			Address const &index = inst.getIndex();
			Address const &source = inst.getSource();
			// target[index] = source
			// First load index's r-value into a register, and LOAD with index
			printCCode(LOAD, GPR1, index);
			printCCode(LOAD, GPR0, source);
			printCCode(STORE, GPR0, target, GPR1);
			break;
		}
		default:
		{
			cout << "CCodeGenerator: unprocess three-address inst!\n";
			exit(0);
		}
		}
	}

	void CCodeGenerator::printCCode(
		enum MemOp op, enum Registers target, Address const &base, int size) const {

		// Index is simply set 0
		printCCode(op, target, base, ZERO, size);
	}

	void CCodeGenerator::printCCode(
		enum MemOp op,
		enum Registers target, Address const &base, enum Registers index, int size) const {

		cout << "\t"; // For indenting
					  // Generate RISC style load and store instruction
					  // Address plays the role of constant here (it was determined by compiler)
					  // ex) store $gpr0,  constant($gpr1)
		int symtabIndex = base.getSymbolTableIndex();
		shared_ptr<Symbol const> symbol = symbolTable_->lookUp(symtabIndex);

		// Special case for numeric constants
		if (symbol->isConstant() && symbol->getSymbolKind() != SYMBOL_CONSTANT_STRING) {
			if (index != ZERO) {
				cout << "printCode constant base error\n";
				exit(0);
			}
			if (target == M0) { // special case
				cout << "*((int *)M0) = ";
				cout << symbol->getLexeme();
				cout << ";\n";
				return;
			}
			printCCode(target);
			cout << " = ";
			cout << symbol->getLexeme();
			cout << ";\n";
			return;
		}

		printCCode(op); // print 'LOAD' or 'STORE'
		switch (symbol->getSymbolKind()) {
		case SYMBOL_CONSTANT_STRING:
		{
			int offset = symbol->getOffset();
			cout << "(";
			printCCode(target);
			cout << ", STRING_SEGMENT , " << offset << " + ";
			printCCode(index);
			cout << ", " << size << " );\n";
			break;
		}
		case SYMBOL_VAR_G:
		{
			int offset = symbol->getOffset();
			cout << "(";
			printCCode(target);
			cout << ", DATA , " << offset << " + ";
			printCCode(index);
			cout << ", " << size << " );\n";
			break;
		}
		case SYMBOL_VAR_L:
		case SYMBOL_TEMPORARY:
		{
			int offset = symbol->getOffset();
			int width = ((symbol->getType()->getWidth() - 1) / 4 + 1) * 4;
			cout << "(";
			printCCode(target);
			cout << ", STACK , FP + (" << -(offset + width) << ") +";
			printCCode(index);
			cout << ", " << size << " );\n";
			break;
		}
		case SYMBOL_PARAMETER:
		{
			int offset = symbol->getOffset();
			cout << "(";
			printCCode(target);
			cout << ", STACK , FP + (" << offset + 4 << ") +";
			printCCode(index);
			cout << ", " << size << " );\n";
			break;
		}
		default: // should not match
		{
			cout << "printCode memOp symboltype error\n";
			exit(0);
		}
		}
	}

	void CCodeGenerator::printCCode(enum Registers reg) const {
		switch (reg) {
		case GPR0:
			cout << " GPR0 ";
			break;
		case GPR1:
			cout << " GPR1 ";
			break;
		case GPR2:
			cout << " GPR2 ";
			break;
		case M0:
			cout << " M0 ";
			break;
		case ZERO:
			cout << " ZERO ";
			break;
		}
	}

	void CCodeGenerator::printCCode(enum MemOp op) const {
		if (op == LOAD)
			cout << "LOAD";
		else
			cout << "STORE";
	}
} // end namespace subcc
