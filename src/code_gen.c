#include "code_gen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define type_void 0
#define type_int 1
#define type_real 2
#define type_nothing 4

extern int lineCount;
extern FILE * fp;
extern struct SymTable SymbolTable;
int scope = 0;
char function_var[20];
int label_count = -1;
int label_stack[1024];
int stack_top = -1;
int main_end = 0;
int left = 0 ;
int flag = 0;
int function_procedure_scope = 0;
int function_type = 0;
int scope_check_gen = 0;
int param_num = 0;

void init_local_var(){
    int i;
    for( i =  SymbolTable.size-1 ; i >= 0 ; i--){
        struct SymTableEntry * it = &SymbolTable.entries[i];
        //fprintf(fp ,"%s procedure\n",function_var);
        if(it->level == 0 && strcmp(function_var,it->name)==0){
            //fprintf(fp ,"find procedure1\n");
            break;
        }
    }
    int num=0;
    int funnum=0;
    struct SymTableEntry *func_pro_name;
    func_pro_name=&SymbolTable.entries[i];
    i++;
    i+=param_num;
    while( i<SymbolTable.size && SymbolTable.entries[i].level == 1 ){
        struct SymTableEntry * tmp = &SymbolTable.entries[i];
        if(strcmp(function_var , tmp->name) == 0){
            for(int j=i+1;j<SymbolTable.size;j++,funnum++){
                    if(SymbolTable.entries[j].level==0)
                        break;
                }
            tmp->var_num=funnum;  
        }
        else{
            if(func_pro_name->type == TypeFunction)
                tmp->var_num=num+param_num-1;
            else
                tmp->var_num=num+param_num;
        }
        if(tmp->type == TypeInt){
            fprintf(fp , "\tldc 0\n" );
            fprintf(fp , "\tistore %d\n" , tmp->var_num);
        }
        else if(tmp->type == TypeReal){
            fprintf(fp , "\tldc 0.0\n");
            fprintf(fp , "\tfstore %d\n" , tmp->var_num);
        }
        else if(tmp->type == TypeArray){
            int dim=1;
            struct array_descriptor *arr=tmp->array;
            while(arr->type==TypeArray){
                dim++;
                arr=arr->next_array;
            }
            enum StdType type = arr->type;
            arr=tmp->array;
            struct array_descriptor *arr2=arr->next_array;
            struct array_descriptor *arr3;
            if(dim>=3)
                arr3=arr2->next_array;
            for(int j=0;j<dim;j++){
                if(j==0){
                    fprintf(fp , "\tbipush %d\n",arr->capacity+1);
                }
                else if(j==1){
                    fprintf(fp , "\tbipush %d\n",arr2->capacity+1);
                }
                else if(j==2){
                    fprintf(fp , "\tbipush %d\n",arr3->capacity+1);
                }
            }
            fprintf(fp , "\tmultianewarray ");
            for(int j=0;j<dim;j++){
                fprintf(fp , "[");
            }
            
            switch(type){
                case TypeInt:   
                    fprintf(fp , "I ");
                    break;
                case TypeReal:  
                    fprintf(fp , "F ");
                    break;
            }
            fprintf(fp , "%d\n" ,dim);
            fprintf(fp , "\tastore %d\n",tmp->var_num);
        }
        num++;
        i++;
    }
}
void fill_param(struct param_list * param){
    struct param_list * tmp = param;
    int num = 1;
    while(tmp->next_param != tmp){
        num++;
        tmp = tmp->next_param;
    }

    tmp = param;
    while(num > 0){
        for(int i = 0 ; i<num ;i++){
            tmp = tmp->next_param;
        }
        if(tmp->type == TypeInt)
            fprintf(fp , "I");
        else if(tmp->type == TypeReal)
            fprintf(fp , "F");
        else if(tmp->type == TypeArray){
            int dim=1;
            struct array_descriptor *arr=tmp->array;
            while(arr->type == TypeArray){
                dim++;
                arr=arr->next_array;
            }
            enum StdType type = arr->type;
            for(int j=0 ; j<dim ;j++){
                fprintf(fp , "[");
            }
            switch(type){
                case TypeInt:{
                    fprintf(fp , "I");
                    break;
                }
                case TypeReal:{
                    fprintf(fp , "F");
                    break;
                }
            }
        }    
        tmp = param;
        num--;
    }
}
void gen_program_main(){
    fprintf(fp , ".method public static main([Ljava/lang/String;)V\n");
    fprintf(fp , "\t.limit locals 100\n\t.limit stack 100\n\tinvokestatic foo/vinit()V\n");
    fprintf(fp, "\tnew java/util/Scanner\n");
    fprintf(fp, "\tdup\n");
    fprintf(fp, "\tgetstatic java/lang/System/in Ljava/io/InputStream;\n");
    fprintf(fp, "\tinvokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n");
    fprintf(fp, "\tputstatic foo/_sc Ljava/util/Scanner;\n");
    fprintf(fp, "\n");
    return;
}
/********** find symbol entry within function or procedure ***************/
struct SymTableEntry  * findSymbol_in_function_procedure(char * s){
    int num=0;
    int funnum=0;
    int pos = 0;
    for(pos = 0; pos < SymbolTable.size  ; pos++){
        struct SymTableEntry * it = &SymbolTable.entries[pos];
        if(strcmp(it->name, function_var) == 0 && it->level == 0){
            break;
        }
    }
    struct SymTableEntry *func_pro_name=&SymbolTable.entries[pos]; 
    for(int i =  pos+1 ; i < SymbolTable.size ; i++,num++){
        struct SymTableEntry * it = &SymbolTable.entries[i];
        if(it->level == 0)
            break;
        if(strcmp(s, it->name) == 0 && it->level <= 1){
            if(strcmp(function_var , s)==0 && left == 0 )
                break;
            if(strcmp(function_var , s)==0){
                for(int j=i+1;j<SymbolTable.size;j++,funnum++){
                    if(SymbolTable.entries[j].level==0)
                        break;
                }
                it->var_num=funnum;
                return it;
            }
            if(func_pro_name->type == TypeFunction)
                it->var_num=num-1;
            else
                it->var_num=num;
            return it;
        }
    }
    for(int i = 0 ; i < SymbolTable.size  ; i++){
        struct SymTableEntry * it = &SymbolTable.entries[i];
        if(strcmp(s, it->name) == 0 && it->level == 0){
            return it;
        }
    }
    return 0;
}
/********** find symbol entry within main ***************/
struct SymTableEntry  * findSymbol_in_main(char * s){
    for(int i = 0 ; i < SymbolTable.size  ; i++){
        struct SymTableEntry * it = &SymbolTable.entries[i];
        if(strcmp(s, it->name) == 0 && it->level == 0){
            return it;
        }
    }
    return 0;
}
/********** generate start of xxx.j *********/
void gen_program_start(){
    fprintf(fp , ".class public foo\n");
    fprintf(fp , ".super java/lang/Object\n\n");
    fprintf(fp, ".field public static _sc Ljava/util/Scanner;\n ");
    return;
}
/********** generate end of xxx.j ***********/
void gen_program_end(){
    if(main_end == 0){
        if(function_type == type_void){
            fprintf(fp, "\treturn\n");
            fprintf(fp, ".end method\n\n");
        }
        else if(function_type == type_int){
            left = 1;
            struct SymTableEntry * var = findSymbol_in_function_procedure(function_var);
            left = 0;
            fprintf(fp , "\tiload %d\n" , var->var_num);
            fprintf(fp , "\tireturn\n");
            fprintf(fp, ".end method\n\n");
            memset(function_var,0,20);
        }
        else if(function_type == type_real){
            left = 1;
            struct SymTableEntry * var = findSymbol_in_function_procedure(function_var);
            left = 0;
            fprintf(fp , "\tfload %d\n" , var->var_num);
            fprintf(fp , "\tfreturn\n");
            fprintf(fp, ".end method\n\n");
            memset(function_var,0,20);
        }
        function_type = type_void;
    }
    return;
}

/********* generate global variable *********/
void gen_global_var(char * name , enum StdType type , int dim , enum StdType  array_type){
    if(type == TypeInt){/// int declaration
		fprintf(fp, ".field public static %s %s\n",name,"I");
    }
    else if(type == TypeReal){/// real declaration
		fprintf(fp, ".field public static %s %s\n",name,"F");
    }
    else { /// array declaration
        fprintf(fp, ".field public static %s ",name);
        for(int i = 0 ; i<dim ;i++){
            fprintf(fp,"%s", "[");
        }
        if(array_type == TypeInt){
            fprintf(fp , "%s\n" , "I");
        }
        else if(array_type == TypeReal){
            fprintf(fp , "%s\n" , "F");
        }
    }

}
/********* generate init function *********/
void gen_vinit(){
    fprintf(fp , "\n.method public static vinit()V\n\t.limit locals 100\n\t.limit stack 100\n");
    for(int i = 0 ; i<SymbolTable.size ;i++){
        if(SymbolTable.entries[i].level !=0)
            break;
        if(SymbolTable.entries[i].type == TypeInt){
            fprintf(fp , "\tldc 0\n");
            fprintf(fp , "\tputstatic foo/%s I\n" , SymbolTable.entries[i].name );
        }
        else if(SymbolTable.entries[i].type == TypeReal){
            fprintf(fp , "\tldc 0.0\n");
            fprintf(fp , "\tputstatic foo/%s F\n" , SymbolTable.entries[i].name );
        }
        else if(SymbolTable.entries[i].type == TypeArray){
            struct array_descriptor *temp=SymbolTable.entries[i].array;
            int dim=1;
            while(temp->type == TypeArray){
                dim++;
                temp=temp->next_array;
            }
            enum StdType type = temp->type;
            temp=SymbolTable.entries[i].array;
            struct array_descriptor *temp2=temp->next_array;
            struct array_descriptor *temp3;
            if(dim>=3)
                temp3=temp2->next_array;
            for(int j=0;j<dim;j++){
                if(j==0)
                    fprintf(fp , "\tbipush %d\n",temp->capacity+1);
                else if(j==1)
                    fprintf(fp , "\tbipush %d\n",temp2->capacity+1);
                else if(j==2)
                    fprintf(fp , "\tbipush %d\n",temp3->capacity+1);
            }
            fprintf(fp , "\tmultianewarray ");
            for(int j=0;j<dim;j++){
                fprintf(fp , "[");
            }
            
            switch(type){
                case TypeInt:   
                    fprintf(fp , "I ");
                    break;
                case TypeReal:  
                    fprintf(fp , "F ");
                    break;
            }
            fprintf(fp , "%d\n" ,dim);
            fprintf(fp , "\tputstatic foo/%s ", SymbolTable.entries[i].name);
            for(int j=0;j<dim;j++)
                fprintf(fp , "[");
            switch(type){
                case TypeInt:   
                    fprintf(fp , "I\n");
                    break;
                case TypeReal:  
                    fprintf(fp , "F\n");
                    break;
            }
        }
    }
	fprintf(fp , "\treturn\n.end method\n\n");
    ////// <init> handle
    fprintf(fp , ".method public <init>()V\n\taload_0\n\tinvokenonvirtual java/lang/Object/<init>()V\n\treturn\n.end method\n\n");
}
/********** DFS to create an java bytecode ***********/
void travel_node(struct node * node){

    /************* main() starts here ***********/
    if(node->lsibling->nodeType == NODE_SUB_DECLS){
        gen_program_main();
        scope_check_gen = 1; 
    }
    printf("Node type is %d here\n" ,node->nodeType );
    switch(node->nodeType) {
        case NODE_SUB_DECLS:{
            gen_vinit();
            break;
        }
        case NODE_BEGIN:{
            main_end++;
            break;
        }
        /******************
        parameter is not finished yet
        ********************/
        case NODE_FUN_HEAD:{
            fprintf(fp , ".method public static ");
            struct node * function_name = nthChild(1 , node);
            struct node * parameter = nthChild(2 , node);
            struct node * type_name = nthChild(3 , node);
            fprintf(fp , "%s(" , function_name->string);
            strcpy(function_var , function_name->string);
            if(parameter->nodeType != NODE_lambda){
                struct SymTableEntry * function = findSymbol_in_main(function_name->string);
                struct param_list * param = function->function->param;
                fill_param(param);
            }

            if (type_name->nodeType==NODE_TYPE_INT){
                fprintf(fp , ")I\n");
                function_type = type_int;
            }
            else if (type_name->nodeType==NODE_TYPE_REAL){
                fprintf(fp , ")F\n");
                function_type = type_real;
            }
            else{
                function_type = type_nothing;
            }
            fprintf(fp , "\t.limit locals 100\n\t.limit stack 100\n");
            function_procedure_scope = 1;
            break;
        }
        case NODE_PRO_HEAD:{
            fprintf(fp , ".method public static ");
            struct node * procedure_name = nthChild(1 , node);
            struct node * parameter = nthChild(2 , node);
            fprintf(fp , "%s(" , procedure_name->string);
            strcpy(function_var , procedure_name->string);
            if(parameter->nodeType != NODE_lambda){
                struct SymTableEntry * procedure = findSymbol_in_main(procedure_name->string);
                struct param_list * param = procedure->procedure->param;
                fill_param(param);
            }
            function_type = type_void;
            fprintf(fp , ")V\n");
            fprintf(fp , "\t.limit locals 100\n\t.limit stack 100\n");
            function_procedure_scope = 1; 
            break;
        }
        case NODE_END:{
            main_end--;
            gen_program_end();
            function_procedure_scope = 0;
            param_num = 0;
            return;
        }
        case NODE_DECL:{
        ///////////// no declaration /////////////          
            if(node->child == NULL)
                    return;
        ///////////// declaration ////////////////    
            else {
        /************** local declaration ***********/
                if(function_procedure_scope ==1){
                    if(node->parent->nodeType==NODE_LIST){
                        //fprintf(fp , "fuckleo\n");
                        init_local_var();
                    }
                    else 
                        param_num++;
                    return;
                }
        /*************** global declaration *************/        
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
            struct SymTableEntry *entry;
            if(scope_check_gen==1)
                entry = findSymbol_in_main(node->string);
            else 
                entry = findSymbol_in_function_procedure(node->string);
            if(entry->level == 1){
                if(left == 1){
                    if(entry->type == TypeInt)
                        fprintf(fp , "\tistore %d\n" , entry->var_num);
                    else if(entry->type == TypeReal)
                        fprintf(fp , "\tfstore %d\n" , entry->var_num);
                    ///// array not finished
                    else if(entry->type == TypeArray){
                        struct array_descriptor *temp=entry->array;
                        int dim=1;
                        while(temp->type == TypeArray){
                            dim++;
                            temp=temp->next_array;
                        }
                        enum StdType type = temp->type;
                        temp=entry->array;
                        struct array_descriptor *temp2=temp->next_array;
                        struct array_descriptor *temp3;
                        if(dim>=3)
                            temp3=temp2->next_array;
                        if(flag == 0){
                            struct node *child1=nthChild(1 , node);
                            struct node *child12;
                            struct node *child15;
                            struct node *child18;
                            switch(dim){
                                case 1:{
                                    child12 = nthChild(2 , child1);
                                    break;
                                }
                                case 2:{
                                    child12 = nthChild(2 , child1);
                                    child15 = nthChild(5 , child1);
                                    break; 
                                }
                                case 3:{
                                    child12 = nthChild(2 , child1);
                                    child15 = nthChild(5 , child1);
                                    child18 = nthChild(8 , child1);
                                    break;
                                }
                            }
                            fprintf(fp , "\taload_%d\n",entry->var_num);
                            for(int i=0 ; i<dim ; i++){
                                if(i==0){
                                    if(dim==1)
                                        fprintf(fp , "\tldc %d\n" , child12->iValue);
                                    else if(dim==2)
                                        fprintf(fp , "\tldc %d\n" , child15->iValue);
                                    else if(dim==3)
                                        fprintf(fp , "\tldc %d\n" , child18->iValue);
                                    fprintf(fp , "\tldc %d\n" , temp->array_begin);
                                    fprintf(fp , "\tisub\n");
                                    if(dim>1)
                                        fprintf(fp , "\taaload\n");
                                }
                                else if(i==1){
                                    if(dim==2)
                                        fprintf(fp , "\tldc %d\n" , child12->iValue);
                                    else if(dim==3)
                                        fprintf(fp , "\tldc %d\n" , child15->iValue);
                                    fprintf(fp , "\tldc %d\n" , temp2->array_begin);
                                    fprintf(fp , "\tisub\n");
                                    if(dim>2)
                                        fprintf(fp , "\taaload\n");
                                }
                                else if(i==2){
                                    fprintf(fp , "\tldc %d\n" , child12->iValue);
                                    fprintf(fp , "\tldc %d\n" , temp3->array_begin);
                                    fprintf(fp , "\tisub\n"); 
                                }
                            }
                        }
                        else{
                            switch(type){
                                case TypeInt:
                                    fprintf(fp , "\tiastore\n");
                                    break;
                                case TypeReal:
                                    fprintf(fp , "\tfastore\n");
                                    break;
                            }
                        }
                    }
                
                }
                else {
                    if(entry->type == TypeInt)
                        fprintf(fp , "\tiload %d\n" , entry->var_num);
                    else if(entry->type == TypeReal)
                        fprintf(fp , "\tfload %d\n" , entry->var_num);
                    else if(entry->type == TypeArray){
                        struct node *child2 = nthChild(2 , node);
                        struct array_descriptor *temp=entry->array;
                        int dim=1;
                        while(temp->type == TypeArray){
                            dim++;
                            temp=temp->next_array;
                        }
                        enum StdType type = temp->type;
                        struct node *child22;
                        struct node *child25;
                        struct node *child28;
                        switch(dim){
                            case 1:{
                                child22 = nthChild(2 , child2);
                                break;
                            }
                            case 2:{
                                child22 = nthChild(2 , child2);
                                child25 = nthChild(5 , child2);
                                break; 
                            }
                            case 3:{
                                child22 = nthChild(2 , child2);
                                child25 = nthChild(5 , child2);
                                child28 = nthChild(8 , child2);
                                break;
                            }
                        }
                        temp=entry->array;
                        struct array_descriptor *temp2=temp->next_array;
                        struct array_descriptor *temp3;
                        if(dim>=3)
                            temp3=temp2->next_array;
                        fprintf(fp , "\taload_%d\n",entry->var_num);
                        for(int i=0 ; i<dim ; i++){
                            if(i==0){
                                switch(dim){
                                    case 1:{
                                        fprintf(fp , "\tldc %d\n" , child22->iValue);
                                        break;
                                    }
                                    case 2:{
                                        fprintf(fp , "\tldc %d\n" , child25->iValue);
                                        break;
                                    }
                                    case 3:{
                                        fprintf(fp , "\tldc %d\n" , child28->iValue);
                                    }
                                }
                                fprintf(fp , "\tldc %d\n" , temp->array_begin);
                                fprintf(fp , "\tisub\n");
                                if(dim>1)
                                    fprintf(fp , "\taaload\n");
                                else{
                                    switch(type){
                                        case TypeInt:
                                            fprintf(fp , "\tiaload\n");
                                            break;
                                        case TypeReal:
                                            fprintf(fp , "\tfaload\n");
                                            break;
                                        case TypeString:
                                            fprintf(fp , "\tsaload\n");
                                            break;
                                        default :
                                            fprintf(fp , "\terror\n");
                                            break;
                                    }   
                                }
                            }
                            else if(i==1){
                                switch(dim){
                                    case 2:{
                                        fprintf(fp , "\tldc %d\n" , child22->iValue);
                                        break;
                                    }
                                    case 3:{
                                        fprintf(fp , "\tldc %d\n" , child25->iValue);
                                    }
                                }
                                fprintf(fp , "\tldc %d\n" , temp2->array_begin);
                                fprintf(fp , "\tisub\n");
                                if(dim>2)
                                    fprintf(fp , "\taaload\n");
                                else{
                                    switch(type){
                                        case TypeInt:
                                            fprintf(fp , "\tiaload\n");
                                            break;
                                        case TypeReal:
                                            fprintf(fp , "\tfaload\n");
                                            break;
                                    }   
                                }
                            }
                            else if(i==2){
                                fprintf(fp , "\tldc %d\n" , child22->iValue);
                                fprintf(fp , "\tldc %d\n" , temp3->array_begin);
                                fprintf(fp , "\tisub\n");
                                switch(type){
                                    case TypeInt:
                                        fprintf(fp , "\tiaload\n");
                                        break;
                                    case TypeReal:
                                        fprintf(fp , "\tfaload\n");
                                        break;
                                }  
                            }
                        }
                    }
                }  
            }
            else {
                if(left == 1){
                    if(entry->type == TypeInt)
                        fprintf(fp , "\tputstatic foo/%s I\n" , entry->name);
                    else if(entry->type == TypeReal)
                        fprintf(fp , "\tputstatic foo/%s F\n" , entry->name);
                    else if(entry->type == TypeArray){
                        struct array_descriptor *temp=entry->array;
                        int dim=1;
                        while(temp->type == TypeArray){
                            dim++;
                            temp=temp->next_array;
                        }
                        enum StdType type = temp->type;
                        temp=entry->array;
                        struct array_descriptor *temp2=temp->next_array;
                        struct array_descriptor *temp3;
                        if(dim>=3)
                            temp3=temp2->next_array;
                        if(flag == 0){
                            struct node *child1=nthChild(1 , node);
                            struct node *child12;
                            struct node *child15;
                            struct node *child18;
                            switch(dim){
                                case 1:{
                                    child12 = nthChild(2 , child1);
                                    break;
                                }
                                case 2:{
                                    child12 = nthChild(2 , child1);
                                    child15 = nthChild(5 , child1);
                                    break; 
                                }
                                case 3:{
                                    child12 = nthChild(2 , child1);
                                    child15 = nthChild(5 , child1);
                                    child18 = nthChild(8 , child1);
                                    break;
                                }
                            }
                            fprintf(fp , "\tgetstatic foo/%s ",entry->name);
                            for(int j=0;j<dim;j++){
                                fprintf(fp , "[");
                            }
                            switch(type){
                                case TypeInt:   
                                    fprintf(fp , "I\n");
                                    break;
                                case TypeReal:  
                                    fprintf(fp , "F\n");
                                    break;
                            }
                            for(int i=0 ; i<dim ; i++){
                                if(i==0){
                                    if(dim==1)
                                        fprintf(fp , "\tldc %d\n" , child12->iValue);
                                    else if(dim==2)
                                        fprintf(fp , "\tldc %d\n" , child15->iValue);
                                    else if(dim==3)
                                        fprintf(fp , "\tldc %d\n" , child18->iValue);
                                    fprintf(fp , "\tldc %d\n" , temp->array_begin);
                                    fprintf(fp , "\tisub\n");
                                    if(dim>1)
                                        fprintf(fp , "\taaload\n");
                                }
                                else if(i==1){
                                    if(dim==2)
                                        fprintf(fp , "\tldc %d\n" , child12->iValue);
                                    else if(dim==3)
                                        fprintf(fp , "\tldc %d\n" , child15->iValue);
                                    fprintf(fp , "\tldc %d\n" , temp2->array_begin);
                                    fprintf(fp , "\tisub\n");
                                    if(dim>2)
                                        fprintf(fp , "\taaload\n");
                                }
                                else if(i==2){
                                    fprintf(fp , "\tldc %d\n" , child12->iValue);
                                    fprintf(fp , "\tldc %d\n" , temp3->array_begin);
                                    fprintf(fp , "\tisub\n"); 
                                }
                            }
                        }
                        else{
                            switch(type){
                                case TypeInt:
                                    fprintf(fp , "\tiastore\n");
                                    break;
                                case TypeReal:
                                    fprintf(fp , "\tfastore\n");
                                    break;
                            }
                        }
                    }
                    ///// array  not finished
                
                }
                else {
                    //fprintf(fp , "getstatic java/lang/System/out Ljava/io/PrintStream;\n");
                    if(entry->type == TypeInt)
                        fprintf(fp , "\tgetstatic foo/%s I\n" , entry->name);
                    else if(entry->type == TypeReal)
                        fprintf(fp , "\tgetstatic foo/%s F\n" , entry->name);
                    else if(entry->type ==TypeFunction){
                        if(node->child != NULL){
                            travel_node(node->child);
                        }
                        fprintf(fp , "\tinvokestatic foo/%s(" , entry->name);
                        if(entry->function->param != NULL){
                            fill_param(entry->function->param);
                        }
                        if(entry->function->type == TypeInt)
                            fprintf(fp , ")I\n");
                        else if(entry->function->type == TypeReal)
                            fprintf(fp , ")F\n");
                        else 
                            fprintf(fp , ")V\n");
                    }
                    else if(entry->type == TypeProcedure){
                        if(node->child != NULL){
                            travel_node(node->child);
                        }
                        
                        fprintf(fp , "\tinvokestatic foo/%s(" , entry->name);
                        if(entry->function == NULL)

                        if(entry->procedure->param != NULL){
                            fill_param(entry->procedure->param);
                        }
                            fprintf(fp , ")V\n");
                    }
                    else if(entry->type == TypeArray){
                        struct node *parent = node->parent;
                        struct node *child2 = nthChild(2 , parent);
                        struct node *child21=   nthChild(1,child2);
                        struct array_descriptor *temp=entry->array;
                        int dim=1;
                        while(temp->type == TypeArray){
                            dim++;
                            temp=temp->next_array;
                        }
                        enum StdType type = temp->type;
                        struct node *child212;
                        struct node *child215;
                        struct node *child218;
                        switch(dim){
                            case 1:{
                                child212 = nthChild(2 , child21);
                                break;
                            }
                            case 2:{
                                child212 = nthChild(2 , child21);
                                child215 = nthChild(5 , child21);
                                break; 
                            }
                            case 3:{
                                child212 = nthChild(2 , child21);
                                child215 = nthChild(5 , child21);
                                child218 = nthChild(8 , child21);
                                break;
                            }
                        }
                        temp=entry->array;
                        struct array_descriptor *temp2=temp->next_array;
                        struct array_descriptor *temp3;
                        if(dim>=3)
                            temp3=temp2->next_array;
                        fprintf(fp , "\tgetstatic foo/%s ",entry->name);
                        for(int j=0;j<dim;j++){
                            fprintf(fp , "[");
                        }
                        
                        switch(type){
                            case TypeInt:   
                                fprintf(fp , "I\n");
                                break;
                            case TypeReal:  
                                fprintf(fp , "F\n");
                                break;
                        }
                        for(int i=0 ; i<dim ; i++){
                            if(i==0){
                                switch(dim){
                                    case 1:{
                                        fprintf(fp , "\tldc %d\n" , child212->iValue);
                                        break;
                                    }
                                    case 2:{
                                        fprintf(fp , "\tldc %d\n" , child215->iValue);
                                        break;
                                    }
                                    case 3:{
                                        fprintf(fp , "\tldc %d\n" , child218->iValue);
                                    }
                                }
                                fprintf(fp , "\tldc %d\n" , temp->array_begin);
                                fprintf(fp , "\tisub\n");
                                if(dim>1)
                                    fprintf(fp , "\taaload\n");
                                else{
                                    switch(type){
                                        case TypeInt:
                                            fprintf(fp , "\tiaload\n");
                                            break;
                                        case TypeReal:
                                            fprintf(fp , "\tfaload\n");
                                            break;
                                    }   
                                }
                            }
                            else if(i==1){
                                switch(dim){
                                    case 2:{
                                        fprintf(fp , "\tldc %d\n" , child212->iValue);
                                        break;
                                    }
                                    case 3:{
                                        fprintf(fp , "\tldc %d\n" , child215->iValue);
                                    }
                                }
                                fprintf(fp , "\tldc %d\n" , temp2->array_begin);
                                fprintf(fp , "\tisub\n");
                                if(dim>2)
                                    fprintf(fp , "\taaload\n");
                                else{
                                    switch(type){
                                        case TypeInt:
                                            fprintf(fp , "\tiaload\n");
                                            break;
                                        case TypeReal:
                                            fprintf(fp , "\tfaload\n");
                                            break;
                                    }   
                                }
                            }
                            else if(i==2){
                                fprintf(fp , "\tldc %d\n" , child212->iValue);
                                fprintf(fp , "\tldc %d\n" , temp3->array_begin);
                                fprintf(fp , "\tisub\n");
                                switch(type){
                                    case TypeInt:
                                        fprintf(fp , "\tiaload\n");
                                        break;
                                    case TypeReal:
                                        fprintf(fp , "\tfaload\n");
                                        break;
                                }  
                            }
                        }
                    }
                }  
            }

            return;
        }
        case NODE_OP:{
            struct node * child1 = nthChild(1 , node);
            struct node * child2 = nthChild(2 , node);
            ////////////// binary operation /////////////
            if(child1 != child2){
                travel_node(child1);
                travel_node(child2);
                switch(node->op){
                    case OP_ADD: {
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\tiadd\n");
                            return;
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfadd\n");
                            return;
                        }
                    }
                    case OP_SUB: {
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\tisub\n");
                            return;
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfsub\n");
                            return;
                        }
                    }
                    case OP_MUL:{
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\timul\n");
                            return;
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfmul\n");
                            return;
                        }
                    }
                    case OP_DIV:{
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\tidiv\n");
                            return;
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfdiv\n");
                            return;
                        }
                    }
                    case OP_LT: {
                        label_count++;
                        stack_top++;
                        label_stack[stack_top]=label_count;
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\tisub\n");
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfcmpl\n");
                        }
                        fprintf(fp , "\tiflt Ltrue_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_0\n");
                        fprintf(fp , "\tgoto Lfalse_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "Ltrue_%d:\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_1\n");
                        fprintf(fp , "Lfalse_%d:\n" , label_stack[stack_top]);
                        stack_top--;
                        return;
                    }
                    case OP_GT:{
                        label_count++;
                        stack_top++;
                        label_stack[stack_top]=label_count;
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\tisub\n");
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfcmpl\n");
                        }
                        fprintf(fp , "\tifgt Ltrue_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_0\n");
                        fprintf(fp , "\tgoto Lfalse_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "Ltrue_%d:\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_1\n");
                        fprintf(fp , "Lfalse_%d:\n" , label_stack[stack_top]);
                        stack_top--;
                        return;

                    }
                    case OP_EQ:{
                        label_count++;
                        stack_top++;
                        label_stack[stack_top]=label_count;
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\tisub\n");
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfcmpl\n");
                        }
                        fprintf(fp , "\tifeq Ltrue_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_0\n");
                        fprintf(fp , "\tgoto Lfalse_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "Ltrue_%d:\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_1\n");
                        fprintf(fp , "Lfalse_%d:\n" , label_stack[stack_top]);
                        stack_top--;
                        return;
                    }
                    case OP_NE:{
                        label_count++;
                        stack_top++;
                        label_stack[stack_top]=label_count;
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\tisub\n");
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfcmpl\n");
                        }
                        fprintf(fp , "\tifne Ltrue_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_0\n");
                        fprintf(fp , "\tgoto Lfalse_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "Ltrue_%d:\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_1\n");
                        fprintf(fp , "Lfalse_%d:\n" , label_stack[stack_top]);
                        stack_top--;
                        return;

                    }
                    case OP_GE:{
                        label_count++;
                        stack_top++;
                        label_stack[stack_top]=label_count;
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\tisub\n");
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfcmpl\n");
                        }
                        fprintf(fp , "\tifge Ltrue_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_0\n");
                        fprintf(fp , "\tgoto Lfalse_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "Ltrue_%d:\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_1\n");
                        fprintf(fp , "Lfalse_%d:\n" , label_stack[stack_top]);
                        stack_top--;
                        return;

                    }
                    case OP_LE:{
                        label_count++;
                        stack_top++;
                        label_stack[stack_top]=label_count;
                        if(child1->valueType == TypeInt){
                            fprintf(fp , "\tisub\n");
                        }
                        else if(child1->valueType == TypeReal){
                            fprintf(fp , "\tfcmpl\n");
                        }
                        fprintf(fp , "\tifle Ltrue_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_0\n");
                        fprintf(fp , "\tgoto Lfalse_%d\n" , label_stack[stack_top]);
                        fprintf(fp , "Ltrue_%d:\n" , label_stack[stack_top]);
                        fprintf(fp , "\ticonst_1\n");
                        fprintf(fp , "Lfalse_%d:\n" , label_stack[stack_top]);
                        stack_top--;
                        return;

                    }
                }
            }
            break;
        }
        case NODE_IF:{
            travel_node(nthChild(1,node));
            stack_top++;
            label_count++;
            label_stack[stack_top]=label_count;
            fprintf(fp , "\tifeq Lfalse_%d\n" , label_stack[stack_top]);
            travel_node(nthChild(2,node));
            fprintf(fp , "\tgoto Lexit_%d\nLfalse_%d:\n" , label_stack[stack_top] , label_stack[stack_top]);
            travel_node(nthChild(3,node));

            fprintf(fp , "Lexit_%d:\n" , label_stack[stack_top]);
            stack_top--;
            return;
        }
        case NODE_WHILE:{
            stack_top++;
            label_count++;
            label_stack[stack_top]=label_count;
            fprintf(fp , "Lbegin_%d:\n" , label_stack[stack_top]);
            travel_node(nthChild(1,node));
            fprintf(fp , "\tifeq Lexit_%d\n" , label_stack[stack_top]);
            travel_node(nthChild(2,node));
            fprintf(fp , "\tgoto Lbegin_%d\nLexit_%d:\n" , label_stack[stack_top] , label_stack[stack_top]);
            stack_top--;
            return;
        }
        case NODE_INT:{
            fprintf(fp , "\tldc %d\n" , node->iValue);
            return;
        }
        case NODE_REAL:{
            fprintf(fp , "\tldc %f\n" , node->rValue);
            return;
        }
        case NODE_STRING_v:{
            fprintf(fp , "\tldc %s\n" , node->string);
            return;
        }
        case NODE_ASSIGN_STMT:{
            struct node * child1 = nthChild(1 , node);
            struct node * child2 = nthChild(2 , node);
            if(child1->array==NULL){
                left = 0;
                travel_node(child2);
                left =1;
                travel_node(child1);
                left = 0;
            }
            else{
                left=1;
                travel_node(child1);
                left=0;
                travel_node(child2);
                left=1;
                flag=1;
                travel_node(child1);
                left=0;
                flag=0;
            }
            return;
        }
        case NODE_READ:{
            fprintf(fp, "\tgetstatic foo/_sc Ljava/util/Scanner;\n");
            struct node * input = nthChild(1 , node);
            struct SymTableEntry * entry;
            if(scope_check_gen==1)
                entry = findSymbol_in_main(input->string);
            else 
                entry = findSymbol_in_function_procedure(input->string);
                if(entry->type == TypeInt){
                    fprintf(fp , "\tinvokevirtual java/util/Scanner/nextInt()I\n");
                    left=1;
                    travel_node(input);
                    left=0;
                }
                else if(entry->type == TypeReal){
                    fprintf(fp , "\tinvokevirtual java/util/Scanner/nextFloat()F\n");
                    left=1;
                    travel_node(input);
                    left=0;
                }
            return;
        }
        case NODE_WRITELN: {
            fprintf(fp , "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
            travel_node(nthChild(1,node));
            if(nthChild(1,node)->valueType == TypeInt)
                fprintf(fp, "\tinvokestatic java/lang/String/valueOf(I)Ljava/lang/String;\n");
	        else if(nthChild(1,node)->valueType == TypeReal)
                fprintf(fp, "\tinvokestatic java/lang/String/valueOf(F)Ljava/lang/String;\n");

            fprintf(fp , "\tinvokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n");
            return;
        }
    }
    struct node *child = node->child;
    if(child != NULL) {
        do {
            travel_node(child);
            child = child->rsibling;
        } while(child != node->child);
    }
}