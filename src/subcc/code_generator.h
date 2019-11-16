#ifndef LIB_CALVIN__SUBCC__C_CODE_GENERATOR_H
#define LIB_CALVIN__SUBCC__C_CODE_GENERATOR_H

/* 2008-06-08 */

#include "three_address.h"

namespace subcc {

// SymbolTable + ThreeAddr---> Target code
class CCodeGenerator {
public:
  CCodeGenerator(shared_ptr<ThreeAdressCode const> threeAddrCode):
    threeAddrCode_(threeAddrCode),
		symbolTable_(threeAddrCode->getSymbolTable()) { }

  // For compiler homework...
  void printCCode() const; 
  
private:
  enum Registers { GPR0, GPR1, GPR2, M0, ZERO };
  enum MemOp { LOAD, STORE };
  
  void printCCode(Instruction const &) const;
  void printCCode(enum MemOp op, enum Registers base, 
      Address const &, int size = 4) const;
  void printCCode(enum MemOp op, enum Registers base, 
      Address const &, enum Registers index, int size = 4) const;
  void printCCode(enum Registers reg) const;
  void printCCode(enum MemOp op) const;
  
  shared_ptr<ThreeAdressCode const> threeAddrCode_;
  shared_ptr<GlobalSymbolTable const> symbolTable_;
};

}

#endif
