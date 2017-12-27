# Compiler_Project_Code_Generation
code generation by Jasmin

Step flow
1.With command flex Scanner.l
Scanner.l -> lex.yy.c

2.With command yacc -d Parser.y
Parser.y -> y.tab.c 

3.compiler out execution file a.out
gcc y.tab.c lex.yy.c symtab.c node-type.c code_gen.c -ly


###################if no error###################
4.

If the mini-pascal file (xxx.p) has no syntax error or semantic error , it will create a foo.j which is in javabyte 

