#ifndef __SYMTAB_H___
#define __SYMTAB_H___


/*
Every Symbol table's first entry is the name(function , procedure , ) for the scope or it will be an local compound statement

*/

#define type_variable 0
#define type_function 1
#define type_procedure 2
#define type_program 3


enum StdType {TypeInt, TypeReal , TypeString , TypeArray , TypeFunction , TypeProcedure , TypeNULL};

extern int current_level;

struct param_list{
    char name[30];
    enum StdType type;
    struct array_descriptor * array;
    struct param_list * next_param;
};

struct array_descriptor{
    enum  StdType type;
    int array_begin;
    int array_end;
    int capacity;
    int iValue;
    double rValue;
    char * string;
    struct array_descriptor * next_array;
};

struct function_attribute{
    enum StdType type;
    struct param_list * param;
};
struct procedure_attribute{
    struct param_list * param;
};

struct SymTableEntry {
    char name[100];
    int iValue;
    char * string;
    double rValue;


    /// used for handle load and store
    int var_num;

    //string type to be added
    int level;
    enum StdType type;
    struct procedure_attribute * procedure;
    struct function_attribute * function ;
    struct array_descriptor * array;
    int array_dim;
};

struct SymTable {
    int size;
    int current_level;
    int capacity;
    struct SymTableEntry entries[1024];
};

struct SymTable SymbolTable;
#include "node-type.h"
void semanticCheck(struct node* node);
struct param_list * newParam(char * s , enum StdType type);
void add_param(struct param_list * root , struct param_list * child);
enum  StdType StdtypeCheck(struct node * node );
struct array_descriptor * newArrayDes(enum StdType type);
struct function_attribute * newFunctionAttr(enum StdType type);
struct procedure_attribute * newProcedureAttr();
void add_ArrayDescri_list(struct  array_descriptor * root , struct array_descriptor * child);
struct array_descriptor *  initArray(struct node * array_node );
struct SymTableEntry* findSymbol_in_scope(char *s);
struct SymTableEntry* findSymbol_fun_pro_var(char *s);
struct SymTableEntry* findSymbol_in_global(char *s);
struct SymTableEntry* addVariable(char *s, enum StdType type , struct array_descriptor * array_desc) ;
struct SymTableEntry* addFunction(char *s, enum StdType type , struct function_attribute * function);
struct SymTableEntry* addProcedure(char *s, enum StdType type , struct procedure_attribute * procedure);
struct node* nthChild(int n, struct node *node);
struct param_list * initParam(struct node * parameter );
void printf_symbol_table();
#endif

