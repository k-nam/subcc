/* SymbolTable.cc */
#include "symbol_table.h"
#include "type.h"
#include "utility.h"
namespace subcc
{

	/************************** Symbol methods ******************************/

	Symbol::Symbol(enum SymbolKinds symbolKind, string const &lexeme,
		shared_ptr<Type const> type) :
		symbolKind_(symbolKind), lexeme_(lexeme), type_(type) {
		++objectCount_;
	}

	Symbol::~Symbol() {
		--objectCount_;
		//std::cout << "Symbol destructed: " << symbolCount << "\n"; 
		if (type_->getType() != TYPE_BASE && type_->getType() != TYPE_RECORD) {
			// Cannot implement memory management of Type object by using Symbol;
			// because for array Types, there are Temporary types for
			// each level of arrays, and this results in multiple deletes
			//delete &type_; 
		}
	}

	bool Symbol::isVariable() const {
		return (symbolKind_ < SYMBOL_VAR_MAX);
	}

	bool Symbol::isTemporary() const {
		return (symbolKind_ > SYMBOL_VAR_MAX && symbolKind_ < SYMBOL_TEMP_MAX);
	}

	bool Symbol::isConstant() const {
		return (symbolKind_ > SYMBOL_TEMP_MAX && symbolKind_ < SYMBOL_CONST_MAX);
	}

	bool Symbol::isStringLiteral() const {
		return (symbolKind_ == SYMBOL_CONSTANT_STRING);
	}

	void Symbol::countObjects() {
		if (objectCount_ != 0) {
			std::cout << "Symbol object memory leak! # was: " << objectCount_ << "\n";
			exit(0);
		} else {
			std::cout << "Symbol object memory OK!\n";
		}
		std::cout << "\n";
	}

	int Symbol::objectCount_ = 0;

	/********************* Constant methods **************************/

	Constant::Constant(char charValue) :
		Symbol(SYMBOL_CONSTANT_CHAR, string(stringOfInt(charValue)),
			getBaseType(BASETYPE_CHAR)) {
		constant_.charValue_ = charValue;
	}

	Constant::Constant(short shortValue) :
		Symbol(SYMBOL_CONSTANT_SHORT, string(stringOfInt(shortValue)),
			getBaseType(BASETYPE_SHORT)) {
		constant_.shortValue_ = shortValue;

	}

	Constant::Constant(int intValue) :
		Symbol(SYMBOL_CONSTANT_INT, string(stringOfInt(intValue)),
			getBaseType(BASETYPE_INT)) {
		constant_.intValue_ = intValue;

	}

	Constant::Constant(string const &lexeme) :
		Symbol(SYMBOL_CONSTANT_STRING, lexeme,
			shared_ptr<ArrayType const>(new ArrayType(
				static_cast<int>(lexeme.length()) - 2, getBaseType(BASETYPE_CHAR)))) {
	}

	Constant::~Constant() {
		//print();
	}

	/*********************** Temporary methods *****************************/

	Temporary::~Temporary() {
		//print();
	}

	/*********************** SymbolTable methods ***************************/

	SymbolTable::SymbolTable(GlobalSymbolTable const &globalTable,
		SymbolTable *parent, bool isEnvironment) :
		globalTable_(globalTable), parent_(parent),
		offset_(isEnvironment ? 0 : parent->getOffset()),
		isEnvironment_(isEnvironment) {
		objectCount_++;
		//std::cout << "Symtab created: " << symtabCount << "\n";
	}

	SymbolTable::SymbolTable(GlobalSymbolTable const &globalTable) :
		globalTable_(globalTable), parent_(NULL), offset_(0),
		isEnvironment_(false) {
		objectCount_++;
		//std::cout << "Symtab created: " << symtabCount << "\n";
	}

	SymbolTable::~SymbolTable() {
		objectCount_--;
		//std::cout << "Symtab destroyed: " << symtabCount << "\n";
		for (size_t i = 0; i < children_.size(); ++i) {
			delete children_[i];
		}
	}

	int SymbolTable::findIndex(string const &lexeme) const {
		map<string, int>::const_iterator iter = mapping_.find(lexeme);
		if (iter == mapping_.end())
			return -1;
		else { // matching
			return iter->second;
		}
	}

	shared_ptr<Type const>
		SymbolTable::getTypeOf(string const &lexeme) const {
		int index = findIndex(lexeme);
		if (index < 0)
			return shared_ptr<Type const>(new ErrorType());
		return globalTable_.lookUp(index)->getType();
	}

	void SymbolTable::insert(shared_ptr<Symbol> symbol, int index) {
		// temporaries need not have distinct lexemes, and does not support lookup
		if (!symbol->isTemporary()) {
			if (mapping_.count(symbol->getLexeme()) != 0) { // already declared symbol
				cout << "SymbolTable insert error\n";
				cout << "lexeme is ";
				cout << symbol->getLexeme();
				exit(0);
			}
			mapping_[symbol->getLexeme()] = index;
		}
		// Manage offset of the symbol and width of symbol table
		// only variables and temporaries occupy memory space
		// We must consider 'alignment' with regard to type width
		if (symbol->isVariable() || symbol->isTemporary()) {
			symbol->setOffset(offset_);
			int typeWidth = symbol->getType()->getWidth();
			// Just align to 4's multiple now
			typeWidth = 4 * ((typeWidth - 1) / 4 + 1);
			offset_ += typeWidth;
			//cout << "symbol: ";
			//cout << " offset is " << symbol.getOffset() << endl;
			const_cast<GlobalSymbolTable &>(globalTable_).updateMaxStackSize(offset_);
		} else if (symbol->isStringLiteral()) {
			auto lexeme = symbol->getLexeme();
			int offset = const_cast<GlobalSymbolTable &>(globalTable_).saveStringLiteral(
				lexeme.substr(1, lexeme.length() - 1));
			symbol->setOffset(offset);
		}
	}

	SymbolTable const &
		SymbolTable::makeChild(bool isEnvironment) {
		children_.push_back(new SymbolTable(globalTable_, this, isEnvironment));
		return *children_.back();
	}

	void SymbolTable::countObjects() {
		if (objectCount_ != 0) {
			std::cout << "SymbolTable object memory leak! # was: " << objectCount_ << "\n";
			exit(0);
		} else {
			std::cout << "SymbolTable object memory OK!\n";
		}
		std::cout << "\n";
	}

	int SymbolTable::objectCount_ = 0;

	/****************** GlobalSymbolTable methods ****************/

	GlobalSymbolTable::GlobalSymbolTable() {
		root_ = new SymbolTable(*this);
		curTable_ = root_;
		objectCount_++;
	}

	GlobalSymbolTable::~GlobalSymbolTable() {
		// Delete symbol tables (they are in tree structure)
		delete root_; // will be recursive operation
		objectCount_--;
		// Delete symbols
		/* Using shared_ptr makes this unnecessary
		for (int i = 0; i < array_.size(); ++i) {
		delete array_[i];
		}*/
	}

	int GlobalSymbolTable::saveStringLiteral(string stringLiteral) {
		int offset = static_cast<int>(stringSegment_.length());
		stringSegment_ += stringLiteral;
		return offset;
	}

	void GlobalSymbolTable::pushTable() {
		curTable_ = const_cast<SymbolTable *>(&curTable_->makeChild(false));
	}

	void GlobalSymbolTable::pushEnvironment() {
		curTable_ = const_cast<SymbolTable *>(&curTable_->makeChild(true));
	}

	void GlobalSymbolTable::pushFunction() {
		curTable_ = const_cast<SymbolTable *>(&curTable_->makeChild(false));
		curTable_->setOffsetToZero();
		maxStackSize_ = 0; // reset maximum stack size
	}

	bool GlobalSymbolTable::popTable() {
		if (curTable_->getParent() != NULL) { // not root
			SymbolTable *parentTable = curTable_->getParent();
			curTable_ = parentTable;
			return true;
		} else // curTable is root
			return false;
	}

	int GlobalSymbolTable::findIndex(string const &lexeme) const {
		SymbolTable *tempTable = curTable_;
		while (true) {
			int index = tempTable->findIndex(lexeme);
			if (index >= 0)
				return index;
			if (tempTable->getParent() == NULL) // root table
				return -1;
			tempTable = tempTable->getParent();
		}
	}

	shared_ptr<Symbol const>
		GlobalSymbolTable::lookUp(int index) const {
		if (index >= static_cast<int>(array_.size())) {
			cout << "symboltable lookup index error\n";
			exit(0);
		}
		return array_[index];
	}

	SymbolTable const &
		GlobalSymbolTable::getEnvironment() {
		return curTable_->getYoungestChild();
	}

	int GlobalSymbolTable::insert(shared_ptr<Symbol> symbol) {
		int index;
		SymbolTable *tableToInsert = curTable_;
		switch (symbol->getSymbolKind()) {
		case SYMBOL_STRUCT:
			// Symbol is a struct name, so get out to the closest non-env table
			while (tableToInsert->isEnvironment())
				tableToInsert = tableToInsert->getParent();
			index = tableToInsert->findIndex(symbol->getLexeme());
			if (index >= 0) // already declared, so faulty!
				return -1;
			break;
		case SYMBOL_FUNC:
			// Functions are always declared in global scope (in C)
			tableToInsert = root_;
			index = tableToInsert->findIndex(symbol->getLexeme());
			if (index >= 0) // already declared, so faulty!
				return -1;
			break;
		case SYMBOL_VAR_G:
		case SYMBOL_VAR_L:
		case SYMBOL_ELEM:
		case SYMBOL_PARAMETER:
		case SYMBOL_TYPENAME:
			// Inspect only the topmost scope
			index = curTable_->findIndex(symbol->getLexeme());
			if (index >= 0) // already declared, so faulty!
				return -1;
			break;
		case SYMBOL_TEMPORARY:
			// Originally, we insert temporaries to global scope; but for simplicity
			// ..we insert them into the current scope, the same as variables
			// Address binding for temporaries happen right away with the current
			// ..symbol table

			// Check validity of insertion (duplicate lexeme), and do preliminary Action
			break;
		case SYMBOL_CONSTANT_CHAR:
		case SYMBOL_CONSTANT_SHORT:
		case SYMBOL_CONSTANT_INT:
		case SYMBOL_CONSTANT_FLOAT:
		case SYMBOL_CONSTANT_DOUBLE:
		case SYMBOL_CONSTANT_STRING:
			// constant symbol: no problem even if already declared!
			index = root_->findIndex(symbol->getLexeme());
			// already used constant
			if (index >= 0) { 
				return index;
			} else {
				index = setEntry(symbol);
				root_->insert(symbol, index);
				return index;
			} // Constants are done here!
		default:
			cout << "GlobalSymbolTable::insert() error\n";
			exit(0);
		}
		// Insert symbol
		index = setEntry(symbol);
		tableToInsert->insert(symbol, index);
		return index;
	}

	bool GlobalSymbolTable::updateMaxStackSize(int newSize) {
		if (newSize > maxStackSize_) {
			maxStackSize_ = newSize;
			return true;
		}
		return false;
	}

	int GlobalSymbolTable::setEntry(shared_ptr<Symbol const> symbol) {
		array_.push_back(symbol);
		return static_cast<int>(array_.size()) - 1;
	}

	void GlobalSymbolTable::countObjects() {
		if (objectCount_ != 0) {
			std::cout << "GlobalSymbolTable object memory leak! # was: " << objectCount_ << "\n";
			exit(0);
		} else {
			std::cout << "GlobalSymbolTable object memory OK!\n";
		}
		std::cout << "\n";
	}

	int GlobalSymbolTable::objectCount_ = 0;

} // end namespace subcc



