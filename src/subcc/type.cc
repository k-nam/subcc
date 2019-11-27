/* Type.cc */

#include <iostream>
#include <deque>
#include "type.h"
#include "symbol_table.h"

namespace subcc
{
using namespace std;

// singleton types
std::shared_ptr<Type const>
getBaseType(enum BaseTypes baseType)
{
	static shared_ptr<Type const> boolType(new BaseType(BASETYPE_BOOL));
	static shared_ptr<Type const> charType(new BaseType(BASETYPE_CHAR));
	static shared_ptr<Type const> shortType(new BaseType(BASETYPE_SHORT));
	static shared_ptr<Type const> intType(new BaseType(BASETYPE_INT));
	static shared_ptr<Type const> floatType(new BaseType(BASETYPE_FLOAT));
	static shared_ptr<Type const> doubleType(new BaseType(BASETYPE_DOUBLE));
	static shared_ptr<Type const> stringType(new BaseType(BASETYPE_STRING));
	static shared_ptr<Type const> voidType(new BaseType(BASETYPE_VOID));

	switch (baseType)
	{
	case BASETYPE_BOOL:
		return boolType;
	case BASETYPE_CHAR:
		return charType;
	case BASETYPE_SHORT:
		return shortType;
	case BASETYPE_INT:
		return intType;
	case BASETYPE_FLOAT:
		return floatType;
	case BASETYPE_DOUBLE:
		return doubleType;
	case BASETYPE_STRING:
		return stringType;
	case BASETYPE_VOID:
		return voidType;
	default:
		std::cout << "getBaseType() error!!\n";
		exit(0);
	}
}

/******** BaseType methods ******/

void Type::countObjects()
{
	if (objectCount_ != 0)
	{
		std::cout << "Type object memory leak! # was: " << objectCount_ << "\n";
		std::cout << "8 is OK because those are built-in types and declared static\n";
		if (objectCount_ != 8)
		{
			exit(0);
		}
	}
	else
	{
		std::cout << "Type object memory OK!\n";
	}
	std::cout << "\n";
}

int Type::objectCount_ = 0;

BaseType::BaseType(enum BaseTypes baseTypeValue) : Type(TYPE_BASE),
												   baseTypeValue_(baseTypeValue)
{
	switch (baseTypeValue)
	{
	case BASETYPE_BOOL:
		width_ = 1;
		break;
	case BASETYPE_CHAR:
		width_ = 1;
		break;
	case BASETYPE_SHORT:
		width_ = 2;
		break;
	case BASETYPE_INT:
		width_ = 4;
		break;
	case BASETYPE_FLOAT:
		width_ = 4;
		break;
	case BASETYPE_DOUBLE:
		width_ = 8;
		break;
	case BASETYPE_STRING:
		width_ = 80;
		break;
	default: // incompatible types: can not have space
		width_ = 0;
	}
}

BaseType::~BaseType()
{
	//std::cout << "Basetype deleted !!!!!\n";
}

bool BaseType::operator==(Type const &rhs) const
{
	if (rhs.getType() != TYPE_BASE)
		return false;
	BaseType const &rhsType = dynamic_cast<BaseType const &>(rhs);
	if (rhsType.getBaseType() != baseTypeValue_)
		return false;
	return true;
}

bool BaseType::operator!=(Type const &rhs) const
{
	return !(operator==(rhs));
}

/******* FuncType methods *********/

FuncType::~FuncType()
{
	//std::cout << "fuctype deleted\n";
	if (domainType_->isBaseType() == false && domainType_->isRecordType() == false)
	{
		//std::cout << "domaintype is: \n";
		//domainType_->print();
		//delete &domainType_;
	}
	if (codomainType_->isBaseType() == false && codomainType_->isRecordType() == false)
	{
		//delete &codomainType_;
	}
}

bool FuncType::operator==(Type const &rhs) const
{
	if (rhs.getType() != TYPE_FUNC)
		return false;
	FuncType const &rhsType = dynamic_cast<FuncType const &>(rhs);
	if (rhsType.getDomainType() != domainType_ ||
		rhsType.getCodomainType() != codomainType_)
		return false;
	return true;
}

bool FuncType::operator!=(Type const &rhs) const
{
	return !(operator==(rhs));
}

/******  RecordType methods ****/

RecordType::RecordType(string const &name, SymbolTable const &environment) : Type(TYPE_RECORD), name_(name), environment_(environment)
{
	width_ = environment.getOffset();
}

RecordType::~RecordType()
{
	//std::cout << "record type deleted\n";
}

bool RecordType::operator==(Type const &rhs) const
{
	if (rhs.getType() != TYPE_RECORD)
		return false;
	RecordType const &rhsType = dynamic_cast<RecordType const &>(rhs);
	if (&environment_ == &rhsType.environment_) // same type
		return true;
	return false;
}

bool RecordType::operator!=(Type const &rhs) const
{
	return !(operator==(rhs));
}

shared_ptr<Type const>
RecordType::getTypeOf(string const &field) const
{
	int index = environment_.findIndex(field);
	if (index < 0) // does not have that field
		return shared_ptr<Type const>(new ErrorType());
	else
		return environment_.getTypeOf(field);
}

int RecordType::getIndexOf(string const &field) const
{
	int index = environment_.findIndex(field);
	if (index < 0)
	{
		cout << "RecordType::getOffsetOf(): field non-existent\n";
		exit(0);
	}
	return index;
}

/****** ArrayType methods *****/

ArrayType::ArrayType(deque<int> const &inDimensions,
					 shared_ptr<Type const> elementType) : Type(TYPE_ARRAY), arraySize_(inDimensions.front()), elementType_()
{

	deque<int> dimensions = inDimensions; // copy to modify
	if (dimensions.size() == 1)
	{ // last stage
		elementType_ = elementType;
		width_ = elementType->getWidth() * arraySize_;
		return;
	}
	if (dimensions.size() > 1)
	{ // need recursive step
		dimensions.pop_front();
		shared_ptr<ArrayType> childType(new ArrayType(dimensions, elementType));
		elementType_ = childType;
		width_ = childType->getWidth() * arraySize_;
		return;
	}
	// error
	cout << "ArrayType construction error: deque should not be empty\n";
}

ArrayType::~ArrayType()
{
	//std::cout << "array type deleted\n";
}

bool ArrayType::operator==(Type const &rhs) const
{
	if (rhs.getType() != TYPE_ARRAY)
		return false;
	ArrayType const &rhsType = dynamic_cast<ArrayType const &>(rhs);
	if (rhsType.arraySize_ != arraySize_)
		return false;
	if (*rhsType.getElementType() != *elementType_)
		return false;
	return true;
}

bool ArrayType::operator!=(Type const &rhs) const
{
	return !(operator==(rhs));
}

/*********** ProductType methods ***********/

ProductType::ProductType(vector<shared_ptr<Type const>> const &types) : Type(TYPE_PRODUCT), types_(types)
{

	width_ = 0;
	for (size_t i = 0; i < types.size(); ++i)
	{
		width_ += types_[i]->getWidth();
	}
}

ProductType::~ProductType()
{
	//std::cout << "product type deleted\n";
	for (size_t i = 0; i < types_.size(); ++i)
	{
		if (types_[i]->isBaseType() == false && types_[i]->isRecordType() == false)
		{
			//types_[i]->print();
			//delete types_[i];
		}
	}
}

bool ProductType::operator==(Type const &rhs) const
{
	if (rhs.getType() != TYPE_PRODUCT)
		return false;
	ProductType const &rhsType = dynamic_cast<ProductType const &>(rhs);
	if (rhsType.getNum() != this->getNum())
		return false;
	for (int i = 0; i < this->getNum(); ++i)
	{
		if (rhsType.getTypeOf(i) != this->getTypeOf(i))
			return false;
	}
	return true;
}

bool ProductType::operator!=(Type const &rhs) const
{
	return !(operator==(rhs));
}

/**************************** Global Functions *****************************/

// Type unification for only numeric base types
// Return TYPE_ERROR if not possible
std::shared_ptr<Type const>
getMaxOfTypes(shared_ptr<Type const> t1, shared_ptr<Type const> t2)
{
	// Deals only with base types; ERROR otherwise
	if (t1->getType() != TYPE_BASE || t2->getType() != TYPE_BASE)
		return shared_ptr<Type const>(new ErrorType());
	shared_ptr<BaseType const> b1 = dynamic_pointer_cast<BaseType const>(t1);
	shared_ptr<BaseType const> b2 = dynamic_pointer_cast<BaseType const>(t2);
	if (!b1->isNumeric() || !b2->isNumeric())
	{
		return shared_ptr<Type const>(new ErrorType());
	}
	if (b1->getBaseType() >= b2->getBaseType()) // t1 is wider
		return t1;
	else
		return t2;
}

// Tests whether char type or not
bool isCharType(shared_ptr<Type const> type)
{
	if (type->getType() != TYPE_BASE)
		return false;
	shared_ptr<BaseType const> baseType = dynamic_pointer_cast<BaseType const>(type);
	if (baseType->getBaseType() != BASETYPE_CHAR)
		return false;
	return true;
}

bool isVoidType(shared_ptr<Type const> type)
{
	if (type->getType() != TYPE_BASE)
		return false;
	shared_ptr<BaseType const> baseType = dynamic_pointer_cast<BaseType const>(type);
	if (baseType->getBaseType() != BASETYPE_VOID)
		return false;
	return true;
}
// Tests whether valid function call arguments (compared to declared type)
bool isCompatible(ProductType const &lhs,
				  ProductType const &rhs)
{
	if (lhs.getNum() != rhs.getNum())
		return false;
	for (int i = 0; i < lhs.getNum(); ++i)
	{
		shared_ptr<Type const> leftType = lhs.getTypeOf(i);
		shared_ptr<Type const> rightType = rhs.getTypeOf(i);
		// void, product, and error type are erroneous
		if (!leftType->isVarDeclarable() || !rightType->isVarDeclarable())
			return false;
		// Numeric types are always compatible (through widening or narrowing)
		if (leftType->isNumeric() && rightType->isNumeric())
			continue;
		if (!(*leftType == *rightType))
			return false;
	}
	return true;
}
} // end namespace subcc
