#ifndef __CODE_GEN_H__
#define __CODE_GEN_H__

#include "node-type.h"
#include "symtab.h"

void gen_program_start();
void gen_program_end();
void travel_node(struct node * node);
void gen_global_var();
void gen_vinit();
int count_param(struct node * node);
void gen_label(node);
struct SymTableEntry * findSymbol_in_function_procedure(char * s);
struct SymTableEntry * findSymbol_in_main(char * s);
void gen_program_main();
#endif