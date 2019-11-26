#pragma once

#include <deque>
#include <iostream>
#include <string>
#include "lexer_generator.h"

namespace subcc
{
using namespace std;
class Symbol;
class SymbolTable;
int const maxNumericType = 10;

enum Types
{
  TYPE_BASE = 1,
  TYPE_ARRAY = 2,
  TYPE_RECORD = 3,
  TYPE_FUNC = 4,
  TYPE_PRODUCT = 5,
  TYPE_ERROR = 100 // indicates type error
};

// value below NUMERIC_TYPE_MAX is reserved for numeric type
// The larger the value, the wider the type width
enum BaseTypes
{
  BASETYPE_BOOL = 1,
  BASETYPE_CHAR = 2,
  BASETYPE_SHORT = 3,
  BASETYPE_INT = 4,
  BASETYPE_FLOAT = 5,
  BASETYPE_DOUBLE = 6,
  BASETYPE_STRING = 11, // only for string constant
  BASETYPE_VOID = 21    // for return value, arg type, and pointers
};

class Type
{
public:
  virtual ~Type() { objectCount_--; /*std::cout << "Type destroyed: " << count << "\n";*/ }
  virtual bool operator==(Type const &) const = 0;
  virtual bool operator!=(Type const &) const = 0;
  enum Types getType() const { return typeType_; }
  int getWidth() const { return width_; }
  // Info for type resolution
  virtual bool isNumeric() const { return false; }
  // Returns whether a variable can be declared with this type
  virtual bool isVarDeclarable() const { return false; }
  virtual bool isError() const { return false; }
  bool isBaseType() const { return typeType_ == TYPE_BASE; }
  bool isRecordType() const { return typeType_ == TYPE_RECORD; }
  bool isArrayType() const { return typeType_ == TYPE_ARRAY; }

public:
  static void countObjects();

protected:
  Type(enum Types typeType) : typeType_(typeType)
  {
    objectCount_++;
    /*std::cout << "Type created: " << count << "\n";*/ } // should not be public
    Type(enum Types typeType, int width) : typeType_(typeType), width_(width)
    {
      objectCount_++; /*std::cout << "Type created: " << count << "\n";*/
    }
    enum Types const typeType_; // type of type
    int width_;                 // memory space in bytes
  private:
    static int objectCount_;
};

class BaseType : public Type
{
public:
  BaseType(enum BaseTypes baseTypeValue);
  ~BaseType();
  virtual bool operator==(Type const &) const;
  virtual bool operator!=(Type const &) const;
  enum BaseTypes getBaseType() const { return baseTypeValue_; }
  bool isNumeric() const { return (baseTypeValue_ < maxNumericType); }
  bool isVarDeclarable() const { return isNumeric(); }

protected:
  enum BaseTypes const baseTypeValue_; // for base types
};

// Revised to use a symbol table itself as type definition.
// Equality holds only for identical types: not structural equality!
class RecordType : public Type
{
public:
  RecordType(string const &name, SymbolTable const &environment);
  ~RecordType();
  bool operator==(Type const &) const;
  bool operator!=(Type const &) const;
  string const &getName() const { return name_; }
  // type of given field; return TYPE_ERROR if faulty
  shared_ptr<Type const> getTypeOf(string const &field) const;
  // Returns symbol table index
  int getIndexOf(string const &field) const;
  bool isVarDeclarable() const { return true; }

protected:
  string name_;
  SymbolTable const &environment_; // basis of type equivalence
};

class ArrayType : public Type
{
public:
  ArrayType(int arraySize, shared_ptr<Type const> elementType) : Type(TYPE_ARRAY, elementType->getWidth() * arraySize),
                                                                 arraySize_(arraySize), elementType_(elementType) {}
  // make in one shot; should not be called with empty deque!
  ArrayType(std::deque<int> const &, shared_ptr<Type const>);
  ~ArrayType(); // should destruct recursively
  bool operator==(Type const &) const;
  bool operator!=(Type const &) const;
  int getArraySize() const { return arraySize_; }
  shared_ptr<Type const> getElementType() const { return elementType_; }
  bool isVarDeclarable() const { return elementType_->isVarDeclarable(); }

protected:
  int const arraySize_;
  shared_ptr<Type const> elementType_; // should not be reference
};

// Artificial type for function arguments: equivalence determines the validity
// ..of function call statement. So convertible types (char <-> int) should be
// ..considered equal in comparison of each element types.
class ProductType : public Type
{
public:
  ProductType(vector<shared_ptr<Type const>> const &types);
  ~ProductType();
  bool operator==(Type const &) const; // for function call arguments validity
  bool operator!=(Type const &) const;
  int getNum() const { return static_cast<int>(types_.size()); }
  shared_ptr<Type const> getTypeOf(int index) const { return types_[index]; }

protected:
  vector<shared_ptr<Type const>> const types_;
};

// domain type is always product type
class FuncType : public Type
{
public:
  FuncType(shared_ptr<ProductType const> domainType,
           shared_ptr<Type const> codomainType) : Type(TYPE_FUNC, 0), domainType_(domainType), codomainType_(codomainType) {}
  ~FuncType();
  bool operator==(Type const &) const;
  bool operator!=(Type const &) const;
  shared_ptr<ProductType const> getDomainType() const { return domainType_; }
  shared_ptr<Type const> getCodomainType() const { return codomainType_; }

protected:
  shared_ptr<ProductType const> domainType_;
  shared_ptr<Type const> codomainType_;
};

class ErrorType : public Type
{
public:
  ErrorType() : Type(TYPE_ERROR) {}
  bool operator==(Type const &) const { return false; }
  bool operator!=(Type const &) const { return true; }
  bool isError() const { return true; }
};

/**** Global type objects *****/
shared_ptr<Type const> getBaseType(enum BaseTypes baseType);
/*
extern BaseType const BoolType;
extern BaseType const CharType;
extern BaseType const ShortType;
extern BaseType const IntType;
extern BaseType const FloatType;
extern BaseType const DoubleType;
extern BaseType const StringType;
extern BaseType const VoidType;
*/

/****** Type manipulating functions *******/
// Unify two numeric type; return TYPE_ERROR if not possible
shared_ptr<Type const> getMaxOfTypes(shared_ptr<Type const>, shared_ptr<Type const>);

bool isCharType(shared_ptr<Type const> type);
bool isVoidType(shared_ptr<Type const> type);

bool isCompatible(ProductType const &, ProductType const &);

} // namespace subcc

