# subcc (subc compiler)
Compiler for subc (subc = subset of C = simple version of C)

Usage
```
// edit subcc/subc_code files as you like

cd _build
cmake ..
make -j
./SubccMain

// copy&paste content of subcc/output/test.c (or another one) to interpreter/assembly.cc (overwrite)
make -j
./InterpreterMain
