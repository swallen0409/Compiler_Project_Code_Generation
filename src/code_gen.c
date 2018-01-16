#include "code_gen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int lineCount;
extern FILE * fp;
extern struct SymTable SymbolTable;
int scope = 0;

#define type_void 0
#define type_int 1
#define type_real 2
#define type_string 3

int function_scope = 0;
int function_type = 0;
int procedure_scope = 0;
int main_scope = 0;
int while_scope = 0;
int if_scope = 0;


/********** generate start of xxx.j *********/
void gen_program_start(){
    printf("start\n");
    fprintf(fp , ".class public foo\n");
    fprintf(fp , ".super java/lang/object\n\n");
    //fprintf(fp, ".field public static _sc Ljava/util/Scanner;\n ");
    return;
}
/********** generate end of xxx.j ***********/
void gen_program_end(struct node * node){
    if(function_type == type_void)
        fprintf(fp, "return\n");
    else if(function_type == type_int)
        fprintf(fp , "ireturn\n");
    else if(function_type == type_real)
        fprintf(fp , "freturn\n");
    //// string return 
	fprintf(fp, ".end method\n");
    return;
}
/********* generate global variable *********/
void gen_global_var(char * name , enum StdType type , int dim , enum StdType  array_type){
    printf("global var\n");
    if(type == TypeInt){/// int declaration
		fprintf(fp, ".field public static %s %s\n",name,"I");
    }
    else if(type == TypeReal){/// real declaration
		fprintf(fp, ".field public static %s %s\n",name,"F");
    }
    else if(type == TypeString){/// string declaration
		fprintf(fp, ".field public static %s %s\n",name,"S");
    }
    else {
        fprintf(fp, ".field public static %s ",name);
        for(int i = 0 ; i<dim ;i++){
            fprintf(fp,"%s", "[");
        }
        if(array_type == TypeInt)
            fprintf(fp , "%s\n" , "I");
        else if(array_type == TypeReal)
            fprintf(fp , "%s\n" , "F");
        else 
            fprintf(fp , "%s\n" , "S");
    }

}
void gen_vinit(){
///////// initiate global variables /////////
    fprintf(fp , "\n.method public static vinit()V\n.limit locals 100\n.limit stack 100");
    for(int i = 0 ; i<SymbolTable.size ;i++){
        if(SymbolTable.entries[i].level !=0)
            break;
        if(SymbolTable.entries[i].type == TypeInt){
            fprintf(fp , "ldc 0\n");
            fprintf(fp , "putstatic foo/%s I\n" , SymbolTable.entries[i].name );
        }
        else if(SymbolTable.entries[i].type == TypeReal){
            fprintf(fp , "ldc 0.0\n");
            fprintf(fp , "putstatic foo/%s F\n" , SymbolTable.entries[i].name );
        }
        else if(SymbolTable.entries[i].type == TypeString){
            fprintf(fp , "sipush 0\n");
            fprintf(fp , "putstatic foo/%s S\n" , SymbolTable.entries[i].name );
        }
        else if(SymbolTable.entries[i].type == TypeArray){//to be continued

        }
    }
	fprintf(fp , "return\n.end method\n\n");
    ////// <init> handle
    fprintf(fp , ".method public <init>()V\naload_0\ninvokenonvirtual java/lang/Object/<init>()V\nreturn\n.end method\n\n");
}
/********** DFS to create an java bytecode ***********/
void travel_node(struct node * node){
//    if(node->lsibling->nodeType == NODE_SUB_DECLS)
//        scope=1;
    printf("Here %d\n" ,node->nodeType );
    switch(node->nodeType) {
        case NODE_BEGIN:{
            break;
        }
        case NODE_FUN_HEAD:{
            break;
        }
        case NODE_PRO_HEAD:{
            break;
        }
        case NODE_END:{
            gen_program_end(node);
            return;
        }
        case NODE_DECL:{
        ///////////// no declaration /////////////          
            if(node->child == NULL)
                    return;
        ///////////// declaration ////////////////    
            else {
                struct node * check_declaration = node->child;
                int check_type = 2;
                int check_node = 1;
                do{
                        struct node *typeNode = nthChild(check_type, node);//node type
                        enum StdType valueType = StdtypeCheck(typeNode);
                        /********* Array variable declaration implement here******************/
                        if(valueType == TypeArray){
                            struct node * array_node = typeNode->child;
                            struct array_descriptor * array_desc_root = initArray(array_node);
                            struct node *idList = nthChild(check_node, node);
                            struct node *idNode = idList->child;
                            int dim = 1;
                            while(array_desc_root->type == TypeArray){
                                dim++;
                                array_desc_root=array_desc_root->next_array;
                            }
                            enum StdType array_type = array_desc_root->type;

                            do {
                                gen_global_var(idNode->string, valueType , dim , array_type);
                                idNode = idNode->rsibling;
                            } while(idNode != idList->child);                          
                        }
                        /*********int , string , real variable declaration implement here***********/
                        else {
                            struct node *idList = nthChild(check_node, node);
                            struct node *idNode = idList->child;
                            do {
                                gen_global_var(idNode->string, valueType , 0 , 0);
                                idNode = idNode->rsibling;
                            } while(idNode != idList->child);
                        }
                        check_declaration = typeNode->rsibling;
                        check_node+=2;
                        check_type+=2;
                }while(check_declaration != node->child);
                return ;
            }
        }
        case NODE_SYM_REF:{
            return;
        }
        case NODE_OP:{
            return;
        }
        case NODE_IF:{
            return;
        }
        case NODE_WHILE:{
            return;
        }
        case NODE_INT:{
            fprintf(fp , "ldc %d\n" , node->iValue);
            return;
        }
        case NODE_REAL:{
            fprintf(fp , "ldc %f\n" , node->rValue);
            return;
        }
        case NODE_STRING_v:{
            fprintf(fp , "ldc %s\n" , node->string);
            return;
        }
        case NODE_ASSIGN_STMT:{
            return;
        }
    }
    /* Default action for other nodes not listed in the switch-case */
    struct node *child = node->child;
    if(child != NULL) {
        do {
            travel_node(child);
            child = child->rsibling;
        } while(child != node->child);
    }
}