#!/bin/bash

flex Scanner.l
yacc Parser.y
gcc y.tab.c lex.yy.c symtab.c node-type.c code_gen.c -ly
cp ./a.out ../TEST-COMPILER04-FOR-CODEGEN/no-error
cp ./a.out ../TEST-COMPILER04-FOR-CODEGEN/
cd ../TEST-COMPILER04-FOR-CODEGEN/no-error
java -jar jasmin.jar foo.j
cd ..
cd ../src
